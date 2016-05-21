#include "BitmapOperator.h"
#include "bitmaplib.h"

extern "C"
{

    JNIEXPORT jint JNICALL
    Java_com_developer_alexandru_edge_BitmapOperator_add(JNIEnv *env, jobject obj, jint value1, jint value2) {
        BitmapOperator op;
        return op.add(value1, value2);
    }

    /**store java bitmap as JNI data*/
    JNIEXPORT jobject JNICALL
    Java_com_developer_alexandru_edge_BitmapOperator_init(JNIEnv * env, jobject obj, jobject bitmap) {
        AndroidBitmapInfo bitmapInfo;
        uint32_t* storedBitmapPixels = NULL;

        int ret;
        if ((ret = AndroidBitmap_getInfo(env, bitmap, &bitmapInfo)) < 0)
        {
            return NULL;
        }

        if (bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
        {
            return NULL;
        }

        void* bitmapPixels;
        if ((ret = AndroidBitmap_lockPixels(env, bitmap, &bitmapPixels)) < 0)
        {
            return NULL;
        }
        uint32_t* src = (uint32_t*) bitmapPixels;
        storedBitmapPixels = new uint32_t[bitmapInfo.height * bitmapInfo.width];
        int pixelsCount = bitmapInfo.height * bitmapInfo.width;
        memcpy(storedBitmapPixels, src, sizeof(uint32_t) * pixelsCount);
        AndroidBitmap_unlockPixels(env, bitmap);

        BitmapOperator* bitmapOperator = new BitmapOperator();
        bitmapOperator->bitmapInfo = bitmapInfo;
        bitmapOperator->pixels = storedBitmapPixels;

        // allocate native memory
        return env->NewDirectByteBuffer(bitmapOperator, 0);
    }

    // restore java bitmap (from native bytes)
    JNIEXPORT jobject JNICALL
    Java_com_developer_alexandru_edge_BitmapOperator_getBitmapFromMem(JNIEnv * env, jobject obj, jobject buffer) {
        BitmapOperator* bitmapOperator = (BitmapOperator*) env->GetDirectBufferAddress(buffer);
        if (bitmapOperator->pixels == NULL)
        {
            return NULL;
        }

        // create new bitmap Bitmap Bitmap.createBitmap (int width, int height, Bitmap.Config config)
        jclass bitmapCls = env->FindClass("android/graphics/Bitmap");
        jmethodID createBitmapFunction = env->GetStaticMethodID(bitmapCls,
                                                                "createBitmap",
                                                                "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
        jstring configName = env->NewStringUTF("ARGB_8888");
        jclass bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");
        jmethodID valueOfBitmapConfigFunction = env->GetStaticMethodID(
                bitmapConfigClass, "valueOf",
                "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");
        jobject bitmapConfig = env->CallStaticObjectMethod(bitmapConfigClass,
                                                           valueOfBitmapConfigFunction, configName);
        jobject newBitmap = env->CallStaticObjectMethod(bitmapCls,
                                                        createBitmapFunction, bitmapOperator->bitmapInfo.width,
                                                        bitmapOperator->bitmapInfo.height, bitmapConfig);
        // put pixels into the new bitmap:
        int ret;
        void* bitmapPixels;
        if ((ret = AndroidBitmap_lockPixels(env, newBitmap, &bitmapPixels)) < 0)
        {
            return NULL;
        }
        uint32_t* newBitmapPixels = (uint32_t*) bitmapPixels;
        int pixelsCount = bitmapOperator->bitmapInfo.height * bitmapOperator->bitmapInfo.width;

        memcpy(newBitmapPixels, bitmapOperator->pixels, sizeof(uint32_t) * pixelsCount);
        AndroidBitmap_unlockPixels(env, newBitmap);

        return newBitmap;
    }

    // free bitmap
    JNIEXPORT void JNICALL
    Java_com_developer_alexandru_edge_BitmapOperator_freeMem (JNIEnv * env, jobject obj, jobject buffer) {
        BitmapOperator* bitmapOperator = (BitmapOperator*)env->GetDirectBufferAddress(buffer);

        if (bitmapOperator->pixels != NULL) {
            delete [ ] bitmapOperator->pixels;
            bitmapOperator -> pixels = NULL;
            delete bitmapOperator;
        }
    }

    // rotate bitmap 180 degrees
    JNIEXPORT void JNICALL
    Java_com_developer_alexandru_edge_BitmapOperator_rotate(JNIEnv * env, jobject obj, jobject buffer) {
        BitmapOperator* bitmapOperator = (BitmapOperator*) env->GetDirectBufferAddress(buffer);
        bitmapOperator->rotate();
    }

    // detect edges
    JNIEXPORT void JNICALL
    Java_com_developer_alexandru_edge_BitmapOperator_detectEdges(JNIEnv * env, jobject obj, jobject buffer) {
        BitmapOperator* bitmapOperator = (BitmapOperator*) env->GetDirectBufferAddress(buffer);
        bitmapOperator->detectEdges();
    }

    // greyscale effect
    JNIEXPORT void JNICALL
    Java_com_developer_alexandru_edge_BitmapOperator_blackAndWhite(JNIEnv * env, jobject obj, jobject buffer) {
        BitmapOperator* bitmapOperator = (BitmapOperator*) env->GetDirectBufferAddress(buffer);
        bitmapOperator->blackAndWhite();
    }
};


