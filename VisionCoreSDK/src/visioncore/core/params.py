from __future__ import annotations

from dataclasses import dataclass
from typing import Any, Literal


ParamType = Literal["int", "float", "bool", "choice"]


@dataclass(frozen=True)
class ParamSpec:
    key: str
    label: str
    type: ParamType
    default: Any
    min: float | int | None = None
    max: float | int | None = None
    step: float | int | None = None
    choices: list[str] | None = None

    def validate(self, value: Any) -> Any:
        if self.type == "int":
            v = int(value)
            if self.min is not None and v < int(self.min):
                raise ValueError(f"{self.key} below min")
            if self.max is not None and v > int(self.max):
                raise ValueError(f"{self.key} above max")
            return v

        if self.type == "float":
            v = float(value)
            if self.min is not None and v < float(self.min):
                raise ValueError(f"{self.key} below min")
            if self.max is not None and v > float(self.max):
                raise ValueError(f"{self.key} above max")
            return v

        if self.type == "bool":
            if isinstance(value, str):
                return value.strip().lower() in {"1", "true", "yes", "y", "on"}
            return bool(value)

        if self.type == "choice":
            if self.choices is None:
                raise ValueError("choices missing")
            v = str(value)
            if v not in self.choices:
                raise ValueError(f"{self.key} invalid choice")
            return v

        raise ValueError(f"Unknown type: {self.type}")
