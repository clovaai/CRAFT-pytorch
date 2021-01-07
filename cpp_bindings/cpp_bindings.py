import ctypes
import numpy as np
from numpy.ctypeslib import ndpointer
import cv2

lib = ctypes.CDLL('cpp_bindings/build/libfast_boxes.so')

c_find_char_boxes = lib.findMinMaxBoxes
c_find_char_boxes.argtypes = [
    ndpointer(ctypes.c_int, flags='C_CONTIGUOUS'),
    ctypes.c_int,
    ctypes.c_int,
    ctypes.c_int,
    ndpointer(ctypes.c_int, flags='C_CONTIGUOUS'),
]


c_find_word_boxes = lib.findWordBoxes
c_find_word_boxes.argtypes = [
    ndpointer(ctypes.c_float, flags='C_CONTIGUOUS'),
    ctypes.c_int,
    ctypes.c_int,
    ndpointer(ctypes.c_int, flags='C_CONTIGUOUS'),
    ctypes.c_int,
    ndpointer(ctypes.c_float, flags='C_CONTIGUOUS'),
    ndpointer(ctypes.c_float, flags='C_CONTIGUOUS'),
    ndpointer(ctypes.c_float, flags='C_CONTIGUOUS'),
    ndpointer(ctypes.c_float, flags='C_CONTIGUOUS'),
    ndpointer(ctypes.c_float, flags='C_CONTIGUOUS'),
    ctypes.c_float,
    ctypes.c_int,
    ctypes.c_int,
    ndpointer(ctypes.c_float, flags='C_CONTIGUOUS'),
    ndpointer(ctypes.c_int, flags='C_CONTIGUOUS'),
]

c_get_max_num_components = lib.getMaxNumComponents


def find_char_boxes(markers, num_classes):
    # Allocate numpy buffer to store the results
    out_boxes = np.zeros((num_classes+1)*4, dtype=np.int32)
    c_find_char_boxes(markers, markers.shape[1], markers.shape[0],
            out_boxes.shape[0], out_boxes)

    char_boxes = []
    for idx in range(2,num_classes+1):
        l,t = out_boxes[idx*4+0], out_boxes[idx*4+1]
        r,b = out_boxes[idx*4+2], out_boxes[idx*4+3]
        w, h = r-l, b-t
        box = np.array([[l, t], [l + w, t], [l + w, t + h], [l, t + h]], dtype=np.float32)
        char_boxes.append(box)

    return char_boxes

def find_word_boxes(textmap, labels, nLabels, stats, text_threshold,
        fast_mode=False, rotated_box=True):
    """
    This function mostly mimics the hot loop from craft_utils.py . However it has two
    new parameters:
        fast_mode (bool): uses a faster post processing algorithm. The results should
            stay mostly the same.
        rotated_box (bool): indicates if the return boxes should be rotated boxes (the
            original) or a simple bounding box. The latter is much faster, however it
            does not works well for text on an angle.
    """
    # Allocate numpy buffer to store the results, 4 points and a label
    out_boxes = np.zeros(nLabels*8, dtype=np.float32)
    out_labels = np.zeros(nLabels, dtype=np.int32)

    stat_left   = np.array([stats[k,cv2.CC_STAT_LEFT]   for k in range(nLabels)], dtype=np.float32)
    stat_top    = np.array([stats[k,cv2.CC_STAT_TOP]    for k in range(nLabels)], dtype=np.float32)
    stat_width  = np.array([stats[k,cv2.CC_STAT_WIDTH]  for k in range(nLabels)], dtype=np.float32)
    stat_height = np.array([stats[k,cv2.CC_STAT_HEIGHT] for k in range(nLabels)], dtype=np.float32)
    stat_area   = np.array([stats[k,cv2.CC_STAT_AREA]   for k in range(nLabels)], dtype=np.float32)

    num_boxes = c_find_word_boxes(textmap, textmap.shape[1], textmap.shape[0],
            labels, nLabels, stat_left, stat_top, stat_width, stat_height,
            stat_area, text_threshold, int(fast_mode), int(rotated_box), out_boxes,
            out_labels)
    out_boxes = [np.array(out_boxes[x*8:x*8+8]).reshape(4,2) for x in range(num_boxes)]
    return out_boxes, out_labels
