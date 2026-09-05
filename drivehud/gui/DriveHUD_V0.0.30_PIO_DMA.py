import re
import time
import tkinter as tk
from tkinter import ttk

try:
    import serial
    from serial.tools import list_ports
except ImportError:
    serial = None
    list_ports = None

BAUD = 115200
STALL_TIMEOUT_SEC = 1.25
DEFAULT_PORT = "COM14"

motor_re = re.compile(r"MOTOR\s+state=(\d+)")
phase_re = re.compile(r"PHASE\s+old=(\d+)\s+new=(\d+)\s+delta=(\d+)\s+motor=(\d+)")
track_re = re.compile(r"TRACK_WRITE\s+addr=\$0022\s+data=\$[0-9A-Fa-f]+\s+\((\d+)\)")
status_re = re.compile(r"STATUS\s+(V[0-9.]+)")
state_re = re.compile(r"STATE\s+(V[0-9.]+)")
status_wp_re = re.compile(r"\bWPV=(\d+)\s+WP=(\d+)")
status_track_re = re.compile(r"\bTV=(\d+)\s+T=(\d+)")
status_pos_re = re.compile(r"\bTPV=(\d+)\s+TP2=(\d+)")
status_density_re = re.compile(r"\bDV=(\d+)\s+D=(\d+)")
status_motor_re = re.compile(r"\bM=(\d+)")
density_re = re.compile(r"DENSITY\s+state=(\d+)")
wp_re = re.compile(r"WRITE_PROTECT\s+state=(\d+)")


