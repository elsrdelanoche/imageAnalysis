from __future__ import annotations

import uuid
from typing import Any

import cv2

from visioncore.core.image import ImageArtifact
from visioncore.core.operation import Operation, OperationContext
from visioncore.core.params import ParamSpec


def _id() -> str:
    return uuid.uuid4().hex[:12]


def _gray(img):
    return img if img.ndim == 2 else cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)


class GaussianBlur(Operation):
    id = "seg.gaussian_blur"
    name = "Gaussian Blur"
    group = "Segmentation & Edges"

    def params(self) -> list[ParamSpec]:
        return [
            ParamSpec("k", "Kernel size (odd)", "int", 5, min=1, max=51, step=2),
            ParamSpec("sigma", "Sigma", "float", 1.0, min=0.0, max=20.0, step=0.1),
        ]

    def apply(self, ctx: OperationContext, input_id: str, params: dict[str, Any]) -> ImageArtifact:
        img = ctx.artifacts[input_id].data
        k = int(params.get("k", 5))
        if k % 2 == 0:
            k += 1
        sigma = float(params.get("sigma", 1.0))
        out = cv2.GaussianBlur(img, (k, k), sigmaX=sigma)
        return ImageArtifact(id=_id(), name=self.name, data=out, meta={"op": self.id, **params})


class HighPass(Operation):
    id = "seg.highpass"
    name = "High-pass (unsharp)"
    group = "Segmentation & Edges"

    def params(self) -> list[ParamSpec]:
        return [
            ParamSpec("k", "Kernel size (odd)", "int", 5, min=1, max=51, step=2),
            ParamSpec("sigma", "Sigma", "float", 1.0, min=0.0, max=20.0, step=0.1),
            ParamSpec("alpha", "Alpha", "float", 1.0, min=0.0, max=5.0, step=0.05),
        ]

    def apply(self, ctx: OperationContext, input_id: str, params: dict[str, Any]) -> ImageArtifact:
        img = ctx.artifacts[input_id].data
        k = int(params.get("k", 5))
        if k % 2 == 0:
            k += 1
        sigma = float(params.get("sigma", 1.0))
        alpha = float(params.get("alpha", 1.0))
        blur = cv2.GaussianBlur(img, (k, k), sigmaX=sigma)
        out = cv2.addWeighted(img, 1 + alpha, blur, -alpha, 0)
        return ImageArtifact(id=_id(), name=self.name, data=out, meta={"op": self.id, **params})


class SobelMagnitude(Operation):
    id = "seg.sobel"
    name = "Sobel Magnitude"
    group = "Segmentation & Edges"

    def params(self) -> list[ParamSpec]:
        return [ParamSpec("ksize", "Kernel size", "int", 3, min=1, max=7, step=2)]

    def apply(self, ctx: OperationContext, input_id: str, params: dict[str, Any]) -> ImageArtifact:
        img = _gray(ctx.artifacts[input_id].data)
        k = int(params.get("ksize", 3))
        if k % 2 == 0:
            k += 1
        gx = cv2.Sobel(img, cv2.CV_32F, 1, 0, ksize=k)
        gy = cv2.Sobel(img, cv2.CV_32F, 0, 1, ksize=k)
        mag = cv2.magnitude(gx, gy)
        mag = cv2.normalize(mag, None, 0, 255, cv2.NORM_MINMAX)
        out = mag.astype('uint8')
        return ImageArtifact(id=_id(), name=self.name, data=out, meta={"op": self.id, **params})


class Canny(Operation):
    id = "seg.canny"
    name = "Canny"
    group = "Segmentation & Edges"

    def params(self) -> list[ParamSpec]:
        return [
            ParamSpec("t_low", "Low threshold", "int", 50, min=0, max=255, step=1),
            ParamSpec("t_high", "High threshold", "int", 150, min=0, max=255, step=1),
            ParamSpec("aperture", "Aperture", "int", 3, min=3, max=7, step=2),
            ParamSpec("l2", "L2 gradient", "bool", False),
        ]

    def apply(self, ctx: OperationContext, input_id: str, params: dict[str, Any]) -> ImageArtifact:
        img = _gray(ctx.artifacts[input_id].data)
        t_low = int(params.get("t_low", 50))
        t_high = int(params.get("t_high", 150))
        aperture = int(params.get("aperture", 3))
        if aperture % 2 == 0:
            aperture += 1
        l2 = bool(params.get("l2", False))
        out = cv2.Canny(img, t_low, t_high, apertureSize=aperture, L2gradient=l2)
        return ImageArtifact(id=_id(), name=self.name, data=out, meta={"op": self.id, **params})
