from __future__ import annotations

import tkinter as tk
from tkinter import ttk
from tkinter import filedialog, messagebox

from dataclasses import dataclass
from typing import Callable, Optional, Tuple, Dict

import numpy as np
from PIL import Image, ImageTk

from app.utils.image_io import resize_to_fit


@dataclass
class ViewCallbacks:
    on_open_a: Callable[[], None]
    on_open_b: Callable[[], None]
    on_apply: Callable[[], None]
    on_apply_both: Callable[[], None]
    on_save_output: Callable[[], None]


class AppView:
    def __init__(self, root: tk.Tk, callbacks: ViewCallbacks):
        self.root = root
        self.callbacks = callbacks

        self.root.title("Práctica 09 — Esquinas (Kirsch / Frei‑Chen / Harris) — MVC")
        self.root.geometry("1200x720")
        self.root.minsize(980, 600)

        self._build_menu()
        self._build_layout()

        self._imgtk_refs: Dict[str, ImageTk.PhotoImage] = {}

    # ---------------- UI BUILD ----------------
    def _build_menu(self):
        menubar = tk.Menu(self.root)

        filem = tk.Menu(menubar, tearoff=0)
        filem.add_command(label="Open Image A…", command=self.callbacks.on_open_a)
        filem.add_command(label="Open Image B…", command=self.callbacks.on_open_b)
        filem.add_separator()
        filem.add_command(label="Save current output…", command=self.callbacks.on_save_output)
        filem.add_separator()
        filem.add_command(label="Exit", command=self.root.quit)
        menubar.add_cascade(label="File", menu=filem)

        helpm = tk.Menu(menubar, tearoff=0)
        helpm.add_command(label="About", command=self._about)
        menubar.add_cascade(label="Help", menu=helpm)

        self.root.config(menu=menubar)

    def _build_layout(self):
        self.main = ttk.Frame(self.root, padding=8)
        self.main.pack(fill=tk.BOTH, expand=True)

        self.main.columnconfigure(0, weight=0)
        self.main.columnconfigure(1, weight=1)
        self.main.rowconfigure(0, weight=1)

        # Left control panel
        self.left = ttk.Frame(self.main, padding=(8, 8))
        self.left.grid(row=0, column=0, sticky="nsw")
        self.left.columnconfigure(0, weight=1)

        # Right notebook (images)
        self.right = ttk.Frame(self.main, padding=(8, 8))
        self.right.grid(row=0, column=1, sticky="nsew")
        self.right.columnconfigure(0, weight=1)
        self.right.rowconfigure(0, weight=1)

        self._build_controls(self.left)
        self._build_notebook(self.right)

        # Status bar
        self.status = tk.StringVar(value="Listo. Carga una imagen para empezar.")
        self.statusbar = ttk.Label(self.root, textvariable=self.status, anchor="w")
        self.statusbar.pack(fill=tk.X, side=tk.BOTTOM)

    def _build_controls(self, parent: ttk.Frame):
        # Active image
        ttk.Label(parent, text="Active Image").grid(row=0, column=0, sticky="w")
        self.active_img = tk.StringVar(value="A")
        row = ttk.Frame(parent)
        row.grid(row=1, column=0, sticky="ew", pady=(2, 10))
        ttk.Radiobutton(row, text="A", value="A", variable=self.active_img).pack(side=tk.LEFT)
        ttk.Radiobutton(row, text="B", value="B", variable=self.active_img).pack(side=tk.LEFT)

        # Method
        ttk.Label(parent, text="Método").grid(row=2, column=0, sticky="w")
        self.method = tk.StringVar(value="Harris")
        self.method_cb = ttk.Combobox(parent, textvariable=self.method, state="readonly",
                                      values=["Kirsch", "Frei-Chen", "Harris"])
        self.method_cb.grid(row=3, column=0, sticky="ew", pady=(2, 10))
        self.method_cb.bind("<<ComboboxSelected>>", lambda e: self._sync_param_panels())

        # Parameter panels
        self.params_container = ttk.Frame(parent)
        self.params_container.grid(row=4, column=0, sticky="ew")
        self.params_container.columnconfigure(0, weight=1)

        self.kirsch_panel = self._kirsch_panel(self.params_container)
        self.frei_panel = self._frei_panel(self.params_container)
        self.harris_panel = self._harris_panel(self.params_container)

        self._sync_param_panels()

        # Apply
        ttk.Button(parent, text="Apply", command=self.callbacks.on_apply).grid(
            row=5, column=0, sticky="ew", pady=(14, 6)
        )
        ttk.Button(parent, text="Apply to A + B", command=self.callbacks.on_apply_both).grid(
            row=6, column=0, sticky="ew", pady=(0, 6)
        )

    def _kirsch_panel(self, parent: ttk.Frame) -> ttk.Labelframe:
        box = ttk.Labelframe(parent, text="Kirsch params", padding=8)
        box.grid(row=0, column=0, sticky="ew")

        self.kirsch_mode = tk.StringVar(value="max_abs")
        ttk.Label(box, text="Combine").grid(row=0, column=0, sticky="w")
        ttk.Combobox(box, textvariable=self.kirsch_mode, state="readonly",
                     values=["max_abs", "max"]).grid(row=1, column=0, sticky="ew", pady=(2, 8))

        self.kirsch_show_dirs = tk.BooleanVar(value=False)
        ttk.Checkbutton(box, text="Also export per-direction maps", variable=self.kirsch_show_dirs).grid(
            row=2, column=0, sticky="w"
        )
        return box

    def _frei_panel(self, parent: ttk.Frame) -> ttk.Labelframe:
        box = ttk.Labelframe(parent, text="Frei-Chen params", padding=8)
        box.grid(row=0, column=0, sticky="ew")

        ttk.Label(box, text="Salida").grid(row=0, column=0, sticky="w")
        self.frei_output = tk.StringVar(value="edge_strength")
        ttk.Combobox(box, textvariable=self.frei_output, state="readonly",
                     values=["edge_strength"]).grid(row=1, column=0, sticky="ew", pady=(2, 8))
        return box

    def _harris_panel(self, parent: ttk.Frame) -> ttk.Labelframe:
        box = ttk.Labelframe(parent, text="Harris params", padding=8)
        box.grid(row=0, column=0, sticky="ew")

        # k
        ttk.Label(box, text="k (0.04–0.06 típico)").grid(row=0, column=0, sticky="w")
        self.h_k = tk.DoubleVar(value=0.04)
        ttk.Entry(box, textvariable=self.h_k).grid(row=1, column=0, sticky="ew", pady=(2, 8))

        # sigma
        ttk.Label(box, text="sigma (suavizado)").grid(row=2, column=0, sticky="w")
        self.h_sigma = tk.DoubleVar(value=1.0)
        ttk.Entry(box, textvariable=self.h_sigma).grid(row=3, column=0, sticky="ew", pady=(2, 8))

        # thresh
        ttk.Label(box, text="threshold relativo (0–1)").grid(row=4, column=0, sticky="w")
        self.h_thresh = tk.DoubleVar(value=0.01)
        ttk.Entry(box, textvariable=self.h_thresh).grid(row=5, column=0, sticky="ew", pady=(2, 8))

        # nms
        ttk.Label(box, text="NMS window (impar)").grid(row=6, column=0, sticky="w")
        self.h_nms = tk.IntVar(value=5)
        ttk.Entry(box, textvariable=self.h_nms).grid(row=7, column=0, sticky="ew", pady=(2, 8))

        # max points
        ttk.Label(box, text="max corners").grid(row=8, column=0, sticky="w")
        self.h_maxp = tk.IntVar(value=2000)
        ttk.Entry(box, textvariable=self.h_maxp).grid(row=9, column=0, sticky="ew", pady=(2, 0))

        return box

    def _build_notebook(self, parent: ttk.Frame):
        self.nb = ttk.Notebook(parent)
        self.nb.grid(row=0, column=0, sticky="nsew")

        self.tab_orig = ttk.Frame(self.nb)
        self.tab_resp = ttk.Frame(self.nb)
        self.tab_over = ttk.Frame(self.nb)

        self.nb.add(self.tab_orig, text="Original")
        self.nb.add(self.tab_resp, text="Response")
        self.nb.add(self.tab_over, text="Overlay")

        self._build_img_tab(self.tab_orig, key="orig")
        self._build_img_tab(self.tab_resp, key="resp")
        self._build_img_tab(self.tab_over, key="over")

    def _build_img_tab(self, tab: ttk.Frame, key: str):
        tab.rowconfigure(0, weight=1)
        tab.columnconfigure(0, weight=1)
        canvas = tk.Canvas(tab, bg="#111111", highlightthickness=0)
        canvas.grid(row=0, column=0, sticky="nsew")
        setattr(self, f"canvas_{key}", canvas)

    def _sync_param_panels(self):
        # show only relevant panel
        for panel in (self.kirsch_panel, self.frei_panel, self.harris_panel):
            panel.grid_remove()

        m = self.method.get()
        if m == "Kirsch":
            self.kirsch_panel.grid()
        elif m == "Frei-Chen":
            self.frei_panel.grid()
        else:
            self.harris_panel.grid()

    def _about(self):
        messagebox.showinfo(
            "About",
            "Práctica 09 — Extracción de esquinas\n"
            "Kirsch, Frei‑Chen y Harris–Stephens\n"
            "Arquitectura MVC, Python + Tkinter + NumPy + Pillow."
        )

    # ---------------- Rendering ----------------
    def _render_on_canvas(self, canvas: tk.Canvas, pil_img: Image.Image):
        canvas.update_idletasks()
        w = max(1, canvas.winfo_width())
        h = max(1, canvas.winfo_height())
        img_fit = resize_to_fit(pil_img, (w, h))

        imgtk = ImageTk.PhotoImage(img_fit)
        canvas.delete("all")
        canvas.create_image(w // 2, h // 2, image=imgtk, anchor="center")
        # keep reference
        self._imgtk_refs[str(canvas)] = imgtk

    def set_images(self, pil_original: Optional[Image.Image],
                   pil_response: Optional[Image.Image],
                   pil_overlay: Optional[Image.Image]) -> None:
        if pil_original is not None:
            self._render_on_canvas(self.canvas_orig, pil_original)
        if pil_response is not None:
            self._render_on_canvas(self.canvas_resp, pil_response)
        if pil_overlay is not None:
            self._render_on_canvas(self.canvas_over, pil_overlay)

    def set_status(self, text: str) -> None:
        self.status.set(text)

    # ---------------- Parameter accessors ----------------
    def get_active_image_key(self) -> str:
        return self.active_img.get()

    def get_method(self) -> str:
        return self.method.get()

    def get_kirsch_params(self) -> dict:
        return {
            "mode": self.kirsch_mode.get(),
            "show_dirs": bool(self.kirsch_show_dirs.get())
        }

    def get_frei_params(self) -> dict:
        return {
            "output": self.frei_output.get(),
        }

    def get_harris_params(self) -> dict:
        return {
            "k": float(self.h_k.get()),
            "sigma": float(self.h_sigma.get()),
            "thresh_rel": float(self.h_thresh.get()),
            "nms": int(self.h_nms.get()),
            "max_points": int(self.h_maxp.get()),
        }