class DriveHUD:
    def __init__(self, root):
        self.root = root
        self.root.title("DriveHUD V0.5.30")
        self.root.geometry("560x455")
        self.root.resizable(False, False)

        self.ser = None
        self.rxbuf = ""
        self.pos2 = None
        self.home_seen = False
        self.motor_on = False
        self.direction = 0  # +1 IN/away from Track 1, -1 OUT/toward Track 1
        self.last_move_time = None
        self.last_phase = None
        self.write_protected = None
        self._wp_pending_state = None
        self._wp_after_id = None
        self._track_after_id = None
        self._track_candidate = None
        self._track_candidate_after_id = None
        self._last_phase_rx_time = None
        self.density = 2
        self.density_valid = False

        self.port_var = tk.StringVar(value=DEFAULT_PORT)
        self.conn_var = tk.StringVar(value="Disconnected")
        self.fw_var = tk.StringVar(value="Firmware: --")
        self.track_var = tk.StringVar(value="--.-")
        self.motor_var = tk.StringVar(value="OFF")
        self.head_var = tk.StringVar(value="PARK")
        self.wp_var = tk.StringVar(value="--")
        self.density_var = tk.StringVar(value="D2")
        self.home_var = tk.StringVar(value="HOME: waiting for $0022 = 1")

        self._build_ui()
        self.root.after(20, self.poll)
        self.root.after(250, self.tick)

    def _build_ui(self):
        pad = {"padx": 12, "pady": 8}
        top = ttk.Frame(self.root)
        top.pack(fill="x", padx=12, pady=(12, 4))
        ttk.Label(top, text="Port:").pack(side="left")
        self.port_box = ttk.Combobox(top, textvariable=self.port_var, width=12)
        self.port_box.pack(side="left", padx=(6, 8))
        ttk.Button(top, text="Refresh", command=self.refresh_ports).pack(side="left", padx=4)
        ttk.Button(top, text="Connect", command=self.connect).pack(side="left", padx=4)
        ttk.Button(top, text="Disconnect", command=self.disconnect).pack(side="left", padx=4)
        ttk.Label(top, textvariable=self.conn_var).pack(side="right")

        ttk.Separator(self.root).pack(fill="x", padx=12, pady=8)

        body = ttk.Frame(self.root)
        body.pack(fill="both", expand=True, padx=18)

        ttk.Label(body, text="TRACK", font=("Segoe UI", 12, "bold")).grid(row=0, column=0, sticky="w", **pad)
        ttk.Label(body, textvariable=self.track_var, font=("Consolas", 38, "bold")).grid(row=0, column=1, sticky="w", **pad)

        ttk.Label(body, text="MOTOR", font=("Segoe UI", 12, "bold")).grid(row=1, column=0, sticky="w", **pad)
        ttk.Label(body, textvariable=self.motor_var, font=("Consolas", 22, "bold")).grid(row=1, column=1, sticky="w", **pad)

        ttk.Label(body, text="HEAD", font=("Segoe UI", 12, "bold")).grid(row=2, column=0, sticky="w", **pad)
        ttk.Label(body, textvariable=self.head_var, font=("Consolas", 22, "bold")).grid(row=2, column=1, sticky="w", **pad)

        ttk.Label(body, text="WRITE PROTECT", font=("Segoe UI", 12, "bold")).grid(row=3, column=0, sticky="w", **pad)
        ttk.Label(body, textvariable=self.wp_var, font=("Consolas", 22, "bold")).grid(row=3, column=1, sticky="w", **pad)

        ttk.Label(body, text="DENSITY", font=("Segoe UI", 12, "bold")).grid(row=4, column=0, sticky="w", **pad)
        ttk.Label(body, textvariable=self.density_var, font=("Consolas", 22, "bold")).grid(row=4, column=1, sticky="w", **pad)

        ttk.Label(body, textvariable=self.home_var).grid(row=5, column=0, columnspan=2, sticky="w", **pad)
        ttk.Label(body, textvariable=self.fw_var).grid(row=6, column=0, columnspan=2, sticky="w", **pad)

        self.refresh_ports()

    def refresh_ports(self):
        ports = []
        if list_ports is not None:
            ports = [p.device for p in list_ports.comports()]
        self.port_box["values"] = ports
        if self.port_var.get() not in ports and ports and self.port_var.get() == "":
            self.port_var.set(ports[0])

    def connect(self):
        if serial is None:
            self.conn_var.set("pyserial not installed")
            return
        self.disconnect()
        try:
            # V0.5.30 late-connect fix:
            # TinyUSB's tud_cdc_connected() follows the host CDC/DTR state.
            # Open with DTR explicitly LOW, then raise it after the port is open.
            # This guarantees the RP2350 sees a fresh disconnect->connect edge
            # and re-sends its cached STATE snapshot.
            ser = serial.Serial()
            ser.port = self.port_var.get().strip()
            ser.baudrate = BAUD
            ser.timeout = 0
            ser.dtr = False
            ser.rts = False
            ser.open()

            self.ser = ser
            self.rxbuf = ""
            self.conn_var.set(f"Connected {self.port_var.get().strip()}")

            # Leave DTR low long enough for firmware's 1 ms USB task to observe
            # the disconnected state, then assert it. Do not sleep/block Tk.
            self.root.after(150, self._assert_cdc_dtr)
        except Exception as exc:
            self.ser = None
            self.conn_var.set(f"Connect failed: {exc}")

    def _assert_cdc_dtr(self):
        if self.ser is None:
            return
        try:
            self.ser.dtr = True
        except Exception as exc:
            self.conn_var.set(f"Serial DTR error: {exc}")

    def disconnect(self):
        if self.ser is not None:
            try:
                self.ser.dtr = False
            except Exception:
                pass
            try:
                self.ser.close()
            except Exception:
                pass
        self.ser = None
        self.conn_var.set("Disconnected")

    def _render_track_now(self):
        self._track_after_id = None
        if self.pos2 is None:
            self.track_var.set("--.-")
            return
        whole = self.pos2 // 2
        half = ".5" if self.pos2 & 1 else ".0"
        self.track_var.set(f"{whole:02d}{half}")

    def show_track(self, immediate=False):
        # Position math remains immediate. Only the visible label is stabilized.
        if immediate:
            if self._track_after_id is not None:
                try:
                    self.root.after_cancel(self._track_after_id)
                except Exception:
                    pass
                self._track_after_id = None
            self._render_track_now()
            return

        if self._track_after_id is not None:
            try:
                self.root.after_cancel(self._track_after_id)
            except Exception:
                pass
        self._track_after_id = self.root.after(120, self._render_track_now)

    def _schedule_initial_track_candidate(self, value):
        if self.pos2 is not None or not (1 <= value <= 42):
            return
        self._track_candidate = value
        if self._track_candidate_after_id is not None:
            try:
                self.root.after_cancel(self._track_candidate_after_id)
            except Exception:
                pass
        self._track_candidate_after_id = self.root.after(250, self._commit_initial_track_candidate)

    def _commit_initial_track_candidate(self):
        self._track_candidate_after_id = None
        if self.pos2 is not None or self._track_candidate is None:
            return

        # DOS may write the destination track to $0022 before the seek.
        # Wait until phase traffic is quiet before using it as the initial anchor.
        if self._last_phase_rx_time is not None:
            age = time.monotonic() - self._last_phase_rx_time
            if age < 0.250:
                delay_ms = max(25, int((0.250 - age) * 1000) + 10)
                self._track_candidate_after_id = self.root.after(
                    delay_ms, self._commit_initial_track_candidate
                )
                return

        self.pos2 = self._track_candidate * 2
        self.show_track(immediate=True)
        self.home_var.set(f"TRACK: initialized from DOS $0022 = {self._track_candidate}")

    def process_motor(self, state):
        self.motor_on = bool(state)
        self.motor_var.set("ON" if self.motor_on else "OFF")
        if not self.motor_on:
            self.direction = 0
            self.head_var.set("PARK")
        else:
            # Do not reset phase here.  A legitimate first half-step may occur
            # immediately around motor-on and must remain countable.
            if self.last_move_time is None:
                self.last_move_time = time.monotonic()
            self.update_head_state()

    def process_phase(self, old, new, delta, event_motor):
        # : stepper position is independent of spindle-motor state.
        #
        # The 1541 Diagnostic Cartridge manual half-step path performs
        # read/modify/write arithmetic on UC2 ORB ($1C00). At phase wrap
        # boundaries (3->0 or 0->3), carry/borrow can transiently clear PB2,
        # which is also the spindle-motor bit. The PHASE event is still a real
        # head step and must not be discarded merely because that same ORB
        # write reports motor=0.
        #
        # MOTOR records alone own displayed spindle state. The embedded
        # event_motor field is intentionally retained in the wire protocol for
        # diagnostics, but it never gates physical step counting.
        self.last_phase = new
        self._last_phase_rx_time = time.monotonic()

        # Preserve the proven position arithmetic exactly.
        if delta == 1:
            self.direction = 1
            if self.pos2 is not None:
                self.pos2 += 1
                self.show_track()
            self.last_move_time = time.monotonic()
            self.head_var.set("IN" if self.motor_on else "PARK")
        elif delta == 3:
            self.direction = -1
            if self.pos2 is not None:
                self.pos2 = max(2, self.pos2 - 1)
                self.show_track()
            self.last_move_time = time.monotonic()
            self.head_var.set("OUT" if self.motor_on else "PARK")
        elif delta == 2:
            # Ambiguous two-state jump: intentionally ignored.
            pass

    def update_head_state(self):
        if not self.motor_on:
            self.head_var.set("PARK")
            return
        if self.last_move_time is not None and (time.monotonic() - self.last_move_time) >= STALL_TIMEOUT_SEC:
            self.direction = 0
            self.head_var.set("STALL")
            return
        if self.direction > 0:
            self.head_var.set("IN")
        elif self.direction < 0:
            self.head_var.set("OUT")

    def process_density(self, state):
        self.density = int(state) & 3
        self.density_valid = True
        self.density_var.set(f"D{self.density}")

    def process_write_protect(self, state):
        # GUI-only stabilization. Firmware acquisition remains the proven V0.0.19 logic.
        # Last WP event wins after 150 ms; no firmware/DMA/PIO state is altered.
        self._wp_pending_state = bool(state)
        if getattr(self, "_wp_after_id", None) is not None:
            try:
                self.root.after_cancel(self._wp_after_id)
            except Exception:
                pass
        self._wp_after_id = self.root.after(150, self._commit_write_protect)

    def _commit_write_protect(self):
        self._wp_after_id = None
        self.write_protected = self._wp_pending_state
        self.wp_var.set("PROTECTED" if self.write_protected else "WRITE ENABLED")

    def process_line(self, line):
        m = state_re.search(line)
        if m:
            self.fw_var.set("Firmware: " + m.group(1))

            mwp = status_wp_re.search(line)
            if mwp and int(mwp.group(1)):
                self.process_write_protect(int(mwp.group(2)))

            md = status_density_re.search(line)
            if md and int(md.group(1)):
                self.process_density(int(md.group(2)))

            mm = status_motor_re.search(line)
            if mm:
                self.process_motor(int(mm.group(1)))

            # RP2350 SRAM snapshot is authoritative on connect/reconnect.
            mp = status_pos_re.search(line)
            if mp and int(mp.group(1)):
                self.pos2 = max(2, int(mp.group(2)))
                self._track_candidate = None
                if self._track_candidate_after_id is not None:
                    try:
                        self.root.after_cancel(self._track_candidate_after_id)
                    except Exception:
                        pass
                    self._track_candidate_after_id = None
                self.show_track(immediate=True)
            else:
                mt = status_track_re.search(line)
                if mt and int(mt.group(1)):
                    tv = int(mt.group(2))
                    if tv == 1:
                        self.pos2 = 2
                        self.home_seen = True
                        self.home_var.set("HOME: anchored at Track 1.0")
                        self.show_track(immediate=True)
                    else:
                        self._schedule_initial_track_candidate(tv)
            return

        m = status_re.search(line)
        if m:
            self.fw_var.set("Firmware: " + m.group(1))

        m = density_re.search(line)
        if m:
            self.process_density(int(m.group(1)))
            return

        m = wp_re.search(line)
        if m:
            self.process_write_protect(int(m.group(1)))
            return

        m = motor_re.search(line)
        if m:
            self.process_motor(int(m.group(1)))
            return

        m = track_re.search(line)
        if m:
            value = int(m.group(1))
            if value == 1:
                # Proven strong synchronization point: physical HOME/bump.
                self.pos2 = 2
                self.home_seen = True
                self._track_candidate = None
                if self._track_candidate_after_id is not None:
                    try:
                        self.root.after_cancel(self._track_candidate_after_id)
                    except Exception:
                        pass
                    self._track_candidate_after_id = None
                self.home_var.set("HOME: anchored at Track 1.0")
                self.show_track(immediate=True)
            elif self.pos2 is None:
                # Non-home $0022 values are startup anchors only.
                # Once phase tracking exists, they cannot overwrite it.
                self._schedule_initial_track_candidate(value)
            return

        m = phase_re.search(line)
        if m:
            self.process_phase(int(m.group(1)), int(m.group(2)), int(m.group(3)), int(m.group(4)))

    def poll(self):
        if self.ser is not None:
            try:
                data = self.ser.read(4096)
                if data:
                    self.rxbuf += data.decode("ascii", errors="ignore")
                    while "\n" in self.rxbuf:
                        line, self.rxbuf = self.rxbuf.split("\n", 1)
                        self.process_line(line.strip())
            except Exception as exc:
                self.conn_var.set(f"Serial error: {exc}")
                self.disconnect()
        self.root.after(20, self.poll)

    def tick(self):
        self.update_head_state()
        self.root.after(100, self.tick)


if __name__ == "__main__":
    root = tk.Tk()
    app = DriveHUD(root)
    root.mainloop()
