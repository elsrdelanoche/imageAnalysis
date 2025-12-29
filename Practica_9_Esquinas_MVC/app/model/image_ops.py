from __future__ import annotations

from dataclasses import dataclass
from typing import Dict, List, Tuple, Literal, Optional

import numpy as np


PadMode = Literal["reflect", "edge", "constant"]


def convolve2d(img: np.ndarray, kernel: np.ndarray, pad: PadMode = "reflect") -> np.ndarray:
    """Convolución 2D (correlación) vectorizada para kernels pequeños.
    - img: HxW float32
    - kernel: kh x kw float32
    """
    img = np.asarray(img, dtype=np.float32)
    k = np.asarray(kernel, dtype=np.float32)
    kh, kw = k.shape
    assert kh % 2 == 1 and kw % 2 == 1, "Kernel debe ser de tamaño impar"
    ph, pw = kh // 2, kw // 2

    if pad == "reflect":
        padded = np.pad(img, ((ph, ph), (pw, pw)), mode="reflect")
    elif pad == "edge":
        padded = np.pad(img, ((ph, ph), (pw, pw)), mode="edge")
    else:
        padded = np.pad(img, ((ph, ph), (pw, pw)), mode="constant", constant_values=0.0)

    H, W = img.shape
    out = np.zeros((H, W), dtype=np.float32)

    # correlación: sum(k[i,j] * padded[i:i+H, j:j+W])
    for i in range(kh):
        for j in range(kw):
            out += k[i, j] * padded[i:i+H, j:j+W]
    return out


# ----------------------------
# Kirsch (8 máscaras 3x3)
# ----------------------------
def kirsch_kernels() -> Dict[str, np.ndarray]:
    # Basado en las máscaras estándar (N, NW, W, SW, S, SE, E, NE)
    return {
        "N":  np.array([[ 5,  5,  5],
                        [-3,  0, -3],
                        [-3, -3, -3]], dtype=np.float32),
        "NW": np.array([[ 5,  5, -3],
                        [ 5,  0, -3],
                        [-3, -3, -3]], dtype=np.float32),
        "W":  np.array([[ 5, -3, -3],
                        [ 5,  0, -3],
                        [ 5, -3, -3]], dtype=np.float32),
        "SW": np.array([[-3, -3, -3],
                        [ 5,  0, -3],
                        [ 5,  5, -3]], dtype=np.float32),
        "S":  np.array([[-3, -3, -3],
                        [-3,  0, -3],
                        [ 5,  5,  5]], dtype=np.float32),
        "SE": np.array([[-3, -3, -3],
                        [-3,  0,  5],
                        [-3,  5,  5]], dtype=np.float32),
        "E":  np.array([[-3, -3,  5],
                        [-3,  0,  5],
                        [-3, -3,  5]], dtype=np.float32),
        "NE": np.array([[-3,  5,  5],
                        [-3,  0,  5],
                        [-3, -3, -3]], dtype=np.float32),
    }


def kirsch_response(gray: np.ndarray, mode: Literal["max", "max_abs"] = "max_abs",
                   pad: PadMode = "reflect") -> Tuple[np.ndarray, Dict[str, np.ndarray]]:
    kernels = kirsch_kernels()
    responses: Dict[str, np.ndarray] = {}
    for name, k in kernels.items():
        responses[name] = convolve2d(gray, k, pad=pad)
    stack = np.stack(list(responses.values()), axis=0)  # 8xHxW
    if mode == "max_abs":
        combined = np.max(np.abs(stack), axis=0)
    else:
        combined = np.max(stack, axis=0)
    return combined.astype(np.float32), responses


# ----------------------------
# Frei-Chen (9 máscaras 3x3)
# ----------------------------
def frei_chen_kernels() -> Dict[str, np.ndarray]:
    s2 = np.sqrt(2.0).astype(np.float32) if hasattr(np.sqrt(2.0), 'astype') else np.float32(np.sqrt(2.0))
    s2 = np.float32(np.sqrt(2.0))
    return {
        "W1": np.array([[ 1,  s2,  1],
                        [ 0,  0,  0],
                        [-1, -s2, -1]], dtype=np.float32),
        "W2": np.array([[ 1,  0, -1],
                        [ s2, 0, -s2],
                        [ 1,  0, -1]], dtype=np.float32),
        "W3": np.array([[ 0, -1,  s2],
                        [ 1,  0, -1],
                        [-s2, 1,  0]], dtype=np.float32),
        "W4": np.array([[ s2, -1,  0],
                        [-1,  0,  1],
                        [ 0,  1, -s2]], dtype=np.float32),
        "W5": np.array([[ 0,  1,  0],
                        [-1,  0, -1],
                        [ 0,  1,  0]], dtype=np.float32),
        "W6": np.array([[-1,  0,  1],
                        [ 0,  0,  0],
                        [ 1,  0, -1]], dtype=np.float32),
        "W7": np.array([[ 1, -2,  1],
                        [-2,  4, -2],
                        [ 1, -2,  1]], dtype=np.float32),
        "W8": np.array([[-2,  1, -2],
                        [ 1,  4,  1],
                        [-2,  1, -2]], dtype=np.float32),
        "W9": np.array([[ 1,  1,  1],
                        [ 1,  1,  1],
                        [ 1,  1,  1]], dtype=np.float32),
    }


