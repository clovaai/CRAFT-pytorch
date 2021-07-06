import numpy as np


def convert_detection_to_annotation(detections):
    """
    Convert the CRAFT output to the standard annotation format
    used by the Nuveo's OCR tools.
    """
    anns = []
    for det in detections:
        ann = {
            "rect": [np.inf, 0, np.inf, 0],
            "vertices": []
        }
        for (x, y) in det:
            ann["vertices"].append({"x": int(x), "y": int(y)})
            ann["rect"][0] = int(np.min((y, ann["rect"][0])))
            ann["rect"][1] = int(np.max((y, ann["rect"][1])))
            ann["rect"][2] = int(np.min((x, ann["rect"][2])))
            ann["rect"][3] = int(np.max((x, ann["rect"][3])))
        anns.append(ann)

    return anns
