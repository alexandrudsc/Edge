//
// Created by Alexandru on 4/9/2016.
//

#ifndef EDGE_BITMAPOPERATOR_H
#define EDGE_BITMAPOPERATOR_H

#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <android/bitmap.h>
#include <unistd.h>

class BitmapOperator {
    public:
        BitmapOperator();
        uint32_t* rotate();
        uint32_t* zoom();
};


#endif //EDGE_BITMAPOPERATOR_H