def frei_chen_edge_strength(gray: np.ndarray, pad: PadMode = "reflect",
                           eps: float = 1e-6) -> Tuple[np.ndarray, Dict[str, np.ndarray]]:
    """Devuelve la razón r = sqrt(sum_{i=1..4} Gi^2 / sum_{i=1..9} Gi^2)
    donde Gi = (B * Wi) y * es convolución/correlación 2D.
    """
    kernels = frei_chen_kernels()
    G: Dict[str, np.ndarray] = {}
    for name, k in kernels.items():
        G[name] = convolve2d(gray, k, pad=pad)

    num = (G["W1"]**2 + G["W2"]**2 + G["W3"]**2 + G["W4"]**2)
    den = (num + G["W5"]**2 + G["W6"]**2 + G["W7"]**2 + G["W8"]**2 + G["W9"]**2)
    r = np.sqrt(num / (den + eps))
    return r.astype(np.float32), G


# ----------------------------
# Harris–Stephens
# ----------------------------
def gaussian_kernel(size: int, sigma: float) -> np.ndarray:
    size = int(size)
    if size % 2 == 0:
        size += 1
    sigma = float(max(1e-6, sigma))
    ax = np.arange(-(size//2), size//2 + 1, dtype=np.float32)
    xx, yy = np.meshgrid(ax, ax)
    k = np.exp(-(xx**2 + yy**2) / (2.0 * sigma * sigma))
    k /= np.sum(k)
    return k.astype(np.float32)


def sobel_kernels() -> Tuple[np.ndarray, np.ndarray]:
    kx = np.array([[-1, 0, 1],
                   [-2, 0, 2],
                   [-1, 0, 1]], dtype=np.float32)
    ky = np.array([[-1, -2, -1],
                   [ 0,  0,  0],
                   [ 1,  2,  1]], dtype=np.float32)
    return kx, ky


def harris_response(gray: np.ndarray, k: float = 0.04,
                    sigma: float = 1.0,
                    grad: Literal["sobel"] = "sobel",
                    pad: PadMode = "reflect") -> np.ndarray:
    gray = np.asarray(gray, dtype=np.float32)

    # Gradientes
    kx, ky = sobel_kernels()
    Ix = convolve2d(gray, kx, pad=pad)
    Iy = convolve2d(gray, ky, pad=pad)

    Ixx = Ix * Ix
    Iyy = Iy * Iy
    Ixy = Ix * Iy

    # Suavizado (ventana gaussiana)
    # tamaño ~ 6*sigma, mínimo 3, impar
    gsize = max(3, int(np.ceil(6.0 * sigma)) | 1)
    gk = gaussian_kernel(gsize, sigma)

    Sxx = convolve2d(Ixx, gk, pad=pad)
    Syy = convolve2d(Iyy, gk, pad=pad)
    Sxy = convolve2d(Ixy, gk, pad=pad)

    det = (Sxx * Syy) - (Sxy * Sxy)
    trace = Sxx + Syy
    R = det - (float(k) * (trace * trace))
    return R.astype(np.float32)


def nms_peaks(R: np.ndarray, thresh_rel: float = 0.01,
              nms_size: int = 5,
              max_points: int = 2000) -> Tuple[np.ndarray, np.ndarray]:
    """Non-maximum suppression sobre R (Harris).
    Retorna: (mask, points_xy) con points_xy Nx2 (x,y).
    """
    R = np.asarray(R, dtype=np.float32)
    H, W = R.shape
    n = int(max(3, nms_size))
    if n % 2 == 0:
        n += 1
    pad = n // 2

    Rmax = float(np.max(R)) if R.size else 0.0
    thr = float(thresh_rel) * Rmax
    if Rmax <= 0 or not np.isfinite(Rmax):
        return np.zeros_like(R, dtype=bool), np.zeros((0, 2), dtype=np.int32)

    # sliding window max (vectorizado)
    Rp = np.pad(R, ((pad, pad), (pad, pad)), mode="constant", constant_values=-np.inf)
    try:
        from numpy.lib.stride_tricks import sliding_window_view
        windows = sliding_window_view(Rp, (n, n))
        local_max = windows.max(axis=(-2, -1))
    except Exception:
        # fallback lento
        local_max = np.empty_like(R, dtype=np.float32)
        for y in range(H):
            for x in range(W):
                local_max[y, x] = np.max(Rp[y:y+n, x:x+n])

    mask = (R >= thr) & (R == local_max)

    ys, xs = np.where(mask)
    if ys.size == 0:
        return mask, np.zeros((0, 2), dtype=np.int32)

    # Ordenar por R descendente y limitar
    vals = R[ys, xs]
    order = np.argsort(-vals)
    if order.size > max_points:
        order = order[:max_points]
    xs = xs[order]; ys = ys[order]

    # Si limitamos puntos, actualizamos mask
    if ys.size != np.count_nonzero(mask):
        mask2 = np.zeros_like(mask)
        mask2[ys, xs] = True
        mask = mask2

    pts = np.stack([xs, ys], axis=1).astype(np.int32)
    return mask, pts
