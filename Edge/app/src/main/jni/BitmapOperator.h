//
// Created by Alexandru on 4/9/2016.
//

#ifndef EDGE_BITMAPOPERATOR_H
#define EDGE_BITMAPOPERATOR_H

#include <jni.h>
#include <stdio.h>
#include <android/bitmap.h>
#include <unistd.h>
#include <cmath>

class BitmapOperator {
public:
    BitmapOperator();
    BitmapOperator(AndroidBitmapInfo * info);
    jint add(jint x, jint y);
    void rotate();
    void detectEdges();
    void blackAndWhite();
    uint32_t* pixels;
    AndroidBitmapInfo bitmapInfo;

private:
    int getAValue(uint32_t pixel);          // alpha
    int getRValue(uint32_t pixel);          // red
    int getGValue(uint32_t pixel);          // green
    int getBValue(uint32_t pixel);          // blue
    uint32_t createPixel(int a, int r, int g, int b);
};


#endif //EDGE_BITMAPOPERATOR_H
