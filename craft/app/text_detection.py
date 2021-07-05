import cv2
import torch
from torch.autograd import Variable

# local import
from craft.model import CRAFT
from craft.utils import detections as det_utils
from craft.utils import annotation as ann_utils
from craft.utils.data import copy_state_dict
from craft.utils.imgproc import resize_aspect_ratio, normalize_mean_variance


class TextDetection:
    def __init__(self, model_path):
        self.model = CRAFT()
        self.model.load_state_dict(copy_state_dict(
            torch.load(model_path, map_location='cpu')))
        self.model.eval()

    def get(
        self, image, text_thresh=0.7, link_thresh=0.2,
        low_text=0.4, mag_ratio=1.5, canvas_size=1280
    ):
        if image is None:
            return []

        # resize
        img_resized, target_ratio, _ = resize_aspect_ratio(
            image, canvas_size, interpolation=cv2.INTER_LINEAR,
            mag_ratio=mag_ratio
        )
        ratio_h = ratio_w = 1 / target_ratio

        # preprocessing
        x = normalize_mean_variance(img_resized)
        x = torch.from_numpy(x).permute(2, 0, 1)  # hwc to chw
        x = Variable(x.unsqueeze(0))              # chw to bchw

        # forward pass
        with torch.no_grad():
            y, _ = self.model(x)

        # make score and link map
        score_text = y[0, :, :, 0].detach().numpy()
        score_link = y[0, :, :, 1].detach().numpy()

        # Post-processing
        detections = det_utils.get_detections(
            score_text, score_link, text_thresh,
            link_thresh, low_text
        )

        # coordinate adjustment
        detections = det_utils.adjust_result_coordinates(
            detections, ratio_w, ratio_h)

        # sort detections
        detections = det_utils.sort_detections(detections)

        # convert detections to annotations
        anns = ann_utils.convert_detection_to_annotation(detections)

        return anns


if __name__ == "__main__":
    model = TextDetection(
        '/home/guilherme/Documents/Code/Nuveo/npa/npa/models/obj_craft/craft_mlt_25k.pth')

    img = cv2.imread(
        '/home/guilherme/Documents/Code/Nuveo/CRAFT-pytorch/craft/images/test.png')
    anns = model.get(img)
    print(anns)
