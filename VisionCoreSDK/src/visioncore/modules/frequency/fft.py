from __future__ import annotations

import math
import uuid
from typing import Any

import cv2
import numpy as np

from visioncore.core.image import ImageArtifact
from visioncore.core.operation import Operation, OperationContext, ensure_uint8
from visioncore.core.params import ParamSpec


def _id() -> str:
    return uuid.uuid4().hex[:12]


def _next_pow2(n: int) -> int:
    return 1 if n <= 1 else 2 ** int(math.ceil(math.log2(n)))


def _fft2_gray(img: np.ndarray) -> tuple[np.ndarray, tuple[int, int]]:
    if img.ndim != 2:
        img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    h, w = img.shape
    H, W = _next_pow2(h), _next_pow2(w)
    pad = np.zeros((H, W), dtype=np.float32)
    pad[:h, :w] = img.astype(np.float32) / 255.0
    F = np.fft.fftshift(np.fft.fft2(pad))
    return F, (h, w)


class FFTSpectrum(Operation):
    id = "freq.fft"
    name = "FFT Spectrum"
    group = "Frequency"

    def params(self) -> list[ParamSpec]:
        return [
            ParamSpec(
                "view",
                "View",
                "choice",
                "Magnitude",
                choices=["Magnitude", "Phase"],
            ),
            ParamSpec("log", "Log scale", "bool", True),
        ]

    def apply(self, ctx: OperationContext, input_id: str, params: dict[str, Any]) -> ImageArtifact:
        img = ctx.artifacts[input_id].data
        view = params.get("view", "Magnitude")
        log = bool(params.get("log", True))

        F, _ = _fft2_gray(img)
        if view == "Phase":
            out = np.angle(F)
            out = (out + np.pi) / (2 * np.pi)  # 0..1
        else:
            mag = np.abs(F)
            if log:
                mag = np.log1p(mag)
            mag = mag / (mag.max() + 1e-9)
            out = mag

        out = ensure_uint8(out * 255.0)
        return ImageArtifact(id=_id(), name=f"{self.name}: {view}", data=out, meta={"op": self.id, **params})
