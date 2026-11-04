"""Regression tests for Monitor head-state telemetry."""

import unittest
from unittest.mock import patch
from pathlib import Path
import importlib.util
import sys

module_path = Path(__file__).resolve().parents[1] / "source" / "1541HUD_V0.8.0.51.py"
spec = importlib.util.spec_from_file_location("onerom_hud", module_path)
touchscreen = importlib.util.module_from_spec(spec)
assert spec and spec.loader
sys.modules[spec.name] = touchscreen
spec.loader.exec_module(touchscreen)
HUDApp = touchscreen.OneROM1541HUDApp
BoardDescriptor = touchscreen.BoardDescriptor
CdcBoardLink = touchscreen.CdcBoardLink
DriveBinding = touchscreen.DriveBinding
DriveTelemetryParser = touchscreen.DriveTelemetryParser
track_is_over_dos_range = touchscreen.track_is_over_dos_range
MAX_RX_BUFFER_BYTES = touchscreen.MAX_RX_BUFFER_BYTES
load_binding = touchscreen.load_binding
save_binding = touchscreen.save_binding
write_protect_card_presentation = touchscreen.write_protect_card_presentation
select_command_cdc_boards = touchscreen.select_command_cdc_boards
discover_cdc_board_report = touchscreen.discover_cdc_board_report


def mock_monotonic(value: float):
    """Patch the in-process touchscreen module despite its numeric filename."""
    return patch.object(touchscreen.time, "monotonic", return_value=value)


class WriteProtectCardPresentationTests(unittest.TestCase):
    """The card must only advertise hardware that is actually attached."""

    def test_control_only_is_an_action_without_invented_sensor_state(self) -> None:
        self.assertEqual(
            write_protect_card_presentation(True, False, True, False),
            ("", "", True),
        )

    def test_monitor_only_never_creates_a_write_protect_card(self) -> None:
        self.assertEqual(
            write_protect_card_presentation(False, True, True, False),
            ("", "", False),
        )

    def test_monitor_state_cannot_replace_control_write_protect_state(self) -> None:
        self.assertEqual(
            write_protect_card_presentation(True, True, False, True, True, True),
            ("FORCES WRITABLE", "OVERRIDE ON", True),
        )

    def test_control_sel_d_sense_shows_physical_state_without_monitor(self) -> None:
        self.assertEqual(
            write_protect_card_presentation(True, False, None, False, True, True),
            ("DISK PROTECTED", "", True),
        )

    def test_control_sel_d_sense_is_visible_when_override_button_is_disabled(self) -> None:
        self.assertEqual(
            write_protect_card_presentation(True, False, None, False, True, False, False, False),
            ("DISK WRITABLE", "", False),
        )

    def test_control_sel_d_sense_reports_masked_during_override(self) -> None:
        self.assertEqual(
            write_protect_card_presentation(True, False, None, True, True, None, True),
            ("FORCES WRITABLE", "PHYSICAL SENSOR MASKED", True),
        )

    def test_controller_reply_decodes_sel_d_physical_state(self) -> None:
        target = HUDApp.__new__(HUDApp)
        target._controller_transactions = {"write_protect_query": "timer"}
        target._pending_wp_override = None
        target._confirmed_writable = False
        target.controller_wp_available = None
        target.controller_wp_sense_available = False
        target.controller_wp_protected = None
        target.controller_wp_sense_masked = False
        target.finish_controller_transaction = lambda card: target._controller_transactions.pop(card, None)
        target.open_size_preview = lambda: None
        target.update_live_hud_fields = lambda: None

        class Var:
            def set(self, value): self.value = value

        class Status:
            def set(self, value): self.value = value

        target.writable = Var()
        target.control_status = Status()
        target.handle_controller_wp_reply(
            "$ROMTEST,WP,OK,AVAILABLE=1,STATE=OFF,SENSE_AVAILABLE=1,PHYSICAL=PROTECTED"
        )
        self.assertTrue(target.controller_wp_sense_available)
        self.assertTrue(target.controller_wp_protected)
        self.assertFalse(target.controller_wp_sense_masked)


