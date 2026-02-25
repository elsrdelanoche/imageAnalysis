from __future__ import annotations

from dataclasses import dataclass

from PySide6.QtCore import QObject, Signal
from PySide6.QtGui import QCursor
from PySide6.QtWidgets import QGraphicsPixmapItem, QGraphicsScene, QGraphicsView, QMenu

from .qt_utils import np_to_qpixmap


class CanvasSignals(QObject):
    artifact_selected = Signal(str)
    artifact_delete_requested = Signal(str)


@dataclass
class CanvasItem:
    artifact_id: str
    item: QGraphicsPixmapItem


class VisionCanvas(QGraphicsView):
    """Multi-image canvas with drag/select."""

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setScene(QGraphicsScene(self))
        self.setDragMode(QGraphicsView.RubberBandDrag)
        self.signals = CanvasSignals()
        self._items: dict[str, QGraphicsPixmapItem] = {}

    def add_artifact(self, artifact_id: str, img_np, name: str = "") -> None:
        pix = np_to_qpixmap(img_np)
        it = QGraphicsPixmapItem(pix)
        it.setFlag(QGraphicsPixmapItem.ItemIsMovable, True)
        it.setFlag(QGraphicsPixmapItem.ItemIsSelectable, True)
        it.setToolTip(name or artifact_id)
        self.scene().addItem(it)

        # Stagger placement a bit
        it.setPos(20 * (len(self._items) % 10), 20 * (len(self._items) % 10))

        self._items[artifact_id] = it

    def remove_artifact(self, artifact_id: str) -> None:
        it = self._items.pop(artifact_id, None)
        if it is None:
            return
        self.scene().removeItem(it)

    def set_active(self, artifact_id: str) -> None:
        for aid, it in self._items.items():
            it.setSelected(aid == artifact_id)

    def mousePressEvent(self, event):
        super().mousePressEvent(event)
        selected = self.scene().selectedItems()
        if selected:
            # Find which artifact id this is
            inv = {v: k for k, v in self._items.items()}
            aid = inv.get(selected[-1])
            if aid:
                self.signals.artifact_selected.emit(aid)

    def contextMenuEvent(self, event):
        selected = self.scene().selectedItems()
        if not selected:
            return
        inv = {v: k for k, v in self._items.items()}
        aid = inv.get(selected[-1])
        if not aid:
            return

        menu = QMenu(self)
        act_del = menu.addAction("Delete")
        chosen = menu.exec(QCursor.pos())
        if chosen == act_del:
            self.signals.artifact_delete_requested.emit(aid)
