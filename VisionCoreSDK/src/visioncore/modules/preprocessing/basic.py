from __future__ import annotations

import uuid
from typing import Any

import cv2
import numpy as np

from visioncore.core.image import ImageArtifact
from visioncore.core.operation import Operation, OperationContext, ensure_uint8
from visioncore.core.params import ParamSpec


def _id() -> str:
    return uuid.uuid4().hex[:12]


class Grayscale(Operation):
    id = "pre.gray"
    name = "Grayscale"
    group = "Preprocessing"

    def params(self) -> list[ParamSpec]:
        return [
            ParamSpec(
                key="method",
                label="Method",
                type="choice",
                default="BT.601",
                choices=["Average", "BT.601", "BT.709"],
            )
        ]

    def apply(self, ctx: OperationContext, input_id: str, params: dict[str, Any]) -> ImageArtifact:
        img = ctx.artifacts[input_id].data
        if img.ndim == 2:
            gray = img.copy()
        else:
            method = params.get("method", "BT.601")
            b, g, r = cv2.split(img)
            if method == "Average":
                gray = ((r.astype(np.float32) + g.astype(np.float32) + b.astype(np.float32)) / 3.0)
            elif method == "BT.709":
                gray = 0.2126 * r + 0.7152 * g + 0.0722 * b
            else:  # BT.601
                gray = 0.299 * r + 0.587 * g + 0.114 * b
            gray = ensure_uint8(gray)
        return ImageArtifact(id=_id(), name=f"{self.name}", data=gray, meta={"op": self.id, **params})


class Negative(Operation):
    id = "pre.negative"
    name = "Negative"
    group = "Preprocessing"

    def params(self) -> list[ParamSpec]:
        return []

    def apply(self, ctx: OperationContext, input_id: str, params: dict[str, Any]) -> ImageArtifact:
        img = ctx.artifacts[input_id].data
        out = 255 - img
        return ImageArtifact(id=_id(), name=f"{self.name}", data=out, meta={"op": self.id})


class BrightnessContrast(Operation):
    id = "pre.brightness_contrast"
    name = "Brightness / Contrast"
    group = "Preprocessing"

    def params(self) -> list[ParamSpec]:
        return [
            ParamSpec("alpha", "Contrast (alpha)", "float", 1.0, min=0.0, max=3.0, step=0.05),
            ParamSpec("beta", "Brightness (beta)", "int", 0, min=-128, max=128, step=1),
        ]

    def apply(self, ctx: OperationContext, input_id: str, params: dict[str, Any]) -> ImageArtifact:
        img = ctx.artifacts[input_id].data
        alpha = float(params.get("alpha", 1.0))
        beta = int(params.get("beta", 0))
        out = cv2.convertScaleAbs(img, alpha=alpha, beta=beta)
        return ImageArtifact(id=_id(), name=f"{self.name}", data=out, meta={"op": self.id, **params})


class HistogramEqualization(Operation):
    id = "pre.histeq"
    name = "Histogram Equalization"
    group = "Preprocessing"

    def params(self) -> list[ParamSpec]:
        return [
            ParamSpec("mode", "Mode", "choice", "Luma", choices=["Gray", "Luma"])
        ]

    def apply(self, ctx: OperationContext, input_id: str, params: dict[str, Any]) -> ImageArtifact:
        img = ctx.artifacts[input_id].data
        mode = params.get("mode", "Luma")
        if img.ndim == 2 or mode == "Gray":
            gray = img if img.ndim == 2 else cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
            out = cv2.equalizeHist(gray)
        else:
            ycrcb = cv2.cvtColor(img, cv2.COLOR_BGR2YCrCb)
            y, cr, cb = cv2.split(ycrcb)
            y = cv2.equalizeHist(y)
            merged = cv2.merge([y, cr, cb])
            out = cv2.cvtColor(merged, cv2.COLOR_YCrCb2BGR)
        return ImageArtifact(id=_id(), name=f"{self.name}", data=out, meta={"op": self.id, **params})
