from __future__ import annotations

import uuid
from typing import Any

import cv2
import numpy as np

from visioncore.core.image import ImageArtifact
from visioncore.core.operation import Operation, OperationContext
from visioncore.core.params import ParamSpec


def _id() -> str:
    return uuid.uuid4().hex[:12]


def _bgr(img: np.ndarray) -> np.ndarray:
    if img.ndim == 2:
        return cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)
    return img


def _gray(img: np.ndarray) -> np.ndarray:
    return img if img.ndim == 2 else cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)


class HarrisCorners(Operation):
    id = "feat.harris"
    name = "Harris Corners"
    group = "Feature Extraction"

    def params(self) -> list[ParamSpec]:
        return [
            ParamSpec("block", "Block size", "int", 2, min=2, max=10, step=1),
            ParamSpec("ksize", "Sobel ksize", "int", 3, min=3, max=7, step=2),
            ParamSpec("k", "Harris k", "float", 0.04, min=0.01, max=0.2, step=0.01),
            ParamSpec("thr", "Response threshold", "float", 0.01, min=0.001, max=0.2, step=0.001),
            ParamSpec("overlay", "Overlay on image", "bool", True),
        ]

    def apply(self, ctx: OperationContext, input_id: str, params: dict[str, Any]) -> ImageArtifact:
        img0 = ctx.artifacts[input_id].data
        gray = np.float32(_gray(img0))

        block = int(params.get("block", 2))
        ksize = int(params.get("ksize", 3))
        if ksize % 2 == 0:
            ksize += 1
        k = float(params.get("k", 0.04))
        thr = float(params.get("thr", 0.01))
        overlay = bool(params.get("overlay", True))

        dst = cv2.cornerHarris(gray, blockSize=block, ksize=ksize, k=k)
        dst = cv2.dilate(dst, None)
        response = dst / (dst.max() + 1e-9)

        if overlay:
            out = _bgr(img0).copy()
            ys, xs = np.where(response > thr)
            for (x, y) in zip(xs.tolist(), ys.tolist()):
                cv2.circle(out, (int(x), int(y)), 2, (0, 0, 255), 1)
        else:
            out = (np.clip(response, 0, 1) * 255).astype(np.uint8)

        return ImageArtifact(id=_id(), name=self.name, data=out, meta={"op": self.id, **params})
