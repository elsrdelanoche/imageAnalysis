class VisionCoreError(Exception):
    """Base exception for VisionCore."""


class ImageNotLoadedError(VisionCoreError):
    pass


class OperationError(VisionCoreError):
    pass
