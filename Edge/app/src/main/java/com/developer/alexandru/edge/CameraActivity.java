package com.developer.alexandru.edge;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.hardware.Camera;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.SurfaceHolder;
import android.view.TextureView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.ImageView;

import java.io.ByteArrayOutputStream;

/**
 * Created by Alexandru on 4/17/2016.
 */

public class CameraActivity extends AppCompatActivity implements Camera.PreviewCallback  {

    private SurfaceHolder surfaceHolder;
    private CameraSurface cameraSurface;
    private ImageView imageView;
    private Camera camera;
    TextureView textureView = null;

//    private CameraLayer mPreview;
//    private OpenGLLayer glView;

    BitmapOperator operator;

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

        setupCamera();

        operator = new BitmapOperator();
    }

    @Override
    protected void onResume() {
        super.onResume();
//        glView = new OpenGLLayer(this);
//        mPreview = new CameraLayer(this, glView);
//        glView.setWillNotDraw(false);
//
//        float mmInPx = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_MM, 20,
//                getResources().getDisplayMetrics());
//
//        ViewGroup.LayoutParams layoutParams = new ViewGroup.LayoutParams((int)mmInPx, (int)mmInPx);
//        glView.setLayoutParams(layoutParams);
//        setContentView(glView);
//        addContentView(mPreview, layoutParams);
    }

        private void setupCamera() {
        cameraSurface = (CameraSurface) findViewById(R.id.surfaceView);
        imageView = (ImageView) findViewById(R.id.imageView);
//        textureView = (TextureView) findViewById(R.id.textureView);

        try {
            camera = Camera.open();
        } catch (RuntimeException e) {
            System.err.println(e);
            return;
        }

        camera.setPreviewCallback(this);
        surfaceHolder = cameraSurface.getHolder();

        CameraHolder cameraHolder = new CameraHolder(this);
        cameraHolder.setCamera(camera);

        cameraSurface.setWillNotDraw(false);
        surfaceHolder.addCallback(cameraHolder);

        surfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
    }


    public void onPreviewFrame(byte[] data, Camera camera) {
        try
        {
            BitmapFactory.Options opts = new BitmapFactory.Options();
            // Convert to JPG
            Camera.Size previewSize = camera.getParameters().getPreviewSize();
            YuvImage yuvimage=new YuvImage(data, ImageFormat.NV21, previewSize.width, previewSize.height, null);
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            yuvimage.compressToJpeg(new Rect(0, 0, previewSize.width, previewSize.height), 80, baos);
            byte[] jdata = baos.toByteArray();

            // Convert to Bitmap
            Bitmap bitmap = BitmapFactory.decodeByteArray(jdata, 0, jdata.length, opts);

            operator = new BitmapOperator();
            operator.initBitmapOperator(bitmap);
            operator.rotate();
            bitmap = operator.getBitmapAndFree();

            imageView.setImageBitmap(bitmap);
        }
        catch(Exception e)
        {

        }
    }
}