class ApplyViewportRestoreTests(unittest.TestCase):
    """A Control reboot must not strand the operator at a stale viewport."""

    def test_rom_apply_restores_the_operated_card_after_reconnect(self) -> None:
        target = HUDApp.__new__(HUDApp)
        target._rom_apply_focus_card = "startup_rom"
        target.hud_scroll_index = 0
        target.scroll_hud_cards = lambda: [
            {"id": f"diagnostic_{number}"} for number in range(7)
        ] + [{"id": "startup_rom"}]
        target.refresh_hud_subscription = lambda: None
        target.request_preview_redraw = lambda: setattr(target, "redraw_requested", True)

        HUDApp.restore_rom_apply_focus(target)

        # Five cards fit onscreen.  Card 8 must be visible on the last
        # viewport, rather than preserving the unrelated top-of-list offset.
        self.assertEqual(target.hud_scroll_index, 3)
        self.assertIsNone(target._rom_apply_focus_card)
        self.assertTrue(target.redraw_requested)


class AsyncConnectionPaintTests(unittest.TestCase):
    """CDC completion must repaint the Canvas without a page change."""

    class _Var:
        def __init__(self, value=False):
            self.value = value

        def set(self, value):
            self.value = value

        def get(self):
            return self.value

    class _Status:
        def __init__(self):
            self.value = ""

        def set(self, value):
            self.value = value

        def get(self):
            return self.value

    def test_async_connection_completion_requests_canvas_redraw(self) -> None:
        target = HUDApp.__new__(HUDApp)
        target._opening_roles = {"controller"}
        target.usb_links = {}
        target.drive_binding = DriveBinding(drive_id="1541 Drive")
        target.binding_path = Path("unused.json")
        target.ub3 = self._Var()
        target.ub4 = self._Var()
        target.usb_status = self._Status()
        target.after = lambda *_args: None
        target.append_usb_log = lambda *_args: None
        target.apply_device_state = lambda: None
        painted = []
        target.request_preview_redraw = lambda: painted.append(True)
        link = type("Link", (), {
            "connected": True,
            "board": BoardDescriptor("CONTROL-1", "COM99", "Control OneROM"),
            "assert_dtr": lambda self: None,
        })()
        binding = DriveBinding(drive_id="1541 Drive", controller_serial="CONTROL-1")
        with patch.object(touchscreen, "save_binding"):
            target._finish_connect_role("controller", binding, link, None)
        self.assertIs(target.usb_links["controller"], link)
        self.assertTrue(target.ub3.get())
        self.assertEqual(painted, [True])

    def test_stale_async_completion_closes_without_restoring_released_binding(self) -> None:
        target = HUDApp.__new__(HUDApp)
        target._opening_roles = {"controller"}
        target._role_generation = {"controller": 2, "hud": 0}
        target._shutting_down = False
        target._close_link_async = lambda current, *_args: current.close()
        link = type("Link", (), {
            "connected": True,
            "closed": False,
            "close": lambda self: setattr(self, "closed", True),
        })()
        # Generation 1 belongs to an open started before the operator released
        # the role. It must not win after the release invalidated it.
        target._finish_connect_role("controller", DriveBinding(drive_id="1541 Drive"), link, None, 1)
        self.assertTrue(link.closed)
        self.assertNotIn("usb_links", target.__dict__)

    def test_worker_completion_queue_runs_only_when_main_loop_drains_it(self) -> None:
        target = HUDApp.__new__(HUDApp)
        target._ui_tasks = touchscreen.queue.SimpleQueue()
        target._shutting_down = False
        target.append_usb_log = lambda *_args: self.fail("queued completion should not fail")
        observed = []
        target.queue_ui_task(lambda: observed.append("main-thread"))
        self.assertEqual(observed, [])
        target.drain_ui_tasks()
        self.assertEqual(observed, ["main-thread"])


