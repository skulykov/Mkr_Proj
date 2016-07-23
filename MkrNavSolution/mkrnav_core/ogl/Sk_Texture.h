#ifndef _SK_TEXTURE_H
#define _SK_TEXTURE_H

#include <math.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <set>
#include <time.h>

#include "common.h"
//#include "Sk_Paint.h"  -> adding this causes compiler problem
#include "../core/filemng.h"
#include "../main/IFileIO.h"
#include <png.h>

#if !SK_BUILD_FOR_ANDROID
#include <windows.h>
#endif

#pragma region PNGLoader Class
class PNGLoader
{
public:
	PNGLoader(){}
    bool Load(const char* filename, U8*& imageData, U16& width, U16& height, U8& colorDepth, bool& alpha){
#if SK_BUILD_FOR_ANDROID
		IFileIO *inf=IFileIO::get();
		if(inf==0)
		{
			return false;
		}
		U32 len = inf->GetSize(filename);
		U8* buffer = new U8[len];

		inf->Read(filename, buffer, len);
#else
		HANDLE hFile = CreateFile(filename,               // file to open
                       GENERIC_READ,          // open for reading
                       FILE_SHARE_READ,       // share for reading
                       NULL,                  // default security
                       OPEN_EXISTING,         // existing file only
                       FILE_ATTRIBUTE_NORMAL, // normal file
                       NULL);                 // no attr. template
 
    if (hFile == INVALID_HANDLE_VALUE) 
    { 
       
        return false; 
    }
	
	DWORD dsize;
	DWORD  dwBytesRead = 0;

	int len= GetFileSize(hFile,&dsize);
	U8* buffer = new U8[len];
	if( FALSE == ReadFile(hFile, buffer, len, &dwBytesRead, NULL) )
    {
       
        CloseHandle(hFile);
        return false;
    }


	CloseHandle(hFile);

#endif
		stream.setBuffer(buffer, len);

		//header for testing if it is a png
		png_byte header[8];

		//read the header
		stream.read(header, 8);

		//test if png
		int is_png = !png_sig_cmp(header, 0, 8);
		if (!is_png) {
			delete[] buffer;
			buffer = NULL;
			sk_log( SK_INFO, "Not a png file : %s", filename);
			return false;
		  }

		 //create png struct
		  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
			  NULL, NULL);
		  if (!png_ptr) {
			delete[] buffer;
			buffer = NULL;
			sk_log( SK_INFO, "Unable to create png struct : %s", filename);
			return false;
		  }

		  //create png info struct
		  png_infop info_ptr = png_create_info_struct(png_ptr);
		  if (!info_ptr) {
			png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
			sk_log( SK_INFO, "Unable to create png info : %s", filename);
			delete[] buffer;
			buffer = NULL;
			return false;
		  }

		  //create png info struct
		  png_infop end_info = png_create_info_struct(png_ptr);
		  if (!end_info) {
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
			sk_log( SK_INFO, "Unable to create png end info : %s", filename);
			delete[] buffer;
			buffer = NULL;
			return false;
		  }

		  //png error stuff, not sure libpng man suggests this.
		  if (setjmp(png_jmpbuf(png_ptr))) {
			delete[] buffer;
			buffer = NULL;
			sk_log( SK_INFO, "Error during setjmp : %s", filename);
			png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
			return false;
		  }

		  //init png reading
		  //png_init_io(png_ptr, fp);
		  png_set_read_fn(png_ptr, &stream, pngReadCallback);

		  //let libpng know you already read the first 8 bytes
		  png_set_sig_bytes(png_ptr, 8);

		  // read all the info up to the image data
		  png_read_info(png_ptr, info_ptr);

		  //variables to pass to get info
		  int bit_depth, color_type;
		  png_uint_32 twidth, theight;

		  // get info about png
		  png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type,
			  NULL, NULL, NULL);

		  width = twidth;
		  height = theight;

		  if(color_type == PNG_COLOR_TYPE_RGB) {
			  colorDepth = 8;
			  alpha = false;
		  }
		  else {
			  colorDepth = 8;
			  alpha = true;
		  }

		  //update width and height based on png info

		  // Update the png info struct.
		  png_read_update_info(png_ptr, info_ptr);

		  // Row size in bytes.
		  int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

		  // Allocate the image_data as a big block, to be given to opengl
		  imageData = new png_byte[rowbytes * height];
		  if (!imageData) {
			//clean up memory and close stuff
			png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
			sk_log( SK_INFO, "Unable to allocate image_data while loading %s ", filename);
			delete[] buffer;
			buffer = NULL;
			return false;
		  }

		  //row_pointers is for pointing to image_data for reading the png with libpng
		  png_bytep *row_pointers = new png_bytep[height];
		  if (!row_pointers) {
			//clean up memory and close stuff
			png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
			delete[] imageData;
			sk_log( SK_INFO, "Unable to allocate row_pointer while loading %s ", filename);
			delete[] buffer;
			buffer = NULL;
			return false;
		  }
		  // set the individual row_pointers to point at the correct offsets of image_data
		for (int i = 0; i < height; ++i)
			row_pointers[height - 1 - i] = imageData + i * rowbytes;

		  //read the png into image_data through row_pointers
		  png_read_image(png_ptr, row_pointers);


		  //clean up memory and close stuff
		  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

		  delete[] row_pointers;
		  delete[] buffer;

		  return true;
	}


