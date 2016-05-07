//
// Created by Alexandru on 4/9/2016.
//

#ifndef EDGE_BITMAPOPERATOR_H
#define EDGE_BITMAPOPERATOR_H

#include <jni.h>
#include <stdio.h>
#include <android/bitmap.h>
#include <unistd.h>

class BitmapOperator {
public:
    BitmapOperator();
    BitmapOperator(AndroidBitmapInfo * info);
    jint add(jint x, jint y);
    void rotate();
    void detectEdges();
    uint32_t* pixels;
    AndroidBitmapInfo bitmapInfo;

private:
    uint32_t getRValue(uint32_t pixel);
};


#endif //EDGE_BITMAPOPERATOR_H
