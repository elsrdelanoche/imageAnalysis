from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Optional, Dict

import numpy as np
from PIL import Image

from app.model.image_ops import (
    kirsch_response,
    frei_chen_edge_strength,
    harris_response,
    nms_peaks,
)
from app.utils.image_io import load_image, to_uint8_img, overlay_points


@dataclass
class ImageState:
    path: Optional[Path] = None
    rgb: Optional[np.ndarray] = None   # uint8 HxWx3
    gray: Optional[np.ndarray] = None  # float32 HxW


class AppController:
    def __init__(self, view):
        self.view = view
        self.images: Dict[str, ImageState] = {"A": ImageState(), "B": ImageState()}
        self.last_output: Optional[Image.Image] = None
        self.last_output_path_hint: Optional[Path] = None

    # --------- File actions ----------
    def open_image(self, key: str, path: str | Path) -> None:
        li = load_image(path)
        st = self.images[key]
        st.path = li.path
        st.rgb = li.rgb
        st.gray = li.gray
        self.last_output_path_hint = li.path.parent
        self.view.set_status(f"Cargada imagen {key}: {li.path.name}")
        self._render_original(key)

    def _render_original(self, key: str) -> None:
        st = self.images[key]
        if st.rgb is None:
            return
        pil_orig = Image.fromarray(st.rgb, mode="RGB")
        self.view.set_images(pil_orig, None, pil_orig)

    def _ensure_outdir(self, key: str, tag: str) -> Path:
        base = self.last_output_path_hint or Path.cwd()
        outdir = base / "outputs" / f"{key}_{tag}"
        outdir.mkdir(parents=True, exist_ok=True)
        return outdir

    # --------- Public actions ----------
    def apply(self) -> None:
        key = self.view.get_active_image_key()
        self._apply_to_key(key, update_view=True)

    def apply_both(self) -> None:
        # procesa lo que esté cargado en ambos lados; actualiza la vista con el activo
        active = self.view.get_active_image_key()
        processed_any = False
        for key in ("A", "B"):
            st = self.images[key]
            if st.gray is not None and st.rgb is not None:
                self._apply_to_key(key, update_view=(key == active))
                processed_any = True
        if not processed_any:
            self.view.set_status("No hay imágenes cargadas para procesar.")

    # --------- Core processing ----------
    def _apply_to_key(self, key: str, update_view: bool) -> None:
        method = self.view.get_method()
        st = self.images[key]
        if st.gray is None or st.rgb is None:
            if update_view:
                self.view.set_status(f"No hay imagen {key}.")
            return

        gray = st.gray
        rgb = st.rgb

        pil_orig = Image.fromarray(rgb, mode="RGB")

        if method == "Kirsch":
            params = self.view.get_kirsch_params()
            combined, per_dir = kirsch_response(gray, mode=params["mode"])
            resp_u8 = to_uint8_img(combined)
            pil_resp = Image.fromarray(resp_u8, mode="L").convert("RGB")

            # Export opcional por dirección
            if params["show_dirs"]:
                outdir = self._ensure_outdir(key, "kirsch")
                Image.fromarray(resp_u8, mode="L").save(outdir / "kirsch_combined.png")
                for name, mat in per_dir.items():
                    Image.fromarray(to_uint8_img(np.abs(mat)), mode="L").save(outdir / f"kirsch_{name}.png")
                status = f"Kirsch {key} listo. Exportado en: {outdir}"
            else:
                status = f"Kirsch {key} listo ({params['mode']})."

            if update_view:
                self.last_output = pil_resp
                self.view.set_images(pil_orig, pil_resp, pil_resp)
                self.view.set_status(status)

        elif method == "Frei-Chen":
            r, G = frei_chen_edge_strength(gray)
            resp_u8 = to_uint8_img(r)
            pil_resp = Image.fromarray(resp_u8, mode="L").convert("RGB")

            outdir = self._ensure_outdir(key, "frei_chen")
            Image.fromarray(resp_u8, mode="L").save(outdir / "frei_chen_edge_strength.png")
            for name, mat in G.items():
                Image.fromarray(to_uint8_img(np.abs(mat)), mode="L").save(outdir / f"frei_{name}.png")

            status = f"Frei‑Chen {key} listo. Exportado en: {outdir}"

            if update_view:
                self.last_output = pil_resp
                self.view.set_images(pil_orig, pil_resp, pil_resp)
                self.view.set_status(status)

        else:  # Harris
            params = self.view.get_harris_params()
            R = harris_response(gray, k=params["k"], sigma=params["sigma"])
            _mask, pts = nms_peaks(
                R,
                thresh_rel=params["thresh_rel"],
                nms_size=params["nms"],
                max_points=params["max_points"],
            )

            resp_u8 = to_uint8_img(R)
            pil_resp = Image.fromarray(resp_u8, mode="L").convert("RGB")

            over = overlay_points(rgb, pts, radius=2)
            pil_over = Image.fromarray(over, mode="RGB")

            outdir = self._ensure_outdir(key, "harris")
            Image.fromarray(resp_u8, mode="L").save(outdir / "harris_R.png")
            pil_over.save(outdir / "harris_overlay.png")
            csv_path = outdir / "harris_points_xy.csv"
            with csv_path.open("w", encoding="utf-8") as f:
                f.write("x,y\n")
                for x, y in pts:
                    f.write(f"{int(x)},{int(y)}\n")

            status = f"Harris {key}: {pts.shape[0]} esquinas. Exportado en: {outdir}"

            if update_view:
                self.last_output = pil_over
                self.view.set_images(pil_orig, pil_resp, pil_over)
                self.view.set_status(status)

    def save_output(self, path: str | Path) -> None:
        if self.last_output is None:
            self.view.set_status("No hay salida para guardar aún.")
            return
        p = Path(path)
        p.parent.mkdir(parents=True, exist_ok=True)
        self.last_output.save(p)
        self.view.set_status(f"Salida guardada: {p}")
