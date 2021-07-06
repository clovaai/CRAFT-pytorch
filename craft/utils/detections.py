"""  
Copyright (c) 2019-present NAVER Corp.
MIT License
"""

# -*- coding: utf-8 -*-
import cv2
import copy
import math
import numpy as np

""" auxilary functions """
# unwarp corodinates


def warp_coord(Minv, pt):
    out = np.matmul(Minv, (pt[0], pt[1], 1))
    return np.array([out[0]/out[2], out[1]/out[2]])


""" end of auxilary functions """


def get_det_boxes_core(
        textmap, linkmap, text_threshold, link_threshold, low_text):
    # prepare data
    linkmap = linkmap.copy()
    textmap = textmap.copy()
    img_h, img_w = textmap.shape

    """ labeling method """
    ret, text_score = cv2.threshold(textmap, low_text, 1, 0)
    ret, link_score = cv2.threshold(linkmap, link_threshold, 1, 0)

    text_score_comb = np.clip(text_score + link_score, 0, 1)
    nLabels, labels, stats, centroids = cv2.connectedComponentsWithStats(
        text_score_comb.astype(np.uint8), connectivity=4)

    det = []
    mapper = []
    for k in range(1, nLabels):
        # size filtering
        size = stats[k, cv2.CC_STAT_AREA]
        if size < 10:
            continue

        # thresholding
        if np.max(textmap[labels == k]) < text_threshold:
            continue

        # make segmentation map
        segmap = np.zeros(textmap.shape, dtype=np.uint8)
        segmap[labels == k] = 255
        # remove link area
        segmap[np.logical_and(link_score == 1, text_score == 0)] = 0
        x, y = stats[k, cv2.CC_STAT_LEFT], stats[k, cv2.CC_STAT_TOP]
        w, h = stats[k, cv2.CC_STAT_WIDTH], stats[k, cv2.CC_STAT_HEIGHT]
        niter = int(math.sqrt(size * min(w, h) / (w * h)) * 2)
        sx, ex, sy, ey = x - niter, x + w + niter + 1, y - niter, y + h + niter + 1
        # boundary check
        if sx < 0:
            sx = 0
        if sy < 0:
            sy = 0
        if ex >= img_w:
            ex = img_w
        if ey >= img_h:
            ey = img_h
        kernel = cv2.getStructuringElement(
            cv2.MORPH_RECT, (1 + niter, 1 + niter))
        segmap[sy:ey, sx:ex] = cv2.dilate(segmap[sy:ey, sx:ex], kernel)

        # make box
        np_contours = np.roll(np.array(np.where(segmap != 0)),
                              1, axis=0).transpose().reshape(-1, 2)
        rectangle = cv2.minAreaRect(np_contours)
        box = cv2.boxPoints(rectangle)

        # align diamond-shape
        w, h = np.linalg.norm(box[0] - box[1]), np.linalg.norm(box[1] - box[2])
        box_ratio = max(w, h) / (min(w, h) + 1e-5)
        if abs(1 - box_ratio) <= 0.1:
            l, r = min(np_contours[:, 0]), max(np_contours[:, 0])
            t, b = min(np_contours[:, 1]), max(np_contours[:, 1])
            box = np.array([[l, t], [r, t], [r, b], [l, b]], dtype=np.float32)

        # make clock-wise order
        startidx = box.sum(axis=1).argmin()
        box = np.roll(box, 4-startidx, 0)
        box = np.array(box)

        det.append(box)
        mapper.append(k)

    return det, labels, mapper


def is_same_line(rect1, rect2):
    """
    Tries to estimate if two rectangles (rect1 and rect2) are at the
    same line based on rect1's height.
    """
    if all([rect1, rect2]):
        h1 = rect1[1] - rect1[0]
        return abs(rect1[0] - rect2[0]) <= h1 / 2

    return False


def poly_to_box(poly):
    """
    Convert any polygon to bounding box format (y0, y1, x0, x1).
    """
    x0, x1 = np.min(poly[:, 0]), np.max(poly[:, 0])
    y0, y1 = np.min(poly[:, 1]), np.max(poly[:, 1])
    box = [y0, y1, x0, x1]
    return box


def sort_detections(polys):
    """
    Sort detections left-right and top-down.
    """
    polys = copy.deepcopy(polys)
    lines_of_polys = []
    while len(polys):
        y_arr = []
        ref_box = None
        remove_indices = []
        lines_of_polys.append([])
        for i, poly in enumerate(polys):
            poly = np.array(poly)
            box = poly_to_box(poly)
            if not ref_box:
                ref_box = box
            if is_same_line(ref_box, box) or i == 0:
                x, y = np.mean(poly[:, 0]), np.mean(poly[:, 1])
                y_arr.append(y)
                remove_indices.append(i)
                lines_of_polys[-1].append((x, poly))

        polys = [p for j, p in enumerate(polys) if j not in remove_indices]
        # sort left-right
        lines_of_polys[-1] = (np.mean(y_arr), [p[1]
                                               for p in sorted(lines_of_polys[-1])])
    # sort top-down
    lines_of_polys = [p[1] for p in sorted(lines_of_polys)]

    polys = sum(lines_of_polys, [])
    return polys


def get_detections(textmap, linkmap, text_threshold, link_threshold, low_text):
    boxes, _, _ = get_det_boxes_core(
        textmap, linkmap, text_threshold, link_threshold, low_text)

    return boxes


def adjust_result_coordinates(polys, ratio_w, ratio_h, ratio_net=2):
    if len(polys) > 0:
        polys = np.array(polys)
        for k in range(len(polys)):
            if polys[k] is not None:
                polys[k] *= (ratio_w * ratio_net, ratio_h * ratio_net)

    return polys
