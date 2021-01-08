#ifndef FIND_COMPONENTS_H
#define FIND_COMPONENTS_H

#include <algorithm>
#include <math.h>
#include <cstring>
#include <vector>
#include <stdio.h>
#include "dilate.hpp"
#include "rotated_bounding_box.hpp"

// 50K will use ~782KB in RAM, good and safe enough
#define MAX_COMPONENTS 50000
// The following variables keep the current min and max coordinates
// on the following format: max_coord = xyxyxyxy....
extern int max_coord[MAX_COMPONENTS*2];
extern int min_coord[MAX_COMPONENTS*2];

extern float max_textmap[MAX_COMPONENTS];

typedef struct Box {
    int l, t, r, b, label;
    Box(int l, int t, int r, int b, int label) {
        this->l = l; this->t = t;
        this->r = r; this->b = b;
        this->label = label;
    };
} Box;

typedef struct Corner {
    float pts[8];
    int label;
    Corner(float *pts, int label) {
        for (int i=0; i<8; i++) {
            this->pts[i] = pts[i];
        }
        this->label = label;
    };
} Corner;

extern "C" {
    void findMinMaxBoxes(const int *markers, int width, int height, int lenOutBoxes, int* outBoxes);
    int findWordBoxes(const float *textmap, int textmapWidth, int textmapHeight, const int* labels,
        int nLabels, float *statsLeft, float *statsTop, float *statsWidth, float *statsHeight,
        float *statsArea, float textThreshold, int fastMode, int rotatedBox, float *outBoxes,
        int *outLabels);
    int getMaxNumComponents() { return MAX_COMPONENTS; };
}

void getRotationMatrix2D(float cx, float cy, float angle, float *outRot);
void matmul(float *mat1, int mat1Rows, int mat1Cols, float *mat2, int mat2Rows, int mat2Cols, float *outMat);

#endif
