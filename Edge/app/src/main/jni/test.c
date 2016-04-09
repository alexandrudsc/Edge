#include <jni.h>
#include "test.h"

JNIEXPORT jint JNICALL Java_com_developer_alexandru_edge_MainActivity_add (JNIEnv * env, jobject obj, jint value1, jint value2) {
    return add(value1, value2);
}