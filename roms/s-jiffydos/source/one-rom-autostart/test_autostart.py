"""Exercise the assembled 6502 bootstrap against a small mocked OneROM bus.

This checks CPU-side copy/verify and RBCP byte framing. It does not emulate the
RP2350 PIO/address monitor or qualify timing on a physical 1541/OneROM.
Install py65 with requirements-test.txt before running this file.
"""

from pathlib import Path
import os
import sys

from py65.devices.mpu6502 import MPU


HERE = Path(__file__).resolve().parent
REPO = HERE.parents[3]
IMAGE = Path(os.environ.get(
    "AUTOSTART_IMAGE",
    str(REPO / "roms/private/s-jiffydos-autostart/s-jiffydos-autostart-e000-ffff.bin"),
))
RUNTIME_FLASH_SLOT = int(os.environ.get("AUTOSTART_RUNTIME_FLASH_SLOT", "10"), 0)
BOOT_PC = 0xE000


class MockBus:
    def __init__(self, corrupt_write=None):
        self.mem = bytearray(0x10000)
        self.events = []
        self.token = 0
        self.corrupt_write = corrupt_write
        self.data = bytearray(8)
        self.data[0:2] = bytes((2, 0))  # two RAM slots; slot zero is active

    def __len__(self):
        return 0x10000

    def __getitem__(self, addr):
        if isinstance(addr, slice):
            return self.mem[addr]
        addr &= 0xFFFF
        if 0xC000 <= addr <= 0xC0FF:
            value = addr & 0xFF
            self.events.append(value)
            return value  # ROM-bus RBCP writes are reads at C000 + byte.
        if addr == 0xC102:
            self.token = (self.token + 1) & 0xFF
            return self.token
        if addr == 0xC104:
            return 0xBB
        if addr == 0xC105:
            return 0xCC
        if 0xC108 <= addr <= 0xC10F:
            return self.data[addr - 0xC108]
        return self.mem[addr]

    def __setitem__(self, addr, value):
        if isinstance(addr, slice):
            self.mem[addr] = value
            return
        addr &= 0xFFFF
        value &= 0xFF
        if addr == self.corrupt_write:
            value ^= 0x01
        self.mem[addr] = value


def contains_subsequence(haystack, needle):
    n = len(needle)
    return any(haystack[i : i + n] == needle for i in range(len(haystack) - n + 1))


def initialize(bus, rom, source):
    bus.mem[0xE000:0x10000] = rom
    bus.mem[0xA000:0xC000] = source
    bus.mem[0xFFFC:0xFFFE] = bytes((0x00, 0xE0))


def main():
    if not IMAGE.exists():
        raise SystemExit(f"missing assembled ROM page: {IMAGE}")
    rom = IMAGE.read_bytes()
    assert len(rom) == 8192, f"expected 8 KiB E-page, got {len(rom)} bytes"

    bus = MockBus()
    source = bytes(((i * 37 + 11) & 0xFF) for i in range(8192))
    initialize(bus, rom, source)
    assert rom[-6:] == bytes((0x00, 0xE0)) * 3, "ROM vectors do not point to the bootstrap"

    cpu = MPU(memory=bus)
    cpu.pc = BOOT_PC
    switched = False
    for steps in range(2_000_000):
        old_pc = cpu.pc
        cpu.step()
        if contains_subsequence(bus.events, [0x00, 0x04, 0x01]):
            switched = True
        # The initial PC is E000; after the successful terminal switch, the
        # reset-vector jump returns there. The bus stream confirms switch first.
        if switched and old_pc != BOOT_PC and cpu.pc == BOOT_PC:
            break
    else:
        raise AssertionError("bootstrap did not reach the runtime reset vector")

    copied = bytes(bus.mem[0x8000:0xA000])
    assert copied == source, "RAMBoard shadow copy did not match its 8 KiB source"
    assert contains_subsequence(
        bus.events,
        [0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x02, 0xBB, 0xCC],
    ), "ENTER_CMD_RESP bytes differ from the configured RBCP session"
    assert contains_subsequence(bus.events, [0x01, 0x03]), "slot-info query missing"
    assert contains_subsequence(bus.events, [0x02, 0x02, 0x01, RUNTIME_FLASH_SLOT]), (
        f"expected LOAD_SLOT to copy selectable flash slot {RUNTIME_FLASH_SLOT} "
        "into inactive RAM slot 1"
    )
    assert contains_subsequence(bus.events, [0x00, 0x04, 0x01]), (
        "expected SWITCH_AND_EXIT to activate RAM slot 1"
    )
    print(f"PASS: executed {steps + 1} 6502 instructions")
    print("PASS: copied and verified all 8192 shadow bytes at $8000-$9FFF")
    print(f"PASS: emitted ENTER_CMD_RESP, GET_RAM_SLOT_INFO_ALL, LOAD_SLOT(1,{RUNTIME_FLASH_SLOT}), SWITCH_AND_EXIT(1)")
    print("PASS: jumped through the new runtime reset vector")

    bad_bus = MockBus(corrupt_write=0x8123)
    initialize(bad_bus, rom, source)
    bad_cpu = MPU(memory=bad_bus)
    bad_cpu.pc = BOOT_PC
    previous_pc = None
    for _ in range(200_000):
        current_pc = bad_cpu.pc
        bad_cpu.step()
        if previous_pc == current_pc == bad_cpu.pc:
            break
        previous_pc = current_pc
    else:
        raise AssertionError("corrupt-copy case did not stop in the failure loop")
    assert not contains_subsequence(bad_bus.events, [0x00, 0x01]), (
        "bootstrap continued into RBCP after a corrupted copy"
    )
    assert bad_bus.mem[0x8123] != source[0x123], "fault injection did not corrupt the destination"
    print("PASS: a corrupted destination byte stops before any RBCP command")
    print("LIMIT: RBCP responses and OneROM switching are mocked; hardware timing remains untested")


if __name__ == "__main__":
    main()
