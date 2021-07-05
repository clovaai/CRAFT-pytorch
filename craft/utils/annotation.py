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
            ann["rect"][0] = np.min((y, ann["rect"][0])).astype(int)
            ann["rect"][1] = np.max((y, ann["rect"][1])).astype(int)
            ann["rect"][2] = np.min((x, ann["rect"][2])).astype(int)
            ann["rect"][3] = np.max((x, ann["rect"][3])).astype(int)
        anns.append(ann)

    return anns
