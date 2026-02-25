from __future__ import annotations

import uuid
from typing import Any

import cv2

from visioncore.core.image import ImageArtifact
from visioncore.core.operation import Operation, OperationContext
from visioncore.core.params import ParamSpec


def _id() -> str:
    return uuid.uuid4().hex[:12]


class BinaryThreshold(Operation):
    id = "pre.threshold"
    name = "Binary Threshold"
    group = "Preprocessing"

    def params(self) -> list[ParamSpec]:
        return [
            ParamSpec("t", "Threshold", "int", 128, min=0, max=255, step=1),
            ParamSpec("invert", "Invert", "bool", False),
        ]

    def apply(self, ctx: OperationContext, input_id: str, params: dict[str, Any]) -> ImageArtifact:
        img = ctx.artifacts[input_id].data
        if img.ndim != 2:
            img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        t = int(params.get("t", 128))
        invert = bool(params.get("invert", False))
        typ = cv2.THRESH_BINARY_INV if invert else cv2.THRESH_BINARY
        _, out = cv2.threshold(img, t, 255, typ)
        return ImageArtifact(id=_id(), name=f"{self.name}", data=out, meta={"op": self.id, **params})
