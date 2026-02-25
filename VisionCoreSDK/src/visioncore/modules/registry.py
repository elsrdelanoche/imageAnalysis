from __future__ import annotations

from visioncore.core.operation import Operation

from visioncore.modules.preprocessing.basic import (
    BrightnessContrast,
    Grayscale,
    HistogramEqualization,
    Negative,
)
from visioncore.modules.preprocessing.geometry import Rotate, Scale
from visioncore.modules.preprocessing.logical import BinaryThreshold
from visioncore.modules.preprocessing.morphology import Morphology
from visioncore.modules.frequency.fft import FFTSpectrum
from visioncore.modules.segmentation.edges import Canny, GaussianBlur, HighPass, SobelMagnitude
from visioncore.modules.features.corners import HarrisCorners


def all_operations() -> list[Operation]:
    # Instantiate (stateless) operations
    return [
        # Preprocessing
        Grayscale(),
        Negative(),
        BrightnessContrast(),
        HistogramEqualization(),
        BinaryThreshold(),
        Rotate(),
        Scale(),
        Morphology(),
        # Frequency
        FFTSpectrum(),
        # Segmentation & edges
        GaussianBlur(),
        HighPass(),
        SobelMagnitude(),
        Canny(),
        # Features
        HarrisCorners(),
    ]
