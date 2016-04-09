#include <jni.h>

JNIEXPORT jint JNICALL Java_com_developer_alexandru_edge_MainActivity_add (JNIEnv * env, jobject obj, jint value1, jint value2) {
    return value1 + value2;
}