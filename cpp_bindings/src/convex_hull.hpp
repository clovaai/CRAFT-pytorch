// Original code: https://cp-algorithms.com/geometry/grahams-scan-convex-hull.html
#ifndef CONVEX_HULL_H
#define CONVEX_HULL_H

#include <algorithm>
#include <vector>


struct Point {
    float x, y;
    Point(int x, int y) {
        this->x = x;
        this->y = y;
    };
    Point() {
        this->x = 0;
        this->y = 0;
    };
};

bool cmp(Point a, Point b);
bool cw(Point a, Point b, Point c);
bool ccw(Point a, Point b, Point c);

void convexHull(std::vector<Point>& a);

#endif
