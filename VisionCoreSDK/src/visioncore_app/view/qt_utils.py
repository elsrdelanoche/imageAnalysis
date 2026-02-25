from __future__ import annotations

import numpy as np
from PySide6.QtGui import QImage, QPixmap


def np_to_qpixmap(img: np.ndarray) -> QPixmap:
    """Convert OpenCV image (BGR or gray) to QPixmap."""
    if img.ndim == 2:
        h, w = img.shape
        qimg = QImage(img.data, w, h, w, QImage.Format_Grayscale8)
        return QPixmap.fromImage(qimg.copy())

    # BGR -> RGB
    h, w, ch = img.shape
    rgb = img[..., ::-1].copy(order="C")
    bytes_per_line = ch * w
    qimg = QImage(rgb.data, w, h, bytes_per_line, QImage.Format_RGB888)
    return QPixmap.fromImage(qimg.copy())
