package com.developer.alexandru.edge;

import android.graphics.Bitmap;
import android.support.annotation.Nullable;

import java.nio.ByteBuffer;

/**
 * Created by Alexandru on 4/17/2016.
 */
public class BitmapOperator {

    static {
        System.loadLibrary("bitmaplib");
    }

    private ByteBuffer byteBuffer;

    private native int add(int x, int y);
    private native ByteBuffer init(Bitmap bitmap);
    private native void rotate(ByteBuffer buffer);
    private native Bitmap getBitmapFromMem(ByteBuffer buffer);
    private native void freeMem(ByteBuffer buffer);
    private native void detectEdges(ByteBuffer buffer);
    private native void blackAndWhite(ByteBuffer buffer);

    // store the bitmap into native memory and return the memory zone
    public void initBitmapOperator(Bitmap bitmap) {
        if (byteBuffer != null)
            freeBitmapMemory();
        byteBuffer = init(bitmap);
    }

    // create bitmap and free memory
    public Bitmap getBitmapAndFree()
    {
        final Bitmap bitmap = getBitmap();
        freeBitmapMemory();
        return bitmap;
    }

    public void rotate() {
        if (byteBuffer != null)
            rotate(byteBuffer);
    }

    public void detectEdges() {
        if (byteBuffer != null)
            detectEdges(byteBuffer);
    }

    public void blackAndWhite() {
        if (byteBuffer != null)
            blackAndWhite(byteBuffer);
    }

    @Nullable
    private Bitmap getBitmap()
    {
        if(byteBuffer == null)
            return null;
        return getBitmapFromMem(this.byteBuffer);
    }

    private void freeBitmapMemory() {
        if (byteBuffer == null)
            return;
        freeMem(byteBuffer);
    }
}
