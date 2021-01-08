#include "find_components.hpp"

int max_coord[MAX_COMPONENTS*2];
int min_coord[MAX_COMPONENTS*2];

float max_textmap[MAX_COMPONENTS];

inline int getIndex(int width, int y, int x) {
    return y*width+x;
}

void matmul(float *mat1, int mat1Rows, int mat1Cols, float *mat2, int mat2Rows, int mat2Cols, float *outMat) {
    for (int i=0; i<mat1Rows*mat2Cols; i++) {
        outMat[i] = 0;
    }

    for (int i=0; i<mat1Rows; i++) {
        for (int j=0; j<mat2Cols; j++) {
            for (int k=0; k<mat1Cols; k++) {
                outMat[i*mat2Cols+j] += mat1[i*mat1Cols+k] * mat2[k*mat2Cols+j];
            }
        }
    }
}

void getRotationMatrix2D(float cx, float cy, float angle, float *outRot) {
    // Code from: https://github.com/opencv/opencv/blob/624d532000d726734637e6d51c8e40785f45b46b/modules/imgproc/src/imgwarp.cpp#L3238
    angle *= PI/180;
    float alpha = cos(angle);
    float beta = sin(angle);

    outRot[0] = alpha;
    outRot[1] = beta;
    outRot[2] = (1-alpha)*cx - beta*cy;
    outRot[3] = -beta;
    outRot[4] = alpha;
    outRot[5] = beta*cx + (1-alpha)*cy;
}

int findWordBoxes(const float *textmap, int textmapWidth, int textmapHeight, const int* labels,
        int nLabels, float *statsLeft, float *statsTop, float *statsWidth, float *statsHeight,
        float *statsArea, float textThreshold, int fastMode, int rotatedBox, float *outBoxes,
        int *outLabels)
{
    // Precompute textmap max value for each label
    memset(&max_textmap, 0, sizeof(int)*MAX_COMPONENTS);
    for (int i=0; i<textmapHeight; i++) {
        for (int j=0; j<textmapWidth; j++) {
            int idx = getIndex(textmapWidth, i, j);
            int label = labels[idx];
            if (max_textmap[label] < textmap[idx]) {
                max_textmap[label] = textmap[idx];
            }
        }
    }

    std::vector<unsigned char> segmap_vec(textmapHeight*textmapWidth);
    std::vector<unsigned char> dilate_dst_vec(textmapHeight*textmapWidth);
    unsigned char *segmap = segmap_vec.data();
    unsigned char *dilate_dst = dilate_dst_vec.data();

    memset(segmap, 0, sizeof(unsigned char)*textmapWidth*textmapHeight);
    memset(dilate_dst, 0, sizeof(unsigned char)*textmapWidth*textmapHeight);

    // findMinMaxBoxes will update our max_coord and min_coord variables
    // with the min and max position of each label
    findMinMaxBoxes(labels, textmapWidth, textmapHeight, 0, NULL);

    std::vector<float> rot_vec(3*2);
    std::vector<float> corners_vec(2*4);
    std::vector<float> pts_array_vec(3*4);
    float *rot = rot_vec.data();
    float *corners = corners_vec.data();
    // Each point is a column.
    float *pts_array = pts_array_vec.data();
    pts_array[8] = pts_array[9] = pts_array[10] = pts_array[11] = 1;
    std::vector<Corner> corners_list;
    for (int k=1; k<nLabels; k++) {
        if (statsArea[k] < 10) continue;

        if (max_textmap[k] < textThreshold) continue;

        int x=statsLeft[k],  y=statsTop[k];
        int w=statsWidth[k], h=statsHeight[k];
        int niter = sqrt(statsArea[k] * std::min(w,h) / (w*h)) * 2;
        int sx = std::max(x-niter,0), ex = std::min(x+w+niter+1, textmapWidth);
        int sy = std::max(y-niter,0), ey = std::min(y+h+niter+1, textmapHeight);
        RotatedBox rot_box;

        if (rotatedBox) {
            std::vector<Point> pts;
            if (fastMode == false) { // original implementation
                for (int i=min_coord[k*2+1]; i<=max_coord[k*2+1]; i++) {
                    for (int j=min_coord[k*2+0]; j<=max_coord[k*2+0]; j++) {
                        int idx = getIndex(textmapWidth, i, j);
                        if (labels[idx] == k) {
                            segmap_vec.at(idx) = 255;
                        }
                    }
                }
                unsigned char *kernel = new unsigned char[(1+niter)*(1+niter)];
                for (int i=0; i<(1+niter)*(1+niter); i++) {
                    kernel[i] = 255;
                }

                dilate(segmap, textmapWidth, textmapHeight, dilate_dst, textmapWidth, textmapHeight,
                        kernel, (1+niter), (1+niter), sx, sy, ex, ey);

                for (int i=sy; i<ey; i++) {
                    for (int j=sx; j<ex; j++) {
                        int idx = getIndex(textmapWidth, i, j);
                        if (dilate_dst_vec.at(idx) > 0) {
                            pts.push_back(Point(j,i));
                        }
                        // Erase what was written to segmap and dilate_dst
                        dilate_dst_vec.at(idx) = segmap_vec.at(idx) = 0;
                    }
                }
                rot_box = rotatedBoxFromPoints(pts);
                delete [] kernel;
            } else { // fastMode: ignore dilate
                for (int i=min_coord[k*2+1]; i<=max_coord[k*2+1]; i++) {
                    for (int j=min_coord[k*2+0]; j<=max_coord[k*2+0]; j++) {
                        int idx = getIndex(textmapWidth, i, j);
                        if (labels[idx] == k) {
                            segmap_vec.at(idx) = 255;
                            pts.push_back(Point(j,i));
                        }
                    }
                }
                rot_box = rotatedBoxFromPoints(pts);
            }
        } else { // Not a rotated box
            rot_box.cx = (sx+ex)/2;
            rot_box.cy = (sy+ey)/2;
            rot_box.width  = ex-sx;
            rot_box.height = ey-sy;
            rot_box.angle = 0;
        }

        if (fastMode == true || rotatedBox == false) {
            // If we didn't do the dilate, the box should be
            // increased to compensate it.
            rot_box.width  += niter;
            rot_box.height += niter;
        }

        getRotationMatrix2D(rot_box.cx, rot_box.cy, -rot_box.angle, rot);
        float l = rot_box.cx-rot_box.width/2;
        float t = rot_box.cy-rot_box.height/2;
        float r = rot_box.cx+rot_box.width/2;
        float b = rot_box.cy+rot_box.height/2;
        pts_array[0] = l; pts_array[4] = t;
        pts_array[1] = r; pts_array[5] = t;
        pts_array[2] = r; pts_array[6] = b;
        pts_array[3] = l; pts_array[7] = b;

        matmul(rot, 2, 3, pts_array, 3, 4, corners);
        corners_list.push_back(Corner(corners, k));
    }
    for (int i=0; i<(int)corners_list.size(); i++) {
        for (int j=0; j<4; j++) {
            outBoxes[i*8+j*2+0] = corners_list.at(i).pts[0*4+j];
            outBoxes[i*8+j*2+1] = corners_list.at(i).pts[1*4+j];
        }
        outLabels[i] = corners_list.at(i).label;
    }

    return corners_list.size();
}