class PopupChoiceViewportTests(unittest.TestCase):
    """Long ROM discovery lists must remain usable on the fixed touch display."""

    def test_long_choice_list_scrolls_in_bounded_seven_row_viewport(self) -> None:
        target = HUDApp.__new__(HUDApp)
        choices = tuple(f"Slot {index} — ROM {index}" for index in range(1, 13))
        target.popup_menu = {"choices": choices, "scroll": 0}
        self.assertEqual(target.popup_visible_choices(), choices[:7])
        target.scroll_popup_choices(99)
        self.assertEqual(target.popup_visible_choices(), choices[-7:])
        target.scroll_popup_choices(-2)
        self.assertEqual(target.popup_visible_choices(), choices[3:10])


class ControllerReplyProtocolTests(unittest.TestCase):
    """Firmware reply records must settle the matching HUD transaction."""

    def reply_target(self, card_id: str):
        target = HUDApp.__new__(HUDApp)
        target._controller_transactions = {card_id: "timer"}
        target.controller_card_feedback = {}
        target._pending_controller_card = card_id
        target._pending_controller_values = {"startup_rom": 2, "boot_iec": 11}
        target._hud_dirty = False
        target.finish_controller_transaction = lambda current: target._controller_transactions.pop(current, None)
        target.update_live_hud_fields = lambda: None

        class Status:
            def set(self, _value: str) -> None:
                pass

        target.control_status = Status()

        class Var:
            def __init__(self, value: str) -> None:
                self.value = value
            def set(self, value: str) -> None:
                self.value = value
            def get(self) -> str:
                return self.value

        target.rom_choice = Var("Slot 2 — JIFFYDOS")
        target.rom_var = Var("Slot 2 — JIFFYDOS")
        target.iec_choice = Var("Device 11")
        target.iec_var = Var("Device 11")
        return target

    def test_iec_reply_record_confirms_boot_iec_transaction(self) -> None:
        target = self.reply_target("boot_iec")
        reply = "$ROMTEST,IEC,OK,ADDRESS=11,VERIFY=11,SAVED_SLOT=1,VALID=1"
        self.assertTrue(target.handle_controller_setting_reply(reply))
        self.assertEqual(target.controller_card_feedback["boot_iec"], "CONFIRMED BY CONTROL ONEROM")
        self.assertFalse(target._controller_transactions)

    def test_set_reply_record_confirms_startup_rom_transaction(self) -> None:
        target = self.reply_target("startup_rom")
        self.assertTrue(target.handle_controller_setting_reply("$ROMTEST,SET,OK,SLOT=2,VERIFY=2"))
        self.assertEqual(target.controller_card_feedback["startup_rom"], "CONFIRMED BY CONTROL ONEROM")
        self.assertFalse(target._controller_transactions)

    def test_drive_reset_finishes_with_an_honest_unverified_result(self) -> None:
        target = self.reply_target("drive_reset")
        scheduled = []
        target._drive_reset_after_id = None
        target.after = lambda delay, callback: (scheduled.append((delay, callback)), "reset-window")[1]
        target.after_cancel = lambda _after_id: None
        self.assertTrue(target.handle_rom_apply_reset_reply("$ROMTEST,RESET,PULSE_STARTED,GPIO=5"))
        self.assertEqual(target.controller_card_feedback["drive_reset"], "RESET PULSE SENT - WAITING")
        self.assertFalse(target._controller_transactions)
        self.assertEqual(scheduled[0][0], 7000)
        scheduled[0][1]()
        self.assertEqual(target.controller_card_feedback["drive_reset"], "RESET WINDOW COMPLETE")

    def test_reset_card_is_last_in_the_visibility_catalog(self) -> None:
        self.assertEqual(touchscreen.HUD_CARD_CATALOG[-1], ("drive_reset", "Control Reset Drive"))


