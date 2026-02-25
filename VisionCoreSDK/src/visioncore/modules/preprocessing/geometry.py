from __future__ import annotations

import uuid
from typing import Any

import cv2

from visioncore.core.image import ImageArtifact
from visioncore.core.operation import Operation, OperationContext
from visioncore.core.params import ParamSpec


def _id() -> str:
    return uuid.uuid4().hex[:12]


class Rotate(Operation):
    id = "pre.rotate"
    name = "Rotate"
    group = "Preprocessing"

    def params(self) -> list[ParamSpec]:
        return [
            ParamSpec("angle", "Angle (deg)", "float", 0.0, min=-180.0, max=180.0, step=1.0),
            ParamSpec(
                "interp",
                "Interpolation",
                "choice",
                "Bilinear",
                choices=["Nearest", "Bilinear", "Bicubic"],
            ),
        ]

    def apply(self, ctx: OperationContext, input_id: str, params: dict[str, Any]) -> ImageArtifact:
        img = ctx.artifacts[input_id].data
        angle = float(params.get("angle", 0.0))
        interp = params.get("interp", "Bilinear")
        inter = {
            "Nearest": cv2.INTER_NEAREST,
            "Bilinear": cv2.INTER_LINEAR,
            "Bicubic": cv2.INTER_CUBIC,
        }[interp]

        h, w = img.shape[:2]
        M = cv2.getRotationMatrix2D((w / 2, h / 2), angle, 1.0)
        out = cv2.warpAffine(img, M, (w, h), flags=inter, borderMode=cv2.BORDER_REPLICATE)
        return ImageArtifact(id=_id(), name=f"{self.name}", data=out, meta={"op": self.id, **params})


class Scale(Operation):
    id = "pre.scale"
    name = "Scale"
    group = "Preprocessing"

    def params(self) -> list[ParamSpec]:
        return [
            ParamSpec("sx", "Scale X", "float", 1.0, min=0.1, max=5.0, step=0.05),
            ParamSpec("sy", "Scale Y", "float", 1.0, min=0.1, max=5.0, step=0.05),
            ParamSpec(
                "interp",
                "Interpolation",
                "choice",
                "Bilinear",
                choices=["Nearest", "Bilinear", "Area"],
            ),
        ]

    def apply(self, ctx: OperationContext, input_id: str, params: dict[str, Any]) -> ImageArtifact:
        img = ctx.artifacts[input_id].data
        sx = float(params.get("sx", 1.0))
        sy = float(params.get("sy", 1.0))
        interp = params.get("interp", "Bilinear")
        inter = {
            "Nearest": cv2.INTER_NEAREST,
            "Bilinear": cv2.INTER_LINEAR,
            "Area": cv2.INTER_AREA,
        }[interp]
        out = cv2.resize(img, None, fx=sx, fy=sy, interpolation=inter)
        return ImageArtifact(id=_id(), name=f"{self.name}", data=out, meta={"op": self.id, **params})
