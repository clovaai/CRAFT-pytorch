// Original code: https://github.com/opencv/opencv/blob/dd1494eebf205b0262c362bbdc57cef212464d1d/modules/imgproc/src/rotcalipers.cpp
#ifndef ROTATED_BOUNDING_BOX_H
#define ROTATED_BOUNDING_BOX_H

#include <cmath>
#include "convex_hull.hpp"
#define PI 3.14159265359

struct RotatedBox {
    float cx, cy, width, height, angle;
    int label;
    RotatedBox(float cx, float cy, float width, float height, float angle) {
        this->cx = cx;
        this->cy = cy;
        this->width = width;
        this->height = height;
        this->angle = angle;
    };
    RotatedBox() {
        cx = cy = width = height = angle = 0;
    };
};

RotatedBox rotatedBoxFromPoints(std::vector<Point> &pts);

#endif