class ControllerRomDiscoveryTests(unittest.TestCase):
    """1541HUD must build the ROM picker from selector metadata, not guesses."""

    def discovery_target(self):
        target = HUDApp.__new__(HUDApp)
        target.rom_choices = (
            "Slot 1 — ORIGINAL", "Slot 2 — JIFFYDOS", "Slot 3 — ORIGINAL",
        )
        target._legacy_rom_choices = target.rom_choices
        target._rom_choices_by_slot = {
            index: label for index, label in enumerate(target.rom_choices, start=1)
        }
        target._rom_discovery_pending = []
        target._rom_discovery_slots = {}
        target._controller_transactions = {}
        target.controller_card_feedback = {}
        target._hud_dirty = False
        sent, scheduled = [], []
        target.send_controller_command = sent.append
        target.after = lambda _delay, callback: (scheduled.append(callback), f"after-{len(scheduled)}")[1]
        target.after_cancel = lambda _after_id: None
        target.append_usb_log = lambda *_args: None
        target.update_live_hud_fields = lambda: None
        target.refresh_control_startup_settings = lambda: sent.append("ROMGET")

        class Status:
            def __init__(self): self.value = ""
            def set(self, value): self.value = value
            def get(self): return self.value

        target.control_status = Status()
        return target, sent, scheduled

    def test_romlist_and_romslot_metadata_define_picker(self) -> None:
        target, sent, scheduled = self.discovery_target()
        target.begin_controller_session()
        self.assertEqual(sent, ["ROMLIST"])
        self.assertTrue(target.handle_controller_rom_list_reply("$ROMTEST,LIST,COUNT=3,SELECTABLE_MAX=2"))
        self.assertEqual(sent[-1], "ROMSLOT=1")
        self.assertTrue(target.handle_controller_rom_slot_reply("$ROMTEST,SLOT,INDEX=1,SELECTABLE=1,NAME=ORIGINAL"))
        self.assertEqual(sent[-1], "ROMSLOT=2")
        self.assertTrue(target.handle_controller_rom_slot_reply("$ROMTEST,SLOT,INDEX=2,SELECTABLE=1,NAME=TEST ROM"))
        self.assertEqual(target.rom_choices, ("Slot 1 — ORIGINAL", "Slot 2 — TEST ROM"))
        self.assertEqual(target.controller_rom_choice_for_slot(2), "Slot 2 — TEST ROM")
        self.assertIsNone(target.controller_rom_choice_for_slot(3))
        scheduled[-1]()
        self.assertEqual(sent[-1], "ROMGET")

    def test_legacy_romlist_error_keeps_fallback_labels(self) -> None:
        target, sent, _scheduled = self.discovery_target()
        target.begin_controller_session()
        self.assertTrue(target.handle_controller_rom_discovery_error("$ROMTEST,ERROR,UNKNOWN_COMMAND"))
        self.assertEqual(target.rom_choices, target._legacy_rom_choices)
        self.assertIn("USING LEGACY SLOT LABELS", target.controller_card_feedback["startup_rom"])


class _FakeCdcDevice:
    """Minimal serial-device double for receive-buffer tests."""

    is_open = True

    def __init__(self, payload: bytes) -> None:
        self.payload = payload

    def read(self, _size: int) -> bytes:
        payload, self.payload = self.payload, b""
        return payload


