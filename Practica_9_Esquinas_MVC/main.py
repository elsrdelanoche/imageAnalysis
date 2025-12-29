from __future__ import annotations

import tkinter as tk
from tkinter import filedialog, messagebox

from app.view.app_view import AppView, ViewCallbacks
from app.controller.app_controller import AppController


def main() -> None:
    root = tk.Tk()

    # MVC: el View necesita callbacks, pero el Controller se crea después.
    controller_holder = {"c": None}
    view_holder = {"v": None}

    def _controller() -> AppController:
        c = controller_holder["c"]
        if c is None:
            raise RuntimeError("Controller aún no inicializado.")
        return c

    def _view() -> AppView:
        v = view_holder["v"]
        if v is None:
            raise RuntimeError("View aún no inicializada.")
        return v

    def on_open_a() -> None:
        path = filedialog.askopenfilename(
            title="Open Image A",
            filetypes=[
                ("Images", "*.png *.jpg *.jpeg *.bmp *.tif *.tiff"),
                ("All files", "*.*"),
            ],
        )
        if path:
            try:
                _controller().open_image("A", path)
            except Exception as e:
                messagebox.showerror("Error", f"No se pudo abrir imagen A:\n{e}")

    def on_open_b() -> None:
        path = filedialog.askopenfilename(
            title="Open Image B",
            filetypes=[
                ("Images", "*.png *.jpg *.jpeg *.bmp *.tif *.tiff"),
                ("All files", "*.*"),
            ],
        )
        if path:
            try:
                _controller().open_image("B", path)
            except Exception as e:
                messagebox.showerror("Error", f"No se pudo abrir imagen B:\n{e}")

    def on_apply() -> None:
        try:
            _controller().apply()
        except Exception as e:
            messagebox.showerror("Error", f"Falló Apply:\n{e}")

    def on_apply_both() -> None:
        """
        Procesa A y B con el método/params actuales.
        - Si el controller tiene apply_both(), lo usa.
        - Si no, hace fallback cambiando la imagen activa en la vista.
        """
        try:
            c = _controller()

            # Si tu controller ya implementa apply_both(), úsalo.
            if hasattr(c, "apply_both") and callable(getattr(c, "apply_both")):
                c.apply_both()
                return

            # Fallback genérico: aplicar en A y luego en B
            v = _view()

            # La vista normalmente tiene active_img (tk.StringVar).
            # Si tu implementación difiere, aquí sería el único ajuste.
            old = v.active_img.get() if hasattr(v, "active_img") else v.get_active_image_key()

            # Aplica en A
            if hasattr(v, "active_img"):
                v.active_img.set("A")
            c.apply()

            # Aplica en B
            if hasattr(v, "active_img"):
                v.active_img.set("B")
            c.apply()

            # Restaura selección previa
            if hasattr(v, "active_img"):
                v.active_img.set(old)

            # Opcional: status
            if hasattr(v, "set_status"):
                v.set_status("Listo: aplicado en A y B.")

        except Exception as e:
            messagebox.showerror("Error", f"Falló Apply to A + B:\n{e}")

    def on_save_output() -> None:
        path = filedialog.asksaveasfilename(
            title="Save output image",
            defaultextension=".png",
            filetypes=[
                ("PNG", "*.png"),
                ("JPEG", "*.jpg *.jpeg"),
                ("BMP", "*.bmp"),
                ("All files", "*.*"),
            ],
        )
        if path:
            try:
                _controller().save_output(path)
            except Exception as e:
                messagebox.showerror("Error", f"No se pudo guardar la salida:\n{e}")

    # Construir vista con TODOS los callbacks requeridos
    view = AppView(
        root,
        ViewCallbacks(
            on_open_a=on_open_a,
            on_open_b=on_open_b,
            on_apply=on_apply,
            on_apply_both=on_apply_both,   # <-- FIX DEL ERROR
            on_save_output=on_save_output,
        ),
    )
    view_holder["v"] = view

    controller = AppController(view)
    controller_holder["c"] = controller

    root.mainloop()


if __name__ == "__main__":
    main()
