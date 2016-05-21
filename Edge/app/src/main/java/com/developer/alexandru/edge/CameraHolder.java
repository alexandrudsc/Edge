package com.developer.alexandru.edge;

import android.content.Context;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.ViewGroup;

import java.io.IOException;
import java.util.List;

/**
 * Created by Alexandru on 4/21/2016.
 */
public class CameraHolder extends SurfaceView implements SurfaceHolder.Callback {
    private static final String TAG = "CAMERA HOLDER";

    private SurfaceHolder mHolder;
    private Camera mCamera;

    private CameraHolder.PreviewCallback callback;

    public CameraHolder(Context context, AttributeSet attrs) {
        super(context, attrs);
        try {
            mCamera = Camera.open();
            // Install a SurfaceHolder.Callback so we get notified when the
            // underlying surface is created and destroyed.
            mHolder = getHolder();
            mHolder.addCallback(this);
            // deprecated setting, but required on Android versions prior to 3.0
            mHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        } catch (RuntimeException ex)
        {
            Log.d(TAG, ex.toString());
        }
    }

    public void surfaceCreated(SurfaceHolder holder) {
        // The Surface has been created, now tell the camera where to draw the preview.
        if (holder == null || mCamera == null)
            return;
        try {
            mCamera.setPreviewDisplay(holder);
            mCamera.startPreview();
        } catch (IOException e) {
            Log.d(TAG, "Error setting camera preview: " + e.getMessage());
        }
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        if (holder == null || mCamera == null)
            return;
        try {
            mCamera.setPreviewCallback(null);
            mCamera.release();
            mCamera = null;
        } catch (RuntimeException ex)
        {
            Log.d(TAG, ex.toString());
        }
    }

    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        // If your preview can change or rotate, take care of those events here.
        // Make sure to stop the preview before resizing or reformatting it.
        if (mCamera == null || mHolder.getSurface() == null){
            // preview surface does not exist
            return;
        }

        // stop preview before making changes
        try {
            mCamera.stopPreview();
        } catch (Exception e){
            // ignore: tried to stop a non-existent preview
        }

        // set preview size and make any resize, rotate or
        // reformatting changes her
//        mCamera.setDisplayOrientation(90);
        Camera.Parameters cameraParameters = mCamera.getParameters();
        cameraParameters.setPreviewFormat(ImageFormat.NV21);

        mCamera.setParameters(cameraParameters);

        // start preview with new settings
        try {
            mCamera.setPreviewDisplay(mHolder);
            mCamera.setPreviewCallback(callback);

            mCamera.startPreview();

        } catch (Exception e){
            Log.d(TAG, "Error starting camera preview: " + e.getMessage());
        }
    }

    @Override
    protected void onLayout(boolean changed, int l, int t, int r, int b) {

    }

    public void setPreviewCallback(PreviewCallback callback) {
        this.callback = callback;
        if (this.mCamera != null) {
            mCamera.setPreviewCallback(this.callback);
            mCamera.startPreview();
        }
        else
            Log.d(TAG, "Camera was null");
    }

    public interface PreviewCallback extends Camera.PreviewCallback{
        void onPreviewFrame(byte[] data, Camera camera);
    }
}