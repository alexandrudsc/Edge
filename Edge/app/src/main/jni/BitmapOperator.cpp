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