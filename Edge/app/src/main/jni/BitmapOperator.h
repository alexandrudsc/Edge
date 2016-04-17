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

    uint32_t* pixels;
    AndroidBitmapInfo bitmapInfo;

};


#endif //EDGE_BITMAPOPERATOR_H
