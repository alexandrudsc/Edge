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


    int ind_pixel = 0;
    int r, g, b, grey;
    int laplace;

    for (int yy = 1; yy < height; yy++) {
        int y = yy - 1;
        for (int x = 1; x < width; x++, ind_pixel++) {
            uint32_t pixel = pixels[ind_pixel];

            r = getRValue(pixel);
            g = getGValue(pixel);
            b = getBValue(pixel);
            grey = (r + g + b) / 3;
            pixels[ind_pixel] = createPixel(getAValue(pixel), r, r, r);
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
    return (uint32_t) (((a << 24) & 0xFF000000) +
                               ((r << 16)  & 0x00FF0000) +
                               ((g << 8) & 0x0000FF00) +
                               (b & 0x000000FF));
}