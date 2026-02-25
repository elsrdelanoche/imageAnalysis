from __future__ import annotations

import uuid
from typing import Any

import cv2

from visioncore.core.image import ImageArtifact
from visioncore.core.operation import Operation, OperationContext
from visioncore.core.params import ParamSpec


def _id() -> str:
    return uuid.uuid4().hex[:12]


def _ensure_gray(img):
    return img if img.ndim == 2 else cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)


class Morphology(Operation):
    id = "pre.morphology"
    name = "Morphology"
    group = "Preprocessing"

    def params(self) -> list[ParamSpec]:
        return [
            ParamSpec(
                "op",
                "Operation",
                "choice",
                "Erode",
                choices=["Erode", "Dilate", "Open", "Close"],
            ),
            ParamSpec("k", "Kernel size (odd)", "int", 3, min=3, max=31, step=2),
            ParamSpec(
                "shape",
                "Kernel shape",
                "choice",
                "Rect",
                choices=["Rect", "Ellipse", "Cross"],
            ),
        ]

    def apply(self, ctx: OperationContext, input_id: str, params: dict[str, Any]) -> ImageArtifact:
        img = _ensure_gray(ctx.artifacts[input_id].data)
        op = params.get("op", "Erode")
        k = int(params.get("k", 3))
        if k % 2 == 0:
            k += 1
        shape = params.get("shape", "Rect")
        shp = {"Rect": cv2.MORPH_RECT, "Ellipse": cv2.MORPH_ELLIPSE, "Cross": cv2.MORPH_CROSS}[shape]
        kernel = cv2.getStructuringElement(shp, (k, k))

        if op == "Erode":
            out = cv2.erode(img, kernel)
        elif op == "Dilate":
            out = cv2.dilate(img, kernel)
        elif op == "Open":
            out = cv2.morphologyEx(img, cv2.MORPH_OPEN, kernel)
        else:
            out = cv2.morphologyEx(img, cv2.MORPH_CLOSE, kernel)

        return ImageArtifact(id=_id(), name=f"{self.name}: {op}", data=out, meta={"op": self.id, **params})
