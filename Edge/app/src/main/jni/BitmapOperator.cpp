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
            //take from each row (up to bottom), from left to right
            uint32_t pixel = previousData[ind_pixel++];
            newBitmapPixels[newWidth * y + x] = pixel;
        }
    delete[] previousData;

}

void BitmapOperator::detectEdges() {
    if (this->pixels == NULL)
        return;

    if (this->bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
        return;

    uint32_t* pixels = this->pixels;
    uint32_t width = this->bitmapInfo.width;
    uint32_t height = this->bitmapInfo.height;


    int ind_pixel = 0;
    int a, r, g, b, grey;
    int laplace;

    ind_pixel = width + 1;
    int sX, sY, s;
    for (int yy = 1; yy < height - 1; yy++, ind_pixel += 2) {
        for (int x = 1; x < width - 1; x++, ind_pixel++) {
            // matricea glisanta de 3x3
            uint32_t pixel_11 = pixels[ind_pixel - width - 1];
            uint32_t pixel_12 = pixels[ind_pixel - width];
            uint32_t pixel_13 = pixels[ind_pixel - width + 1];

            uint32_t pixel_21 = pixels[ind_pixel - 1];
            uint32_t pixel_22 = pixels[ind_pixel];
            uint32_t pixel_23 = pixels[ind_pixel + 1];

            uint32_t pixel_31 = pixels[ind_pixel + width - 1];
            uint32_t pixel_32 = pixels[ind_pixel + width];
            uint32_t pixel_33 = pixels[ind_pixel + width + 1];

            sX = pixels[ind_pixel + width + 1] - pixels[ind_pixel + width - 1] +
                pixels[ind_pixel + 1] + pixels[ind_pixel + 1] -
                pixels[ind_pixel - 1] - pixels[ind_pixel - 1] +
                pixels[ind_pixel - width + 1] - pixels[ind_pixel - width - 1];

            sY = pixels[ind_pixel + width + 1] + pixels[ind_pixel + width] +
                pixels[ind_pixel + width] + pixels[ind_pixel + width - 1] -
                pixels[ind_pixel - width + 1] - pixels[ind_pixel - width] -
                pixels[ind_pixel - width] - pixels[ind_pixel - width - 1];

            sX = pixel_31 + 2 * pixel_32 + pixel_33 -
                    pixel_11 - 2 * pixel_12 - pixel_13;

            sY = pixel_13 + 2 * pixel_23 + pixel_33 -
                    pixel_11 - 2 * pixel_21 - pixel_31;

            s = (sX + sY) / 2;
            a = getAValue(pixels[ind_pixel]);
            if (s < 50)
                s = 0;
            else
                s = 0xFF;
            pixels[ind_pixel] = createPixel(a, s, s, s);
        }
    }
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