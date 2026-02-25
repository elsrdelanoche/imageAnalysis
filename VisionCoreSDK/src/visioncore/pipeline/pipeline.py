from __future__ import annotations

import uuid
from dataclasses import dataclass
from typing import Any

import cv2
import numpy as np

from visioncore.core.exceptions import ImageNotLoadedError
from visioncore.core.image import ImageArtifact
from visioncore.core.operation import Operation, OperationContext


def _new_id() -> str:
    return uuid.uuid4().hex[:12]


@dataclass
class PipelineResult:
    artifact: ImageArtifact
    active_id: str


class ImagePipeline:
    """Holds the working set of images and executes operations.

    This is the 'Model' side: no UI dependencies.
    """

    def __init__(self) -> None:
        self._artifacts: dict[str, ImageArtifact] = {}
        self._active_id: str | None = None

    @property
    def artifacts(self) -> dict[str, ImageArtifact]:
        return self._artifacts

    @property
    def active_id(self) -> str | None:
        return self._active_id

    def set_active(self, artifact_id: str) -> None:
        if artifact_id not in self._artifacts:
            raise KeyError(f"Unknown artifact: {artifact_id}")
        self._active_id = artifact_id

    def load_image(self, path: str) -> PipelineResult:
        data = cv2.imread(path, cv2.IMREAD_UNCHANGED)
        if data is None:
            raise ImageNotLoadedError(f"Could not read: {path}")

        # Normalize to BGR for display/ops.
        if data.ndim == 2:
            name = "Loaded (gray)"
        else:
            if data.shape[2] == 4:
                # BGRA -> BGR
                data = cv2.cvtColor(data, cv2.COLOR_BGRA2BGR)
            name = "Loaded"

        artifact = ImageArtifact(
            id=_new_id(),
            name=name,
            data=data,
            meta={"source_path": path},
        )
        self._artifacts[artifact.id] = artifact
        self._active_id = artifact.id
        return PipelineResult(artifact=artifact, active_id=artifact.id)

    def execute(
        self,
        op: Operation,
        params: dict[str, Any] | None = None,
        input_id: str | None = None,
    ) -> PipelineResult:
        if params is None:
            params = {}

        if input_id is None:
            if self._active_id is None:
                raise ImageNotLoadedError("No active image")
            input_id = self._active_id

        ctx = OperationContext(artifacts=self._artifacts)
        out = op.apply(ctx, input_id, params)
        self._artifacts[out.id] = out
        self._active_id = out.id
        return PipelineResult(artifact=out, active_id=out.id)

    def remove(self, artifact_id: str) -> None:
        self._artifacts.pop(artifact_id, None)
        if self._active_id == artifact_id:
            self._active_id = next(iter(self._artifacts.keys()), None)
