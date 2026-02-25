import numpy as np

from visioncore.pipeline.pipeline import ImagePipeline
from visioncore.modules.registry import all_operations


def test_ops_smoke(tmp_path):
    # create a synthetic image
    img = np.zeros((64, 64, 3), dtype=np.uint8)
    img[16:48, 16:48] = (0, 255, 0)

    import cv2

    p = tmp_path / "in.png"
    cv2.imwrite(str(p), img)

    pipe = ImagePipeline()
    pipe.load_image(str(p))

    ops = {op.id: op for op in all_operations()}

    # run a couple of operations
    pipe.execute(ops["pre.gray"], {"method": "BT.601"})
    pipe.execute(ops["seg.canny"], {"t_low": 10, "t_high": 30, "aperture": 3, "l2": False})

    assert pipe.active_id is not None
    assert len(pipe.artifacts) >= 3