protected:
    static void pngReadCallback(png_structp png_ptr, png_bytep data, png_size_t length)
	{
		if(png_ptr->io_ptr == NULL)
			  return;

		BufferStream* stream = (BufferStream*)png_ptr->io_ptr;
		if(!stream->read(data, length))
			return;
	}
    BufferStream stream;
};
#pragma endregion

#pragma region Sk_Texture Class
struct TextureInfo {
    enum TextureFormat {
        RGB8      = 0,
        RGBA8     = 1,
        LUMINANCE_ALPHA =2
    };

    U16 width;
    U16 height;
    U32 minFilter;
    U32 magFilter;
    U16 pixelAlignment;
    TextureFormat format;
};

class Sk_Texture
{
private:
    U32 textureId;
	const char *filename;
    TextureInfo textureInfo;
public:
	Sk_Texture() 
	{
		textureInfo.format=TextureInfo::RGB8;
		textureInfo.width=0;
		textureInfo.height=0;
		textureInfo.minFilter=0;
		textureInfo.magFilter=0;
		textureId=0;
	}

	~Sk_Texture() {
		if(textureId!=0)
			Dispose();
	}

	TextureInfo *getTextureInfo(){return &textureInfo;}

	bool Load(const char *filename) {
		this->filename = filename;
		U8* imageData;
		U16 w, h;
		U8 depth;
		bool alpha;

		PNGLoader loader;

		if(loader.Load(filename, imageData, w, h, depth, alpha)) {
			GLenum format;
			sk_log(SK_ERROR,"Load Texture: %d %d %d %d \n",w,h,depth,alpha?1:0);
			if(depth == 8) {
				
				glGenTextures(1, &textureId);
				glBindTexture(GL_TEXTURE_2D, textureId);
				if(alpha) {
					format = GL_RGBA;
					textureInfo.format=TextureInfo::RGBA8;
				}
				else {
					format = GL_RGB;
					textureInfo.format=TextureInfo::RGBA8;
				}

				glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format,
								GL_UNSIGNED_BYTE, imageData);
				//http://stackoverflow.com/questions/8688980/android-opengl2-0-showing-black-textures?rq=1
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, /*GL_REPEAT*/GL_CLAMP_TO_EDGE );
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, /*GL_REPEAT*/GL_CLAMP_TO_EDGE );//Turns out that is required if your texture sizes are not powers of 2.

				textureInfo.width=w;
				textureInfo.height=h;
				textureInfo.minFilter=GL_LINEAR;
				textureInfo.magFilter=GL_LINEAR;

				glBindTexture(GL_TEXTURE_2D, 0);
				delete[] imageData;
				return true;
			}
			delete[] imageData;
			return false;
		}

		return false;

	}


	bool Load(void* rawData, TextureInfo& info) {
		glPixelStorei(GL_UNPACK_ALIGNMENT, info.pixelAlignment);

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		if(info.format == TextureInfo::LUMINANCE_ALPHA) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, info.width, info.height, 0,
						 GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, rawData);
		}
		return false;

		return true;
	}

	bool Reload() {
		bool res = Load(filename);
		if(!res) return false;
		glBindTexture(GL_TEXTURE_2D, textureId);
		SetFilters(textureInfo.minFilter, textureInfo.magFilter);
		glBindTexture(GL_TEXTURE_2D, 0);

		return true;
	}

	void Dispose() {
		glBindTexture(GL_TEXTURE_2D, textureId);
		unsigned int textures[] = {textureId};
		glDeleteTextures(1, textures);
		textureId=0;
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Bind() {
		glBindTexture(GL_TEXTURE_2D, textureId);
	}

	void Unbind() {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void SetFilters(U32 minFilter, U32 magFilter) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );
		textureInfo.minFilter=minFilter;
		textureInfo.magFilter=magFilter;
	}

};
#pragma endregion
#endif //_SK_TEXTURE_H