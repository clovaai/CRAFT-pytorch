#ifndef DILATE_H
#define DILATE_H

#include <algorithm>
#include <cmath>

// WARNING (gfickel): src and dst cannot be the same on dilate!
void dilate(const unsigned char *src, int srcW, int srcH,
        unsigned char *dst, int dstW, int dstH,
        const unsigned char *kernel, int kerW, int kerH,
        int roiLeft, int roiTop, int roiRight, int roiBottom);

unsigned char* copyMakeBorder(const unsigned char *src, int srcW,
        int srcH, int top, int bottom, int left, int right);

#endif
