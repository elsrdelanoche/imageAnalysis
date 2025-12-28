from __future__ import annotations

import tkinter as tk
from tkinter import filedialog

from app.view.app_view import AppView, ViewCallbacks
from app.controller.app_controller import AppController


def main():
    root = tk.Tk()

    # Controller will be created after view, but callbacks need it.
    controller_holder = {"c": None}

    def on_open_a():
        path = filedialog.askopenfilename(
            title="Open Image A",
            filetypes=[("Images", "*.png *.jpg *.jpeg *.bmp *.tif *.tiff"), ("All files", "*.*")]
        )
        if path:
            controller_holder["c"].open_image("A", path)

    def on_open_b():
        path = filedialog.askopenfilename(
            title="Open Image B",
            filetypes=[("Images", "*.png *.jpg *.jpeg *.bmp *.tif *.tiff"), ("All files", "*.*")]
        )
        if path:
            controller_holder["c"].open_image("B", path)

    def on_apply():
        controller_holder["c"].apply()

    def on_save_output():
        path = filedialog.asksaveasfilename(
            title="Save output image",
            defaultextension=".png",
            filetypes=[("PNG", "*.png"), ("JPEG", "*.jpg *.jpeg"), ("BMP", "*.bmp"), ("All files", "*.*")]
        )
        if path:
            controller_holder["c"].save_output(path)

    view = AppView(root, ViewCallbacks(
        on_open_a=on_open_a,
        on_open_b=on_open_b,
        on_apply=on_apply,
        on_save_output=on_save_output,
    ))
    controller = AppController(view)
    controller_holder["c"] = controller

    root.mainloop()


if __name__ == "__main__":
    main()
