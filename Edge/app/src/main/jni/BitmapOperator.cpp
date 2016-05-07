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

    uint32_t* pixels = this->pixels;
    uint32_t* pixels2 = this->pixels;
    uint32_t width = this->bitmapInfo.width;
    uint32_t height = this->bitmapInfo.height;

    int whereToGet = 0;
    for (int y = height - 1; y >= height / 2; --y)
        for (int x = width - 1; x >= 0; --x)
        {
            //take from each row (up to bottom), from left to right
            uint32_t tempPixel = pixels2[width * y + x];
            pixels2[width * y + x] = pixels[whereToGet];
            pixels[whereToGet] = tempPixel;
            ++whereToGet;
        }

    //if the height isn't even, flip the middle row :
    if (height % 2 == 1)
    {
        int y = height / 2;
        whereToGet = width * y;
        int lastXToHandle = width % 2 == 0 ? (width / 2) : (width / 2) - 1;
        for (int x = width - 1; x >= lastXToHandle; --x)
        {
            uint32_t tempPixel = pixels2[width * y + x];
            pixels2[width * y + x] = pixels[whereToGet];
            pixels[whereToGet] = tempPixel;
            ++whereToGet;
        }
    }
}

void BitmapOperator::detectEdges() {
    if (this->pixels == NULL)
        return;

    if (this->bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
        return;

    uint32_t* pixels = this->pixels;
    uint32_t width = this->bitmapInfo.width;
    uint32_t height = this->bitmapInfo.height;

    // Laplace
    int ind_pixel = 0;
    int r, g, b;

    for (int yy = 1; yy < height; yy++) {
        int y = yy - 1;
        for (int x = 1; x < width; x++, ind_pixel++) {
            b = (int) ((pixels[ind_pixel] & 0x00FF0000) >> 16);
            g = (int)((pixels[ind_pixel] & 0x0000FF00) >> 8);
            r = (int) (pixels[ind_pixel] & 0x000000FF);
            pixels[ind_pixel] |= 0x00FF0000;
        }
    }
}

uint32_t  BitmapOperator::getRValue(uint32_t pixel) {
    return pixel & 0xFF0000;
}