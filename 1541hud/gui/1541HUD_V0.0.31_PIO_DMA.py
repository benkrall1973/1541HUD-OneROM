import tkinter as tk

from hud_core_v030 import DriveHUD as _ProvenDriveHUDCore


class HUD1541(_ProvenDriveHUDCore):
    """1541HUD V0.0.31 candidate using the hardware-proven V0.0.30 GUI core."""

    def __init__(self, root):
        super().__init__(root)
        self.root.title("1541HUD V0.0.31 candidate")


if __name__ == "__main__":
    root = tk.Tk()
    app = HUD1541(root)
    root.mainloop()