void findMinMaxBoxes(const int *markers, int width, int height, int lenOutBoxes, int* outBoxes) {
    memset(&max_coord, 0, sizeof(int)*MAX_COMPONENTS*2);
    // The following memset is not seting min_coord to 1!
    memset(&min_coord, 1, sizeof(int)*MAX_COMPONENTS*2);

    int max_idx = 0;
    for (int i=0; i<height; i++) {
        int row = i*width;
        for (int j=0; j<width; j++) {
            int component_id = markers[row+j]*2;
            if (markers[row+j] > max_idx) {
                max_idx = markers[row+j];
            }
            if (max_idx > MAX_COMPONENTS) {
                // THIS IS BAAAAD!!! Stoping to avoid stack corruption
                goto END_FOR;
            }
            if (j < min_coord[component_id]) {
                min_coord[component_id] = j;
            }
            if (j > max_coord[component_id]) {
                max_coord[component_id] = j;
            }
            if (i < min_coord[component_id+1]) {
                min_coord[component_id+1] = i;
            }
            if (i > max_coord[component_id+1]) {
                max_coord[component_id+1] = i;
            }
        }
    }
END_FOR:

    for (int i=0; i<=max_idx; i++) {
        if ((i*4+3) > lenOutBoxes) {
            break;
        }
        outBoxes[i*4+0] = min_coord[i*2];
        outBoxes[i*4+1] = min_coord[i*2+1];
        outBoxes[i*4+2] = max_coord[i*2]+1;
        outBoxes[i*4+3] = max_coord[i*2+1]+1;
    }
}
