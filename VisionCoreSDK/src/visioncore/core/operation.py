from __future__ import annotations

from abc import ABC, abstractmethod
from dataclasses import dataclass
from typing import Any

import numpy as np

from .image import ImageArtifact
from .params import ParamSpec


@dataclass(frozen=True)
class OperationContext:
    """Execution context passed into operations."""

    artifacts: dict[str, ImageArtifact]


class Operation(ABC):
    """Atomic transformation.

    Contract: takes one input artifact id, returns a new ImageArtifact.
    """

    id: str
    name: str
    group: str  # UI grouping

    @abstractmethod
    def params(self) -> list[ParamSpec]:
        raise NotImplementedError

    @abstractmethod
    def apply(
        self,
        ctx: OperationContext,
        input_id: str,
        params: dict[str, Any],
    ) -> ImageArtifact:
        raise NotImplementedError


def ensure_uint8(img: np.ndarray) -> np.ndarray:
    if img.dtype == np.uint8:
        return img
    img = np.clip(img, 0, 255)
    return img.astype(np.uint8)