class DriveTelemetryParserHeadStateTests(unittest.TestCase):
    def test_track_overrange_starts_above_track_thirty_five(self) -> None:
        self.assertFalse(track_is_over_dos_range(None))
        self.assertFalse(track_is_over_dos_range(70))
        self.assertTrue(track_is_over_dos_range(71))

    def test_successive_track_writes_set_requested_direction(self) -> None:
        parser = DriveTelemetryParser()
        with mock_monotonic(10.0):
            parser.process("MOTOR state=1")
            parser.process("TRACK_WRITE addr=$0022 data=$12 (18)")
            parser.process("TRACK_WRITE addr=$0022 data=$14 (20)")
            self.assertEqual(parser.state.head, "IN")
            parser.process("TRACK_WRITE addr=$0022 data=$0F (15)")
        self.assertEqual(parser.state.head, "OUT")

    def test_status_target_changes_set_requested_direction(self) -> None:
        parser = DriveTelemetryParser()
        with mock_monotonic(10.0):
            parser.process("STATE T0.0.16 TV=1 T=18 M=1")
            parser.process("STATE T0.0.16 TV=1 T=20 M=1")
            self.assertEqual(parser.state.head, "IN")
            parser.process("STATE T0.0.16 TV=1 T=15 M=1")
        self.assertEqual(parser.state.head, "OUT")

    def test_repeated_status_target_does_not_hide_direction(self) -> None:
        parser = DriveTelemetryParser()
        with mock_monotonic(10.0):
            parser.process("STATE T0.0.16 TV=1 T=18 M=1")
            parser.process("STATE T0.0.16 TV=1 T=20 M=1")
        with mock_monotonic(10.1):
            parser.process("STATE T0.0.16 TV=1 T=20 M=1")
        self.assertEqual(parser.state.head, "IN")
        with mock_monotonic(10.8):
            self.assertEqual(parser.refresh().head, "STALL")

    def test_skipped_phase_stays_active_in_the_last_confirmed_direction(self) -> None:
        parser = DriveTelemetryParser()
        parser.state.motor = True
        parser.state.head = "OUT"
        parser._last_direction = "OUT"
        parser._last_motion = 1.0
        with mock_monotonic(10.0):
            parser.process("PHASE old=0 new=2 delta=2 motor=1")
        self.assertEqual(parser.state.head, "OUT")
        with mock_monotonic(10.79):
            self.assertEqual(parser.refresh().head, "OUT")
        with mock_monotonic(10.8):
            self.assertEqual(parser.refresh().head, "STALL")

    def test_head_stalls_after_eight_tenths_of_a_second_without_events(self) -> None:
        parser = DriveTelemetryParser()
        parser.state.motor = True
        parser.state.head = "IN"
        parser._last_motion = 10.0
        with mock_monotonic(10.79):
            self.assertEqual(parser.refresh().head, "IN")
        with mock_monotonic(10.8):
            self.assertEqual(parser.refresh().head, "STALL")

    def test_first_target_write_resets_the_stall_interval_without_direction(self) -> None:
        parser = DriveTelemetryParser()
        parser.state.motor = True
        parser._last_motion = 1.0
        with mock_monotonic(20.0):
            parser.process("TRACK_WRITE addr=$0022 data=$12 (18)")
        self.assertEqual(parser._last_motion, 20.0)
        with mock_monotonic(20.79):
            self.assertNotEqual(parser.refresh().head, "PARK")

    def test_motor_off_parks_the_head(self) -> None:
        parser = DriveTelemetryParser()
        parser.state.motor = True
        parser.state.head = "OUT"
        parser.process("MOTOR state=0")
        self.assertEqual(parser.state.head, "PARK")

    def test_phase_traffic_alone_does_not_invent_absolute_home(self) -> None:
        parser = DriveTelemetryParser()
        parser.process("MOTOR state=1")
        for _ in range(100):
            parser.process("PHASE old=1 new=0 delta=3 motor=1")
        parser.process("PHASE old=0 new=1 delta=1 motor=1")
        self.assertIsNone(parser.state.position_half_tracks)
        self.assertEqual(parser.state.position_source, "UNANCHORED")

    def test_0022_one_reanchors_then_phase_counting_resumes(self) -> None:
        parser = DriveTelemetryParser()
        parser.process("MOTOR state=1")
        parser.process("TRACK_WRITE addr=$0022 data=$01 (1)")
        self.assertEqual(parser.state.track, "01.0")
        parser.process("PHASE old=0 new=1 delta=1 motor=1")
        self.assertEqual(parser.state.track, "01.5")
        parser.process("PHASE old=1 new=0 delta=3 motor=1")
        self.assertEqual(parser.state.track, "01.0")

    def test_non_home_track_write_does_not_overwrite_established_phase_position(self) -> None:
        parser = DriveTelemetryParser()
        parser.process("MOTOR state=1")
        parser.process("TRACK_WRITE addr=$0022 data=$01 (1)")
        parser.process("PHASE old=0 new=1 delta=1 motor=1")
        parser.process("TRACK_WRITE addr=$0022 data=$12 (18)")
        self.assertEqual(parser.state.track, "01.5")

    def test_complete_burst_records_survive_buffer_guard(self) -> None:
        link = CdcBoardLink(BoardDescriptor("TEST", "COM1", "Test device"))
        payload = (b"SYNC T0.0.16 COUNT=1 LEVEL=0\n" * 1200)
        link.device = _FakeCdcDevice(payload)
        lines = link.read_lines()
        self.assertEqual(len(lines), 1200)
        self.assertTrue(all(line.startswith("SYNC ") for line in lines))


