//
// Created by Alexandru on 4/9/2016.
//

#include "BitmapOperator.h"

BitmapOperator::BitmapOperator() {
}

BitmapOperator::BitmapOperator(AndroidBitmapInfo * info) {
    this->bitmapInfo = *info;
}

jint BitmapOperator::add(jint x, jint y) {
    return  x + y;
}

void BitmapOperator::rotate() {
    if (this->pixels == NULL)
        return;

    uint32_t* previousData = this->pixels;
    uint32_t newWidth = this->bitmapInfo.height;
    uint32_t newHeight = this->bitmapInfo.width;
    this->bitmapInfo.width = newWidth;
    this->bitmapInfo.height = newHeight;
    uint32_t* newBitmapPixels = new uint32_t[newWidth * newHeight];
    int ind_pixel = 0;

    this->pixels = newBitmapPixels;
    for (int x = newWidth - 1; x >= 0; --x)
        for (int y = 0; y < newHeight; ++y)
        {
            uint32_t pixel = previousData[ind_pixel++];
            newBitmapPixels[newWidth * y + x] = pixel;
        }
    delete[] previousData;

}

void BitmapOperator::blackAndWhite() {
    if (this->pixels == NULL)
        return;
    uint32_t newWidth = this->bitmapInfo.height;
    uint32_t newHeight = this->bitmapInfo.width;
    for (int x = 0; x < newWidth * newHeight; x++)
    {
        int a = getAValue(this->pixels[x]);
        int b = getBValue(this->pixels[x]);
        int r = getRValue(this->pixels[x]);
        int g = getGValue(this->pixels[x]);

        r = (r + b + g) /3;

        this->pixels[x] = createPixel(a, r, r, r);
    }
}

void BitmapOperator::detectEdges() {
    if (this->pixels == NULL)
        return;

    if (this->bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
        return;

    blackAndWhite();

    uint32_t* pixels = this->pixels;
    uint32_t width = this->bitmapInfo.width;
    uint32_t height = this->bitmapInfo.height;
    uint32_t* pixels_edge = new uint32_t[width * height];

    int ind_pixel = 0;
    int a, r, g, b, grey;
    int laplace;

    ind_pixel = width + 1;
    int sX, sY, s, sX_alpha, sY_alpha, s_alpha;
    for (int yy = 1; yy < height - 1; yy++) {
        for (int x = 1; x < width - 1; x++, ind_pixel++) {
            // 3x3 moving matrix
            uint32_t pixel_11 = pixels[ind_pixel - width - 1];
            uint32_t pixel_12 = pixels[ind_pixel - width];
            uint32_t pixel_13 = pixels[ind_pixel - width + 1];

            uint32_t pixel_21 = pixels[ind_pixel - 1];
            uint32_t pixel_22 = pixels[ind_pixel];
            uint32_t pixel_23 = pixels[ind_pixel + 1];

            uint32_t pixel_31 = pixels[ind_pixel + width - 1];
            uint32_t pixel_32 = pixels[ind_pixel + width];
            uint32_t pixel_33 = pixels[ind_pixel + width + 1];

            // sobel X axis
            sX = getRValue(pixel_33) - getRValue(pixel_31) +
                 getRValue(pixel_23) + getRValue(pixel_23) -
                 getRValue(pixel_21) - getRValue(pixel_21) +
                 getRValue(pixel_13) - getRValue(pixel_11);

            // sobel Y axis
            sY = getRValue(pixel_33) + getRValue(pixel_32) +
                 getRValue(pixel_32) + getRValue(pixel_31) -
                 getRValue(pixel_13) - getRValue(pixel_12) -
                 getRValue(pixel_12) - getRValue(pixel_11);

            s = sqrt(sX * sX + sY * sY);

            if (s < 0)
                s = 0;
            if (s > 255)
                s = 0xFF;

            pixels_edge[ind_pixel] = createPixel(0xFF, s, s, s);
        }
    }
    delete[] this->pixels;
    this->pixels = pixels_edge;
}

int BitmapOperator::getAValue(uint32_t pixel) {
    return (int)((pixel & 0xFF000000) >> 24);
}

int BitmapOperator::getRValue(uint32_t pixel) {
    return (int)((pixel & 0x00FF0000) >> 16);
}

int BitmapOperator::getGValue(uint32_t pixel) {
    return (int)((pixel & 0x0000FF00) >> 8);
}

int BitmapOperator::getBValue(uint32_t pixel) {
    return (int)((pixel & 0x000000FF));
}

uint32_t BitmapOperator::createPixel(int a, int r, int g, int b) {
    return (uint32_t) ((a << 24) |
                       (r << 16) |
                       (g << 8)  |
                       b);
}