package com.developer.alexandru.edge;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.graphics.PixelFormat;
import android.hardware.Camera;
import android.media.Image;
import android.opengl.GLSurfaceView;
import android.opengl.GLU;
import android.opengl.GLSurfaceView.Renderer;
import android.view.SurfaceHolder;

/**
 * This class uses OpenGL ES to render the camera's viewfinder image on the
 * screen. Unfortunately I don't know much about OpenGL (ES). The code is mostly
 * copied from some examples. The only interesting stuff happens in the main
 * loop (the run method) and the onPreviewFrame method.
 */
public class OpenGLLayer extends GLSurfaceView implements SurfaceHolder.Callback,
		Camera.PreviewCallback, Renderer {

	int onDrawFrameCounter = 1;
	int[] cameraTexture;
	byte[] glCameraFrame = new byte[256*256]; //size of a texture must be a power of 2
	FloatBuffer cubeBuff;
	FloatBuffer texBuff;

	ImageProcessor imageProcessor;

	public OpenGLLayer(Context c) {
		super(c);

//        super.setEGLConfigChooser(5, 6, 5, 8, 16, 0);
		this.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        this.setRenderer(this);
        this.getHolder().setFormat(PixelFormat.TRANSLUCENT);
	}

	public void onDrawFrame(GL10 gl) {
		onDrawFrameCounter++;
		
		gl.glEnable(GL10.GL_TEXTURE_2D);
		gl.glClear(GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT);

		bindCameraTexture(gl);
		
		gl.glLoadIdentity();
		GLU.gluLookAt(gl, 0, 0, 4.2f, 0, 0, 0, 0, 1, 0);
		
		gl.glRotatef(onDrawFrameCounter,1,0,0); //Rotate the camera image
		gl.glRotatef((float)Math.sin(onDrawFrameCounter/20.0f)*40,0,1,0); //Rotate the camera image
		gl.glRotatef((float)Math.cos(onDrawFrameCounter/40.0f)*40,0,0,1); //Rotate the camera image

		gl.glNormal3f(0,0,1);
		gl.glDrawArrays(GL10.GL_TRIANGLE_STRIP, 0, 4);			
		gl.glDrawArrays(GL10.GL_TRIANGLE_STRIP, 4, 4);
		gl.glDrawArrays(GL10.GL_TRIANGLE_STRIP, 8, 4);
		gl.glDrawArrays(GL10.GL_TRIANGLE_STRIP,12, 4);
		gl.glDrawArrays(GL10.GL_TRIANGLE_STRIP,16, 4);
		gl.glDrawArrays(GL10.GL_TRIANGLE_STRIP,20, 4);
	}

	public void onSurfaceChanged(GL10 gl, int width, int height) {
		gl.glViewport(0, 0, width, height);

		float ratio = (float) width / height;
		gl.glMatrixMode(GL10.GL_PROJECTION);
		gl.glLoadIdentity();
		gl.glFrustumf(-ratio, ratio, -1, 1, 1, 10);
		
		gl.glMatrixMode(GL10.GL_MODELVIEW);
		gl.glLoadIdentity();
		GLU.gluLookAt(gl, 0, 0, 4.2f, 0, 0, 0, 0, 1, 0);		
	}

	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		gl.glHint(GL10.GL_PERSPECTIVE_CORRECTION_HINT, GL10.GL_FASTEST);

		gl.glClearColor(0, 0, 0, 0);
		gl.glEnable(GL10.GL_CULL_FACE);
		gl.glShadeModel(GL10.GL_SMOOTH);
		gl.glEnable(GL10.GL_DEPTH_TEST);
		
		cubeBuff = makeFloatBuffer(camObjCoord);
		texBuff = makeFloatBuffer(camTexCoords);		
		gl.glVertexPointer(3, GL10.GL_FLOAT, 0, cubeBuff);
		gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
		gl.glTexCoordPointer(2, GL10.GL_FLOAT, 0, texBuff);
		gl.glEnableClientState(GL10.GL_TEXTURE_COORD_ARRAY);
	}	
	
	/**
	 * Generates a texture from the black and white array filled by the onPreviewFrame
	 * method.
	 */
	void bindCameraTexture(GL10 gl) {
		synchronized(this) {
			if (cameraTexture==null)
				cameraTexture=new int[1];
			else
				gl.glDeleteTextures(1, cameraTexture, 0);
			
			gl.glGenTextures(1, cameraTexture, 0);
			int tex = cameraTexture[0];
			gl.glBindTexture(GL10.GL_TEXTURE_2D, tex);
			gl.glTexImage2D(GL10.GL_TEXTURE_2D, 0, GL10.GL_LUMINANCE, 256, 256, 0, GL10.GL_LUMINANCE, GL10.GL_UNSIGNED_BYTE, ByteBuffer.wrap(glCameraFrame));
			gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_LINEAR);
		}
	}
	
	/**
	 * This method is called if a new image from the camera arrived. The camera
	 * delivers images in a yuv color format. It is converted to a black and white
	 * image with a size of 256x256 pixels (only a fraction of the resulting image
	 * is used). Afterwards Rendering the frame (in the main loop thread) is started by
	 * setting the newFrameLock to true. 
	 */
	public void onPreviewFrame(byte[] yuvs, Camera camera) {		
   		int bwCounter = 0;
   		int yuvsCounter = 0;
//   		for (int y = 0; y < 160; y++) {
//   			System.arraycopy(yuvs, yuvsCounter, glCameraFrame, bwCounter, 240);
//   			yuvsCounter = yuvsCounter + 240;
//   			bwCounter = bwCounter + 256;
//   		}

		for (int y = 0; y < glCameraFrame.length; y++)
			glCameraFrame[y] = 0;

		if (imageProcessor != null)
			imageProcessor.process(yuvs);
	}
	
	FloatBuffer makeFloatBuffer(float[] arr) {
		ByteBuffer bb = ByteBuffer.allocateDirect(arr.length*4);
		bb.order(ByteOrder.nativeOrder());
		FloatBuffer fb = bb.asFloatBuffer();
		fb.put(arr);
		fb.position(0);
		return fb;
	}
	
	final static float camObjCoord[] = new float[] {
				// FRONT
				 -2.0f, -1.5f,  2.0f,
				  2.0f, -1.5f,  2.0f,
				 -2.0f,  1.5f,  2.0f,
				  2.0f,  1.5f,  2.0f,
				 // BACK
				 -2.0f, -1.5f, -2.0f,
				 -2.0f,  1.5f, -2.0f,
				  2.0f, -1.5f, -2.0f,
				  2.0f,  1.5f, -2.0f,
				 // LEFT
				 -2.0f, -1.5f,  2.0f,
				 -2.0f,  1.5f,  2.0f,
				 -2.0f, -1.5f, -2.0f,
				 -2.0f,  1.5f, -2.0f,
				 // RIGHT
				  2.0f, -1.5f, -2.0f,
				  2.0f,  1.5f, -2.0f,
				  2.0f, -1.5f,  2.0f,
				  2.0f,  1.5f,  2.0f,
				 // TOP
				 -2.0f,  1.5f,  2.0f,
				  2.0f,  1.5f,  2.0f,
				 -2.0f,  1.5f, -2.0f,
				  2.0f,  1.5f, -2.0f,
				 // BOTTOM
				 -2.0f, -1.5f,  2.0f,
				 -2.0f, -1.5f, -2.0f,
				  2.0f, -1.5f,  2.0f,
				  2.0f, -1.5f, -2.0f,
			};
			final static float camTexCoords[] = new float[] {
				// Camera preview
				 0.0f, 0.0f,
				 0.9375f, 0.0f,
				 0.0f, 0.625f,
				 0.9375f, 0.625f,

				// BACK
				 0.9375f, 0.0f,
				 0.9375f, 0.625f,
				 0.0f, 0.0f,
				 0.0f, 0.625f,
				// LEFT
				 0.9375f, 0.0f,
				 0.9375f, 0.625f,
				 0.0f, 0.0f,
				 0.0f, 0.625f,
				// RIGHT
				 0.9375f, 0.0f,
				 0.9375f, 0.625f,
				 0.0f, 0.0f,
				 0.0f, 0.625f,
				// TOP
				 0.0f, 0.0f,
				 0.9375f, 0.0f,
				 0.0f, 0.625f,
				 0.9375f, 0.625f,
				// BOTTOM
				 0.9375f, 0.0f,
				 0.9375f, 0.625f,
				 0.0f, 0.0f,
				 0.0f, 0.625f		 
			};

	public void setImageProcessor (ImageProcessor imageProcessor)
	{
		this.imageProcessor = imageProcessor;
	}

}