class TelemetrySubscriptionTests(unittest.TestCase):
    def test_background_subscription_keeps_all_evidence_classes_enabled(self) -> None:
        target = HUDApp.__new__(HUDApp)
        self.assertEqual(target.visible_hud_subscription_mask(), touchscreen.HUD_TELEMETRY_ALL_MASK)


class CdcBoardLinkTests(unittest.TestCase):
    def test_unterminated_receive_data_cannot_grow_the_buffer_forever(self) -> None:
        link = CdcBoardLink(BoardDescriptor("TEST", "COM1", "Test device"))
        link.device = _FakeCdcDevice(b"xx")
        link._buffer = "x" * (MAX_RX_BUFFER_BYTES - 1)

        self.assertEqual(link.read_lines(), [])
        self.assertEqual(link._buffer, "")

    def test_close_isolates_late_bytes_from_a_previous_windows_reader(self) -> None:
        link = CdcBoardLink(BoardDescriptor("TEST", "COM1", "Test device"))
        previous_queue = link._windows_received
        previous_queue.put(b"STALE\n")

        link.close()
        # A worker that finishes after close still writes to its captured old
        # queue; it must not affect the next connection's receive queue.
        previous_queue.put(b"LATE\n")

        self.assertEqual(link._windows_take_received(), b"")


class TrackSyncTests(unittest.TestCase):
    def test_manual_sync_sets_directory_track_without_a_drive_command(self) -> None:
        target = HUDApp.__new__(HUDApp)
        target.telemetry_parser = DriveTelemetryParser()
        target.live_track = "--.-"
        target._hud_dirty = False
        events = []

        class TrackVariable:
            value = ""

            def set(self, value: str) -> None:
                self.value = value

        target.track_var = TrackVariable()
        target.clear_current_header = lambda: events.append("clear-header")
        target.clear_recent_sectors = lambda: events.append("clear-sectors")
        target.record_activity = lambda event: events.append(event)
        target.append_usb_log = lambda role, message: events.append((role, message))
        target.update_live_hud_fields = lambda: events.append("redraw")

        target.sync_track_to_18()

        self.assertEqual(target.telemetry_parser.state.track, "18.0")
        self.assertEqual(target.telemetry_parser.state.position_source, "MANUAL SYNC 18")
        self.assertEqual(target.track_var.value, "18.0")
        self.assertIn("MANUAL SYNC T18", events)
        self.assertTrue(target._hud_dirty)


class MechanismStepsTests(unittest.TestCase):
    def test_clear_steps_resets_only_the_local_display_counter(self) -> None:
        target = HUDApp.__new__(HUDApp)
        target.phase_event_count = 37
        target._hud_dirty = False
        events = []
        target.append_usb_log = lambda role, message: events.append((role, message))
        target.update_live_hud_fields = lambda: events.append("redraw")

        target.clear_mechanism_steps()

        self.assertEqual(target.phase_event_count, 0)
        self.assertTrue(target._hud_dirty)
        self.assertIn("redraw", events)
        self.assertTrue(any("local display only" in message for _role, message in events))


