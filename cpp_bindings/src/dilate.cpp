#include "dilate.hpp"


// https://github.com/opencv/opencv/blob/198b5096aaf8f5d855b98337e9de2fc45485c5a7/modules/ts/src/ts_func.cpp#L642
void dilate(const unsigned char *src, int srcW, int srcH,
        unsigned char *dst, int dstW, int dstH,
        const unsigned char *kernel, int kerW, int kerH,
        int roiLeft, int roiTop, int roiRight, int roiBottom)
{
    if (dstW < srcW || dstH < srcH)
        return;
    if (src == dst)
        return;

    int new_src_w = srcW + kerW/2 + kerW - kerW/2 - 1;
    unsigned char *new_src = copyMakeBorder(src, srcW, srcH, kerH/2, kerH - kerH/2 - 1,
               kerW/2, kerW - kerW/2 - 1);

    int *ofs = new int[kerW*kerH];
    int ofs_size = 0;
    for( int i = 0; i < kerH; i++ )
        for( int j = 0; j < kerW; j++ )
            if( kernel[i*kerW + j] != 0 ) {
                ofs[ofs_size++] = (i*new_src_w + j);
            }

    if( ofs_size == 0 )
        ofs[ofs_size++] = kerH/2*new_src_w + kerW;

    for( int y = roiTop; y < roiBottom; y++ )
    {
        const unsigned char *sptr = &new_src[y*new_src_w];
        unsigned char *dptr = &dst[y*dstW];

        for( int x = roiLeft; x < roiRight; x++ )
        {
            unsigned char result = sptr[x + ofs[0]];
            for( int i = 1; i < ofs_size; i++ ) {
                result = std::max(result, sptr[x + ofs[i]]);
            }
            dptr[x] = result;
        }
    }

    delete [] ofs;
    delete [] new_src;
}

unsigned char* copyMakeBorder(const unsigned char *src, int srcW, int srcH, int top, int bottom, int left, int right)
{
    unsigned char *dst = new unsigned char[(srcH + top + bottom) * (srcW + left + right)];
    int dstW = (srcW + left + right);
    int i, j, esz = sizeof(unsigned char);
    int width = srcW*esz;

    left *= esz;
    right *= esz;
    for( i = 0; i < srcH; i++ )
    {
        const unsigned char* sptr = &src[i*srcW];
        unsigned char* dptr = &dst[(i + top)*dstW + left];
        for( j = 0; j < left; j++ )
            dptr[j - left] = 0;
        if( dptr != sptr )
            for( j = 0; j < width; j++ )
                dptr[j] = sptr[j];
        for( j = 0; j < right; j++ )
            dptr[j + width] = 0;
    }

    for( i = 0; i < top; i++ )
    {
        unsigned char* dptr = &dst[i*dstW];
        for( j = 0; j < dstW; j++ )
            dptr[j] = 0;
    }

    for( i = 0; i < bottom; i++ )
    {
        unsigned char* dptr = &dst[(i + top + srcH)*dstW];
        for( j = 0; j < dstW; j++ )
            dptr[j] = 0;
    }

    return dst;
}
