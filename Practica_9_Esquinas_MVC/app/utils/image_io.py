from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Optional, Tuple

import numpy as np
from PIL import Image


@dataclass
class LoadedImage:
    path: Optional[Path]
    pil: Image.Image  # RGB
    gray: np.ndarray  # float32 in [0,255]
    rgb: np.ndarray   # uint8 HxWx3


def load_image(path: str | Path) -> LoadedImage:
    p = Path(path)
    pil = Image.open(p).convert("RGB")
    rgb = np.array(pil, dtype=np.uint8)
    # luminancia aproximada (sRGB)
    gray = (0.2126 * rgb[..., 0] + 0.7152 * rgb[..., 1] + 0.0722 * rgb[..., 2]).astype(np.float32)
    return LoadedImage(path=p, pil=pil, gray=gray, rgb=rgb)


def to_uint8_img(x: np.ndarray) -> np.ndarray:
    """Normaliza un mapa (float) a uint8 [0,255]."""
    if x.size == 0:
        return np.zeros((1, 1), dtype=np.uint8)
    x = np.asarray(x, dtype=np.float32)
    mn = float(np.nanmin(x))
    mx = float(np.nanmax(x))
    if not np.isfinite(mn) or not np.isfinite(mx) or mx <= mn:
        return np.zeros_like(x, dtype=np.uint8)
    y = (x - mn) / (mx - mn)
    y = np.clip(y, 0.0, 1.0)
    return (y * 255.0 + 0.5).astype(np.uint8)


def overlay_points(rgb: np.ndarray, points_xy: np.ndarray, radius: int = 2) -> np.ndarray:
    """Dibuja puntos (x,y) sobre una imagen RGB uint8. Devuelve copia."""
    from PIL import ImageDraw

    img = Image.fromarray(rgb.copy(), mode="RGB")
    draw = ImageDraw.Draw(img)
    r = int(max(1, radius))
    for (x, y) in points_xy:
        x = int(x); y = int(y)
        draw.ellipse((x - r, y - r, x + r, y + r), outline=(255, 0, 0), width=2)
    return np.array(img, dtype=np.uint8)


def resize_to_fit(pil_img: Image.Image, max_size: Tuple[int, int]) -> Image.Image:
    """Escala manteniendo aspecto para encajar en max_size."""
    w, h = pil_img.size
    max_w, max_h = max_size
    if w <= 0 or h <= 0:
        return pil_img
    scale = min(max_w / w, max_h / h)
    if scale >= 1.0:
        return pil_img
    new_w = max(1, int(w * scale))
    new_h = max(1, int(h * scale))
    return pil_img.resize((new_w, new_h), resample=Image.BILINEAR)
