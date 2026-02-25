from __future__ import annotations

from typing import Any

from PySide6.QtCore import Qt
from PySide6.QtWidgets import (
    QCheckBox,
    QComboBox,
    QDockWidget,
    QDoubleSpinBox,
    QFileDialog,
    QFormLayout,
    QLabel,
    QMainWindow,
    QPushButton,
    QScrollArea,
    QSpinBox,
    QTreeWidget,
    QTreeWidgetItem,
    QWidget,
)

from visioncore.core.params import ParamSpec

from .base import UiEvents, ViewPort
from .canvas import VisionCanvas


_DARK_QSS = """
QMainWindow { background: #121212; color: #e6e6e6; }
QDockWidget { background: #151515; color: #e6e6e6; }
QWidget { background: #121212; color: #e6e6e6; font-size: 12px; }
QMenuBar { background: #151515; color: #e6e6e6; }
QMenuBar::item:selected { background: #2b2b2b; }
QMenu { background: #151515; color: #e6e6e6; }
QTreeWidget { background: #151515; border: 1px solid #2a2a2a; }
QPushButton { background: #1f1f1f; border: 1px solid #2a2a2a; padding: 8px; border-radius: 6px; }
QPushButton:hover { background: #2a2a2a; }
QScrollArea { border: none; }
"""


class MainWindow(QMainWindow):
    def __init__(self, events: UiEvents):
        super().__init__()
        self._events = events
        self.setWindowTitle("VisionCore SDK")
        self.resize(1300, 800)
        self.setStyleSheet(_DARK_QSS)

        self.canvas = VisionCanvas(self)
        self.setCentralWidget(self.canvas)

        self._op_tree = QTreeWidget()
        self._op_tree.setHeaderHidden(True)
        self._op_tree.itemSelectionChanged.connect(self._on_op_selected)

        self._param_container = QWidget()
        self._param_layout = QFormLayout(self._param_container)
        self._param_widgets: dict[str, QWidget] = {}

        self._apply_btn = QPushButton("Apply")
        self._apply_btn.clicked.connect(lambda: self._events.on_apply_operation())

        left = QWidget()
        left_layout = QFormLayout(left)
        left_layout.addRow(QLabel("Operations"))
        left_layout.addRow(self._op_tree)
        left_layout.addRow(QLabel("Parameters"))

        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        scroll.setWidget(self._param_container)
        left_layout.addRow(scroll)
        left_layout.addRow(self._apply_btn)

        dock = QDockWidget("Tools", self)
        dock.setWidget(left)
        dock.setAllowedAreas(Qt.LeftDockWidgetArea | Qt.RightDockWidgetArea)
        self.addDockWidget(Qt.LeftDockWidgetArea, dock)

        # file menu
        m = self.menuBar().addMenu("File")
        act_open = m.addAction("Open image")
        act_open.triggered.connect(lambda: self._events.on_open_image())
        act_save = m.addAction("Save active as…")
        act_save.triggered.connect(lambda: self._events.on_save_active())
        act_del = m.addAction("Delete active")
        act_del.triggered.connect(lambda: self._events.on_delete_active())
        m.addSeparator()
        act_quit = m.addAction("Quit")
        act_quit.triggered.connect(self.close)

        self.statusBar().showMessage("Ready")

        # canvas events
        self.canvas.signals.artifact_selected.connect(lambda aid: self._events.on_set_active(aid))
        self.canvas.signals.artifact_delete_requested.connect(lambda aid: self._events.on_set_active(aid) or self._events.on_delete_active())

        self._selected_op_id: str | None = None

    # ----------------- ViewPort API -----------------
    def set_status(self, text: str) -> None:
        self.statusBar().showMessage(text)

    def add_artifact_to_canvas(self, artifact_id: str, name: str, img_bgr_or_gray) -> None:
        self.canvas.add_artifact(artifact_id, img_bgr_or_gray, name=name)

    def remove_artifact_from_canvas(self, artifact_id: str) -> None:
        self.canvas.remove_artifact(artifact_id)

    def set_active_artifact(self, artifact_id: str) -> None:
        self.canvas.set_active(artifact_id)

    def set_operation_catalog(self, catalog: dict[str, list[tuple[str, str]]]) -> None:
        # {group: [(op_id, op_name), ...]}
        self._op_tree.clear()
        for group, ops in catalog.items():
            g = QTreeWidgetItem([group])
            g.setData(0, Qt.UserRole, None)
            self._op_tree.addTopLevelItem(g)
            for op_id, op_name in ops:
                it = QTreeWidgetItem([op_name])
                it.setData(0, Qt.UserRole, op_id)
                g.addChild(it)
            g.setExpanded(True)

    def set_param_form(self, specs: list[ParamSpec], values: dict[str, Any]) -> None:
        # Clear old
        while self._param_layout.rowCount():
            self._param_layout.removeRow(0)
        self._param_widgets.clear()

        for spec in specs:
            w: QWidget
            if spec.type == "int":
                sb = QSpinBox()
                if spec.min is not None:
                    sb.setMinimum(int(spec.min))
                if spec.max is not None:
                    sb.setMaximum(int(spec.max))
                if spec.step is not None:
                    sb.setSingleStep(int(spec.step))
                sb.setValue(int(values.get(spec.key, spec.default)))
                w = sb
            elif spec.type == "float":
                ds = QDoubleSpinBox()
                ds.setDecimals(4)
                if spec.min is not None:
                    ds.setMinimum(float(spec.min))
                if spec.max is not None:
                    ds.setMaximum(float(spec.max))
                if spec.step is not None:
                    ds.setSingleStep(float(spec.step))
                ds.setValue(float(values.get(spec.key, spec.default)))
                w = ds
            elif spec.type == "bool":
                cb = QCheckBox()
                cb.setChecked(bool(values.get(spec.key, spec.default)))
                w = cb
            elif spec.type == "choice":
                c = QComboBox()
                for ch in (spec.choices or []):
                    c.addItem(ch)
                val = str(values.get(spec.key, spec.default))
                idx = c.findText(val)
                if idx >= 0:
                    c.setCurrentIndex(idx)
                w = c
            else:
                w = QLabel("Unsupported")

            self._param_widgets[spec.key] = w
            self._param_layout.addRow(spec.label, w)

    def get_param_values(self) -> dict[str, Any]:
        out: dict[str, Any] = {}
        for k, w in self._param_widgets.items():
            if isinstance(w, QSpinBox):
                out[k] = w.value()
            elif isinstance(w, QDoubleSpinBox):
                out[k] = w.value()
            elif isinstance(w, QCheckBox):
                out[k] = w.isChecked()
            elif isinstance(w, QComboBox):
                out[k] = w.currentText()
        return out

    # ----------------- Local UI events -----------------
    def _on_op_selected(self) -> None:
        items = self._op_tree.selectedItems()
        if not items:
            return
        op_id = items[0].data(0, Qt.UserRole)
        if not op_id:
            return
        self._selected_op_id = str(op_id)
        self._events.on_select_operation(self._selected_op_id)

    # Convenience dialogs
    def ask_open_path(self) -> str | None:
        path, _ = QFileDialog.getOpenFileName(
            self,
            "Open image",
            "",
            "Images (*.png *.jpg *.jpeg *.bmp *.tif *.tiff);;All files (*)",
        )
        return path or None

    def ask_save_path(self) -> str | None:
        path, _ = QFileDialog.getSaveFileName(
            self,
            "Save image",
            "output.png",
            "PNG (*.png);;JPEG (*.jpg *.jpeg);;BMP (*.bmp);;All files (*)",
        )
        return path or None
