import os
import cv2
import torch
import numpy as np
from cog import BasePredictor, Input, Path

import craft_utils
import imgproc
from craft import CRAFT


class Predictor(BasePredictor):
    def setup(self):
        # Load model once
        self.net = CRAFT()
        weight_path = "craft_mlt_25k.pth"

        # Download weights if not present
        if not os.path.exists(weight_path):
            import requests
            url = "https://github.com/clovaai/CRAFT-pytorch/releases/download/1.0/craft_mlt_25k.pth"
            r = requests.get(url)
            with open(weight_path, "wb") as f:
                f.write(r.content)

        self.net.load_state_dict(
            torch.load(weight_path, map_location="cpu")
        )
        self.net.eval()

    def predict(
        self,
        image: Path = Input(description="Input image"),
        text_threshold: float = Input(default=0.7, description="Text confidence threshold"),
        link_threshold: float = Input(default=0.4, description="Link confidence threshold"),
        low_text: float = Input(default=0.4, description="Low text threshold"),
    ) -> dict:
        # Load image
        img = imgproc.loadImage(str(image))

        # Run detection
        bboxes, polys, score_text = craft_utils.test_net(
            self.net,
            img,
            text_threshold=text_threshold,
            link_threshold=link_threshold,
            low_text=low_text,
            cuda=False,
        )

        # Draw boxes on image
        vis = img.copy()
        for box in bboxes:
            pts = np.array(box).astype(np.int32).reshape((-1, 1, 2))
            cv2.polylines(vis, [pts], True, (0, 255, 0), 2)

        # Save output image
        out_path = "/tmp/output.jpg"
        cv2.imwrite(out_path, vis)

        # Convert boxes to JSON serializable format
        boxes_json = [np.array(box).astype(float).tolist() for box in bboxes]

        return {
            "output_image": Path(out_path),
            "boxes": boxes_json,
        }
