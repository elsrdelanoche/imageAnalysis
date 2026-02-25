from __future__ import annotations

from dataclasses import dataclass
from typing import Any

import numpy as np


@dataclass
class ImageArtifact:
    """A processed image plus metadata.

    - data is a numpy array in BGR (OpenCV) or single-channel.
    """

    id: str
    name: str
    data: np.ndarray
    meta: dict[str, Any]

    @property
    def shape(self) -> tuple[int, ...]:
        return self.data.shape
