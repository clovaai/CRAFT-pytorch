#include "convex_hull.hpp"


bool cmp(Point a, Point b) {
    return a.x < b.x || (a.x == b.x && a.y < b.y);
}

bool cw(Point a, Point b, Point c) {
    return a.x*(b.y-c.y)+b.x*(c.y-a.y)+c.x*(a.y-b.y) < 0;
}

bool ccw(Point a, Point b, Point c) {
    return a.x*(b.y-c.y)+b.x*(c.y-a.y)+c.x*(a.y-b.y) > 0;
}

void convexHull(std::vector<Point>& a) {
    if (a.size() == 1)
        return;

    sort(a.begin(), a.end(), &cmp);
    Point p1 = a[0], p2 = a.back();
    std::vector<Point> up, down;
    up.push_back(p1);
    down.push_back(p1);
    for (int i = 1; i < (int)a.size(); i++) {
        if (i == (int)a.size() - 1 || cw(p1, a.at(i), p2)) {
            while (up.size() >= 2 && !cw(up.at(up.size()-2), up.at(up.size()-1), a.at(i)))
                up.pop_back();
            up.push_back(a.at(i));
        }
        if (i == (int)a.size() - 1 || ccw(p1, a.at(i), p2)) {
            while(down.size() >= 2 && !ccw(down.at(down.size()-2), down.at(down.size()-1), a.at(i)))
                down.pop_back();
            down.push_back(a.at(i));
        }
    }

    a.clear();
    for (int i = 0; i < (int)up.size(); i++)
        a.push_back(up.at(i));
    for (int i = down.size() - 2; i > 0; i--)
        a.push_back(down.at(i));
}
