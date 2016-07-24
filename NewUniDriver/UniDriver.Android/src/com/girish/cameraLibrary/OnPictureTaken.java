package com.girish.cameraLibrary;

import java.io.File;

import android.graphics.Bitmap;
import android.hardware.Camera;

public interface OnPictureTaken {
	void pictureTaken(byte[] data, Camera camera);
}
