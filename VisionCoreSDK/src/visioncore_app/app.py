from __future__ import annotations

import sys

from PySide6.QtWidgets import QApplication

from visioncore_app.controller.app_controller import AppController
from visioncore_app.view.base import UiEvents
from visioncore_app.view.main_window import MainWindow


def main() -> int:
    app = QApplication(sys.argv)

    controller_holder: dict[str, AppController] = {}

    def _controller() -> AppController:
        return controller_holder["c"]

    # View -> Controller callbacks
    def on_open_image() -> None:
        path = win.ask_open_path()
        if not path:
            return
        try:
            _controller().open_image(path)
        except Exception as e:
            win.set_status(f"Open failed: {e}")

    def on_save_active() -> None:
        path = win.ask_save_path()
        if not path:
            return
        try:
            _controller().save_active(path)
        except Exception as e:
            win.set_status(f"Save failed: {e}")

    def on_delete_active() -> None:
        try:
            _controller().delete_active()
        except Exception as e:
            win.set_status(f"Delete failed: {e}")

    def on_select_operation(op_id: str) -> None:
        try:
            _controller().select_operation(op_id)
        except Exception as e:
            win.set_status(f"Select failed: {e}")

    def on_apply_operation() -> None:
        try:
            params = win.get_param_values()
            _controller().apply_selected_operation(params)
        except Exception as e:
            win.set_status(f"Apply failed: {e}")

    def on_set_active(aid: str) -> None:
        _controller().set_active(aid)

    win = MainWindow(
        UiEvents(
            on_open_image=on_open_image,
            on_save_active=on_save_active,
            on_delete_active=on_delete_active,
            on_select_operation=on_select_operation,
            on_apply_operation=on_apply_operation,
            on_set_active=on_set_active,
        )
    )

    controller_holder["c"] = AppController(win)

    win.show()
    return app.exec()


if __name__ == "__main__":
    raise SystemExit(main())
