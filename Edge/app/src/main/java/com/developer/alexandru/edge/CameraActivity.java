package com.developer.alexandru.edge;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.hardware.Camera;
import android.os.Bundle;
import android.renderscript.Allocation;
import android.renderscript.Element;
import android.renderscript.RenderScript;
import android.renderscript.ScriptIntrinsicYuvToRGB;
import android.renderscript.Type;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;

import java.io.ByteArrayOutputStream;

/**
 * Created by Alexandru on 4/17/2016.
 */

public class CameraActivity extends AppCompatActivity implements CameraHolder.PreviewCallback {

    private static final String TAG = "CameraActivity";

    private CameraHolder cameraSurface;
    private ImageView imageView;
    Bitmap bitmap;

    BitmapOperator operator;
    Allocation outData;
    Allocation inData;
    ScriptIntrinsicYuvToRGB scriptIntrinsicYuvToRGB;
    RenderScript renderScript;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera_activity);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });

        operator = new BitmapOperator();

        setupCamera();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (renderScript != null)
            renderScript.destroy();
    }

    private void setupCamera() {
        cameraSurface = (CameraHolder) findViewById(R.id.surfaceView);
        imageView = (ImageView) findViewById(R.id.imageView);
        cameraSurface.setPreviewCallback(this);
    }

    public void onPreviewFrame(byte[] data, Camera camera) {
        try {
            BitmapFactory.Options opts = new BitmapFactory.Options();
            Camera.Size previewSize = camera.getParameters().getPreviewSize();

            // Convert to Bitmap
            opts.inSampleSize = 4;

            YuvImage yuvimage = new YuvImage(data, ImageFormat.NV21, previewSize.width, previewSize.height, null);
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            yuvimage.compressToJpeg(new Rect(0, 0, previewSize.width, previewSize.height), 80, baos);
            byte[] jdata = baos.toByteArray();
            bitmap = BitmapFactory.decodeByteArray(jdata, 0, jdata.length, opts);

            operator = new BitmapOperator();
            operator.initBitmapOperator(bitmap);
            operator.detectEdges();
            bitmap = operator.getBitmapAndFree();

            imageView.setImageBitmap(bitmap);
        } catch (Exception e) {
            Log.e(TAG, e.getMessage());
        }
    }

    private void setupRenderScript(int width, int height) {
        renderScript = RenderScript.create(this);
        scriptIntrinsicYuvToRGB = ScriptIntrinsicYuvToRGB.create(renderScript, Element.RGBA_8888(renderScript));


        Type.Builder tbIn = new Type.Builder(renderScript, Element.U8(renderScript));
        tbIn.setX(width);
        tbIn.setY(height);
        Type.Builder tbOut = new Type.Builder(renderScript, Element.RGBA_8888(renderScript));
        tbOut.setX(width);
        tbOut.setY(height);

        inData = Allocation.createTyped(renderScript, tbIn.create(), Allocation.MipmapControl.MIPMAP_NONE,  Allocation.USAGE_SCRIPT & Allocation.USAGE_SHARED);
        outData = Allocation.createTyped(renderScript, tbOut.create(), Allocation.MipmapControl.MIPMAP_NONE,  Allocation.USAGE_SCRIPT & Allocation.USAGE_SHARED);

    }

    private static void decodeYUV420SP(int[] rgb, byte[] yuv420sp, int width, int height) {
        final int frameSize = width * height;

        for (int j = 0, yp = 0; j < height; j++) {
            int uvp = frameSize + (j >> 1) * width, u = 0, v = 0;
            for (int i = 0; i < width; i++, yp++) {
                int y = (0xff & ((int) yuv420sp[yp])) - 16;
                if (y < 0) y = 0;
                if ((i & 1) == 0) {
                    v = (0xff & yuv420sp[uvp++]) - 128;
                    u = (0xff & yuv420sp[uvp++]) - 128;
                }

                int y1192 = 1192 * y;
                int r = (y1192 + 1634 * v);
                int g = (y1192 - 833 * v - 400 * u);
                int b = (y1192 + 2066 * u);

                if (r < 0) r = 0; else if (r > 262143) r = 262143;
                if (g < 0) g = 0; else if (g > 262143) g = 262143;
                if (b < 0) b = 0; else if (b > 262143) b = 262143;

                rgb[yp] = 0xff000000 | ((r << 6) & 0xff0000) | ((g >> 2) & 0xff00) | ((b >> 10) & 0xff);
            }
        }
    }

}
