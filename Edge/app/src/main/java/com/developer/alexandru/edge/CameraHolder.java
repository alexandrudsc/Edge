package com.developer.alexandru.edge;

import android.content.Context;
import android.hardware.Camera;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.ViewGroup;

import org.w3c.dom.DOMException;

import java.io.IOException;
import java.util.List;

/**
 * Created by Alexandru on 4/21/2016.
 */
public class CameraHolder extends ViewGroup implements SurfaceHolder.Callback {

        SurfaceView mSurfaceView;
        SurfaceHolder mHolder;
        Camera mCamera;
        CameraActivity activity;

        public CameraHolder(CameraActivity activity) {
            super(activity);

            this.activity = activity;

            mSurfaceView = new SurfaceView(activity);
            addView(mSurfaceView);

            // Install a SurfaceHolder.Callback so we get notified when the
            // underlying surface is created and destroyed.
            mHolder = mSurfaceView.getHolder();
            mHolder.addCallback(this);
            mHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        }

    public void setCamera(Camera camera) {
        if (mCamera == camera) { return; }

//        stopPreviewAndFreeCamera();

        mCamera = camera;
//        mCamera.setPreviewCallback(activity);

        if (mCamera != null) {
            List<Camera.Size> localSizes = mCamera.getParameters().getSupportedPreviewSizes();
            requestLayout();

            try {
                mCamera.setPreviewDisplay(mHolder);
            } catch (IOException e) {
                e.printStackTrace();
            }

            // Important: Call startPreview() to start updating the preview
            // surface. Preview must be started before you can take a picture.
            mCamera.startPreview();
        }
    }

    @Override
    protected void onLayout(boolean changed, int l, int t, int r, int b) {

    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        try {
            mCamera = Camera.open();
        }
        catch (RuntimeException e) {
            System.err.println(e);
            return;
        }

        Camera.Parameters param;
        param = mCamera.getParameters();
        param.setPreviewSize(352, 288);
        mCamera.setParameters(param);
        mCamera.setPreviewCallback(activity);

        try {
            mCamera.setPreviewDisplay(holder);
            mCamera.startPreview();
        }
        catch (Exception e) {
            System.err.println(e);
            return;
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int format, int width, int height) {
        if (surfaceHolder.getSurface() == null) {
            return;
        }

        try {
            mCamera.stopPreview();
        }

        catch (Exception e) {
        }

        try {
            mCamera.setPreviewDisplay(surfaceHolder);
            mCamera.setPreviewCallback(activity);
            mCamera.startPreview();
        }
        catch (Exception e) {
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        if (mCamera != null) {
            try {
                mCamera.stopPreview();
                mCamera.release();

            } catch (RuntimeException ex) {

            } finally {
                mCamera = null;
            }
        }
    }

}