class SyncQualificationTests(unittest.TestCase):
    def qualifier_target(self) -> HUDApp:
        target = HUDApp.__new__(HUDApp)
        target.motor = True
        target.firmware_rpm = None
        target.live_density = 2
        target.qualified_sync_count = None
        target._sync_candidate_counts = []
        target.last_stable_rpm = None
        target.last_sync_per_rev = None
        target.rpm_samples = []
        return target

    def test_partial_spinup_sample_stays_acquiring(self) -> None:
        target = self.qualifier_target()
        self.assertIsNone(target.observe_sync_sample(117))
        self.assertIsNone(target.qualified_sync_count)
        self.assertEqual(target.last_stable_rpm, 300.0)

    def test_two_consistent_samples_produce_qualified_rpm(self) -> None:
        target = self.qualifier_target()
        self.assertIsNone(target.observe_sync_sample(190))
        self.assertAlmostEqual(target.observe_sync_sample(190), 300.0)
        self.assertEqual(target.qualified_sync_count, 190)

    def test_reset_does_not_restore_the_retired_desktop_rpm_estimate(self) -> None:
        target = self.qualifier_target()
        target.observe_sync_sample(190)
        target.observe_sync_sample(190)
        target.reset_sync_qualification()
        self.assertIsNone(target.qualified_sync_count)
        self.assertIsNone(target.effective_rpm())

    def test_motor_running_without_firmware_rpm_stays_unknown(self) -> None:
        target = self.qualifier_target()
        self.assertIsNone(target.effective_rpm())

    def test_bad_sync_window_holds_last_qualified_rpm(self) -> None:
        target = self.qualifier_target()
        target.observe_sync_sample(190)
        target.observe_sync_sample(190)
        self.assertAlmostEqual(target.last_stable_rpm, 300.0)
        self.assertIsNone(target.observe_sync_sample(20))
        self.assertIsNone(target.effective_rpm())


