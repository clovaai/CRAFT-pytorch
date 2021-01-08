#include "rotated_bounding_box.hpp"

static void rotatingCalipers( const std::vector<Point> &points, int n, float* out )
{
    float minarea = 1e16;
    char buffer[32] = {};
    int i, k;
    std::vector<float> abuf(n*3);
    float* inv_vect_length = abuf.data();
    Point* vect = (Point*)(inv_vect_length + n);
    int left = 0, bottom = 0, right = 0, top = 0;
    int seq[4] = { -1, -1, -1, -1 };

    /* rotating calipers sides will always have coordinates
     (a,b) (-b,a) (-a,-b) (b, -a)
     */
    /* this is a first base vector (a,b) initialized by (1,0) */
    float orientation = 0;
    float base_a;
    float base_b = 0;

    float left_x, right_x, top_y, bottom_y;
    Point pt0 = points[0];

    left_x = right_x = pt0.x;
    top_y = bottom_y = pt0.y;

    for( i = 0; i < n; i++ )
    {
        double dx, dy;

        if( pt0.x < left_x )
            left_x = pt0.x, left = i;

        if( pt0.x > right_x )
            right_x = pt0.x, right = i;

        if( pt0.y > top_y )
            top_y = pt0.y, top = i;

        if( pt0.y < bottom_y )
            bottom_y = pt0.y, bottom = i;

        Point pt = points[(i+1) & (i+1 < n ? -1 : 0)];

        dx = pt.x - pt0.x;
        dy = pt.y - pt0.y;

        vect[i].x = (float)dx;
        vect[i].y = (float)dy;
        inv_vect_length[i] = (float)(1./std::sqrt(dx*dx + dy*dy));

        pt0 = pt;
    }

    // find convex hull orientation
    {
        double ax = vect[n-1].x;
        double ay = vect[n-1].y;

        for( i = 0; i < n; i++ )
        {
            double bx = vect[i].x;
            double by = vect[i].y;

            double convexity = ax * by - ay * bx;

            if( convexity != 0 )
            {
                orientation = (convexity > 0) ? 1.f : (-1.f);
                break;
            }
            ax = bx;
            ay = by;
        }
    }
    base_a = orientation;

    /*****************************************************************************************/
    /*                         init calipers position                                        */
    seq[0] = bottom;
    seq[1] = right;
    seq[2] = top;
    seq[3] = left;
    /*****************************************************************************************/
    /*                         Main loop - evaluate angles and rotate calipers               */

    /* all of edges will be checked while rotating calipers by 90 degrees */
    for( k = 0; k < n; k++ )
    {
        /* sinus of minimal angle */
        /*float sinus;*/

        /* compute cosine of angle between calipers side and polygon edge */
        /* dp - dot product */
        float dp[4] = {
            +base_a * vect[seq[0]].x + base_b * vect[seq[0]].y,
            -base_b * vect[seq[1]].x + base_a * vect[seq[1]].y,
            -base_a * vect[seq[2]].x - base_b * vect[seq[2]].y,
            +base_b * vect[seq[3]].x - base_a * vect[seq[3]].y,
        };

        float maxcos = dp[0] * inv_vect_length[seq[0]];

        /* number of calipers edges, that has minimal angle with edge */
        int main_element = 0;

        /* choose minimal angle */
        for ( i = 1; i < 4; ++i )
        {
            float cosalpha = dp[i] * inv_vect_length[seq[i]];
            if (cosalpha > maxcos)
            {
                main_element = i;
                maxcos = cosalpha;
            }
        }

        /*rotate calipers*/
        {
            //get next base
            int pindex = seq[main_element];
            float lead_x = vect[pindex].x*inv_vect_length[pindex];
            float lead_y = vect[pindex].y*inv_vect_length[pindex];
            switch( main_element )
            {
            case 0:
                base_a = lead_x;
                base_b = lead_y;
                break;
            case 1:
                base_a = lead_y;
                base_b = -lead_x;
                break;
            case 2:
                base_a = -lead_x;
                base_b = -lead_y;
                break;
            case 3:
                base_a = -lead_y;
                base_b = lead_x;
                break;
            default:
                break;
            }
        }
        /* change base point of main edge */
        seq[main_element] += 1;
        seq[main_element] = (seq[main_element] == n) ? 0 : seq[main_element];

        /* find area of rectangle */
        float height;
        float area;

        /* find vector left-right */
        float dx = points[seq[1]].x - points[seq[3]].x;
        float dy = points[seq[1]].y - points[seq[3]].y;

        /* dotproduct */
        float width = dx * base_a + dy * base_b;

        /* find vector left-right */
        dx = points[seq[2]].x - points[seq[0]].x;
        dy = points[seq[2]].y - points[seq[0]].y;

        /* dotproduct */
        height = -dx * base_b + dy * base_a;

        area = width * height;
        if( area <= minarea )
        {
            float *buf = (float *) buffer;

            minarea = area;
            /* leftist point */
            ((int *) buf)[0] = seq[3];
            buf[1] = base_a;
            buf[2] = width;
            buf[3] = base_b;
            buf[4] = height;
            /* bottom point */
            ((int *) buf)[5] = seq[0];
            buf[6] = area;
        }
    }                           /* for */

    {
        float *buf = (float *) buffer;

        float A1 = buf[1];
        float B1 = buf[3];

        float A2 = -buf[3];
        float B2 = buf[1];

        float C1 = A1 * points[((int *) buf)[0]].x + points[((int *) buf)[0]].y * B1;
        float C2 = A2 * points[((int *) buf)[5]].x + points[((int *) buf)[5]].y * B2;

        float idet = 1.f / (A1 * B2 - A2 * B1);

        float px = (C1 * B2 - C2 * B1) * idet;
        float py = (A1 * C2 - A2 * C1) * idet;

        out[0] = px;
        out[1] = py;

        out[2] = A1 * buf[2];
        out[3] = B1 * buf[2];

        out[4] = A2 * buf[4];
        out[5] = B2 * buf[4];
    }
}


RotatedBox rotatedBoxFromPoints(std::vector<Point> &pts) {
    Point out[3];
    RotatedBox box;

    convexHull(pts);

    int n = pts.size();

    if( n > 2 )
    {
        rotatingCalipers( pts, n, (float*)out );
        box.cx = out[0].x + (out[1].x + out[2].x)*0.5f;
        box.cy = out[0].y + (out[1].y + out[2].y)*0.5f;
        box.width = (float)std::sqrt((double)out[1].x*out[1].x + (double)out[1].y*out[1].y);
        box.height = (float)std::sqrt((double)out[2].x*out[2].x + (double)out[2].y*out[2].y);
        box.angle = (float)atan2( (double)out[1].y, (double)out[1].x );
    }
    else if( n == 2 )
    {
        box.cx = (pts[0].x + pts[1].x)*0.5f;
        box.cy = (pts[0].y + pts[1].y)*0.5f;
        double dx = pts[1].x - pts[0].x;
        double dy = pts[1].y - pts[0].y;
        box.width = (float)std::sqrt(dx*dx + dy*dy);
        box.height = 0;
        box.angle = (float)atan2( dy, dx );
    }
    else
    {
        if( n == 1 )
        {
            box.cx = pts[0].x;
            box.cy = pts[0].y;
        }
    }

    box.angle = (float)(box.angle*180/PI);
    return box;
}
