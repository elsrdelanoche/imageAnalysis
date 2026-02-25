from __future__ import annotations

from dataclasses import dataclass

import cv2

from visioncore.core.exceptions import VisionCoreError
from visioncore.core.operation import Operation
from visioncore.modules.registry import all_operations
from visioncore.pipeline.pipeline import ImagePipeline

from visioncore_app.view.base import ViewPort


@dataclass
class CatalogItem:
    op_id: str
    op_name: str
    group: str


class AppController:
    """Coordinates the UI with the processing pipeline."""

    def __init__(self, view: ViewPort):
        self.view = view
        self.model = ImagePipeline()

        self._ops: dict[str, Operation] = {op.id: op for op in all_operations()}
        self._selected_op_id: str | None = None

        # Build operation catalog for UI
        grouped: dict[str, list[tuple[str, str]]] = {}
        for op in self._ops.values():
            grouped.setdefault(op.group, []).append((op.id, op.name))
        # Stable ordering
        grouped = {k: sorted(v, key=lambda x: x[1]) for k, v in sorted(grouped.items())}
        self.view.set_operation_catalog(grouped)

        self.view.set_status("Ready")

    # ---------- File actions ----------
    def open_image(self, path: str) -> None:
        res = self.model.load_image(path)
        self.view.add_artifact_to_canvas(res.artifact.id, res.artifact.name, res.artifact.data)
        self.view.set_active_artifact(res.active_id)
        self.view.set_status(f"Loaded: {path}")

    def save_active(self, path: str) -> None:
        aid = self.model.active_id
        if not aid:
            self.view.set_status("No active image")
            return
        img = self.model.artifacts[aid].data
        ok = cv2.imwrite(path, img)
        self.view.set_status("Saved" if ok else "Save failed")

    def delete_active(self) -> None:
        aid = self.model.active_id
        if not aid:
            return
        self.model.remove(aid)
        self.view.remove_artifact_from_canvas(aid)
        if self.model.active_id:
            self.view.set_active_artifact(self.model.active_id)
        self.view.set_status("Deleted")

    # ---------- Operation workflow ----------
    def select_operation(self, op_id: str) -> None:
        self._selected_op_id = op_id
        op = self._ops[op_id]
        specs = op.params()
        defaults = {s.key: s.default for s in specs}
        self.view.set_param_form(specs, defaults)
        self.view.set_status(f"Selected: {op.name}")

    def apply_selected_operation(self, params: dict) -> None:
        if not self._selected_op_id:
            self.view.set_status("Select an operation")
            return
        op = self._ops[self._selected_op_id]
        res = self.model.execute(op, params=params)
        self.view.add_artifact_to_canvas(res.artifact.id, res.artifact.name, res.artifact.data)
        self.view.set_active_artifact(res.active_id)
        self.view.set_status(f"Applied: {op.name}")

    def set_active(self, artifact_id: str) -> None:
        try:
            self.model.set_active(artifact_id)
            self.view.set_active_artifact(artifact_id)
        except Exception:
            return