class DriveBindingPersistenceTests(unittest.TestCase):
    def test_serial_bindings_and_appearance_share_one_json_file(self) -> None:
        path = Path("onerom_drive_bindings.json")
        binding = DriveBinding(
            controller_serial="CONTROL-123",
            hud_serial="MONITOR-456",
            appearance={"background": "#112233", "accent": "#AABBCC"},
            priorities={"track": 1, "capture_health": None, "sync_rate": 12},
            control_features={"rom_select": True, "iec_address": False, "write_protect_override": False},
            window_size={"width": 1280, "height": 720},
        )
        import tempfile
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "onerom_drive_bindings.json"
            save_binding(path, binding)
            self.assertTrue(path.exists())
            loaded = load_binding(path)

        self.assertEqual(loaded.controller_serial, "CONTROL-123")
        self.assertEqual(loaded.hud_serial, "MONITOR-456")
        self.assertEqual(loaded.appearance, {"background": "#112233", "accent": "#AABBCC"})
        self.assertEqual(loaded.priorities, {"track": 1, "capture_health": None, "sync_rate": 12})
        self.assertEqual(loaded.control_features, {"rom_select": True, "iec_address": False, "write_protect_override": False})
        self.assertEqual(loaded.window_size, {"width": 1280, "height": 720})

    def test_known_onerom_vid_pid_are_the_only_discoverable_boards(self) -> None:
        class Port:
            def __init__(self, serial_number, device, vid, pid, hwid="", location="", interface=None):
                self.serial_number = serial_number
                self.device = device
                self.description = "Test"
                self.vid = vid
                self.pid = pid
                self.hwid = hwid
                self.location = location
                self.interface = interface
        class Ports:
            def __init__(self, values):
                self.values = values
            def comports(self):
                return self.values
        good = Port("GOOD", "COM1", touchscreen.ONEROM_USB_VID, touchscreen.ONEROM_USB_PID)
        bad = Port("BAD", "COM2", 0x1234, 0x5678)
        with patch.object(touchscreen, "list_ports", Ports([bad, good])):
            boards = touchscreen.discover_cdc_boards()
        self.assertEqual([board.serial_number for board in boards], ["GOOD"])

    def test_two_cdc_ports_with_one_serial_select_the_primary_interface(self) -> None:
        ports = [
            BoardDescriptor("ONE", "COM12", "OneROM Serial 2", interface_number=4),
            BoardDescriptor("ONE", "COM11", "OneROM Serial", interface_number=2),
        ]
        selected = select_command_cdc_boards(ports)
        self.assertEqual([(board.serial_number, board.port, board.interface_number) for board in selected],
                         [("ONE", "COM11", 2)])

    def test_ambiguous_duplicate_ports_are_refused_not_guessed(self) -> None:
        ports = [
            BoardDescriptor("ONE", "COM11", "OneROM Serial"),
            BoardDescriptor("ONE", "COM12", "OneROM Serial 2"),
        ]
        self.assertEqual(select_command_cdc_boards(ports), [])

    def test_windows_mi_marker_identifies_primary_cdc_port(self) -> None:
        class Port:
            def __init__(self, device, hwid):
                self.serial_number = "ONE"
                self.device = device
                self.description = "OneROM USB"
                self.vid = touchscreen.ONEROM_USB_VID
                self.pid = touchscreen.ONEROM_USB_PID
                self.hwid = hwid
                self.location = ""
                self.interface = None
        class Ports:
            def comports(self):
                return [
                    Port("COM12", "USB\\VID_1209&PID_F542&MI_04\\7&SECOND"),
                    Port("COM11", "USB\\VID_1209&PID_F542&MI_02\\7&FIRST"),
                ]
        with patch.object(touchscreen, "list_ports", Ports()):
            boards = touchscreen.discover_cdc_boards()
        self.assertEqual([(board.port, board.interface_number) for board in boards], [("COM11", 2)])

    def test_report_explains_ignored_secondary_interface(self) -> None:
        boards = [BoardDescriptor("ONE", "COM11", "OneROM", interface_number=2),
                  BoardDescriptor("ONE", "COM12", "OneROM Serial 2", interface_number=4)]
        with patch.object(touchscreen, "list_ports", None):
            self.assertIn("PySerial", discover_cdc_board_report()[1][0])


class ControllerTransactionTests(unittest.TestCase):
    def test_wp_query_is_tracked_as_a_transaction(self) -> None:
        target = HUDApp.__new__(HUDApp)
        target._controller_transactions = {"write_protect_query": "timer"}
        target._pending_wp_override = None
        target.controller_wp_available = None
        target._confirmed_writable = False
        class Var:
            def __init__(self): self.value = False
            def set(self, value): self.value = value
        class Status:
            def set(self, _value): pass
        target.writable = Var()
        target.control_status = Status()
        target.finish_controller_transaction = lambda current: target._controller_transactions.pop(current, None)
        target.update_live_hud_fields = lambda: None
        target.handle_controller_wp_reply("$ROMTEST,WP,STATE=OFF,AVAILABLE=1")
        self.assertFalse(target._controller_transactions)
        self.assertFalse(target.writable.value)
        self.assertTrue(target.controller_wp_available)


    def test_unknown_config_fields_do_not_discard_known_settings(self) -> None:
        path = Path("onerom_drive_bindings.json")
        payload = (
            '{"version": 99, "controller_serial": "CONTROL-123", '
            '"hud_serial": "MONITOR-456", "future_setting": true}'
        )
        with patch.object(Path, "read_text", return_value=payload):
            loaded = load_binding(path)

        self.assertEqual(loaded.version, 99)
        self.assertEqual(loaded.controller_serial, "CONTROL-123")
        self.assertEqual(loaded.hud_serial, "MONITOR-456")


if __name__ == "__main__":
    unittest.main()




