#include <jni.h>
#include <android/bitmap.h>
#include "BitmapOperator.h"


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
        if (bitmapOperator->pixels == NULL)
            return;

        uint32_t* pixels = bitmapOperator->pixels;
        uint32_t* pixels2 = bitmapOperator->pixels;
        uint32_t width = bitmapOperator->bitmapInfo.width;
        uint32_t height = bitmapOperator->bitmapInfo.height;

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
};


