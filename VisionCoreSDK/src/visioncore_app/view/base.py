from __future__ import annotations

from dataclasses import dataclass
from typing import Any, Protocol

from visioncore.core.params import ParamSpec


class ViewPort(Protocol):
    """UI contract consumed by controllers."""

    def set_status(self, text: str) -> None: ...

    def add_artifact_to_canvas(self, artifact_id: str, name: str, img_bgr_or_gray) -> None: ...

    def remove_artifact_from_canvas(self, artifact_id: str) -> None: ...

    def set_active_artifact(self, artifact_id: str) -> None: ...

    def set_operation_catalog(self, catalog: dict[str, list[tuple[str, str]]]) -> None: ...

    def set_param_form(self, specs: list[ParamSpec], values: dict[str, Any]) -> None: ...

    def get_param_values(self) -> dict[str, Any]: ...


@dataclass(frozen=True)
class UiEvents:
    """Callbacks the View uses to talk to controllers."""

    on_open_image: callable
    on_save_active: callable
    on_delete_active: callable
    on_select_operation: callable
    on_apply_operation: callable
    on_set_active: callable
