// File: NeheTGATexture.cpp
// Copyright 2006 GameDev.net LLC, Carsten Haubold
//
// Title: A class for loading TGA-textures
//
// Version History:
//		12 July 06	-	created
//		13 July 06	-	errors in TGA loading fixed (lc_overlord)
//		14 July 06	-	Added type 10 RLE compressed TGA loading (lc_overlord)
//
#include "NeheTGATexture.h"

using namespace std;
namespace NeHe{
	//Function: imageTGA
	//empty constructor
	imageTGA::imageTGA(){
		m_width = 0;
		m_height = 0;
		m_bpp = 0;
	}
	//Function: imageTGA
	//constructor which loads the specified TGA file
	//
	//Parameters:
	//	filename - full or relative path to the texture, has to be .tga	
	imageTGA::imageTGA(string filename, imageSampler sampler){
		
		ifstream File(filename.c_str(), ios::in | ios::binary);
		unsigned char header[20];
	
		if (!File)
		{
			m_id=0;
			exit(0);
		}
	
		//read all 18 bytes of the hrader
		File.read (reinterpret_cast<char *>(header), sizeof (char)*18);
	
		//should be image type 2 (color) or type 10 (rle compressed color)
		if (header[2] != 2 && header[2] != 10)
		{
			File.close();
			m_id=0;
			exit(0);
		}

		//if there is an image ID section then skip over it
		if (header[0])
		{
			File.seekg(header[0],ios_base::cur);
		}

		// get the size and bitdepth from the header
		m_width = header[13] * 256 + header[12];
		m_height = header[15] * 256 + header[14];
		m_bpp = header[16] / 8;

		if (m_bpp != 3 && m_bpp != 4) 
		{
			File.close();
			m_id=0;
			exit(0);
		}
		
		long imageSize = m_width * m_height * m_bpp;
		
		//allocate memory for image data
		unsigned char *data = new unsigned char[imageSize];
		
		//read the uncompressed image data if type 2
		if (header[2] == 2) 
		{
			File.read(reinterpret_cast<char *>(data), sizeof (char)*imageSize);
		}

		long ctpixel=0,ctloop=0;

		//read the compressed image data if type 10
		if (header[2] == 10)  
		{
			// stores the rle header and the temp color data
			unsigned char rle;
			unsigned char color[4];
		
			while (ctpixel<imageSize)
			{
				// reads the the RLE header
				File.read(reinterpret_cast<char *>(&rle), 1);

				// if the rle header is below 128 it means that what folows is just raw data with rle+1 pixels
				if (rle<128)
				{
					File.read(reinterpret_cast<char *>(&data[ctpixel]), m_bpp*(rle+1));
					ctpixel+=m_bpp*(rle+1);
				}

				// if the rle header is equal or above 128 it means that we have a string of rle-127 pixels 
				// that use the folowing pixels color
				else 
				{
					// read what color we should use
					File.read(reinterpret_cast<char *>(&color[0]), m_bpp);

					// insert the color stored in tmp into the folowing rle-127 pixels
					ctloop=0;
					while(ctloop<(rle-127))
					{						
						data[ctpixel]=color[0];
						data[ctpixel+1]=color[1];
						data[ctpixel+2]=color[2];
						if (m_bpp==4)
						{
							data[ctpixel+3]=color[3];
						}

						ctpixel+=m_bpp;
						ctloop++;
					}
				}
			}
		}
		
		ctpixel=0;

		//Because TGA file store their colors in BGRA format we need to swap the red and blue color components
		while (ctpixel<imageSize)
		{
			data[ctpixel] ^= data[ctpixel+2] ^=	data[ctpixel] ^= data[ctpixel+2];
			ctpixel+= m_bpp;
		}

		//close file
		File.close();
		//selecting the image type
		unsigned int color_mode=GL_RGB;
		if(m_bpp==4) color_mode=GL_RGBA;
		//Generating textures for OpenGL
		glGenTextures(1,&m_id);
		glBindTexture(GL_TEXTURE_2D,m_id);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,sampler.m_magFilter);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,sampler.m_minFilter);		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,sampler.m_wrapS);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,sampler.m_wrapT);

		//Set the ansiotropic filtering level, if set this will help reducing the blurriness of trilinear filtering
		if (sampler.m_ansiotropic)
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,sampler.m_ansiotropic);
		}
		
		// if any mipmaping is used build the texture with gluBuild2DMipmaps instead of glTexImage2D
		if(sampler.m_mipmap)
		{
			gluBuild2DMipmaps(GL_TEXTURE_2D, color_mode, m_width, m_height, color_mode, GL_UNSIGNED_BYTE, data); 		
		}

		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, color_mode, m_width, m_height, 0, color_mode, GL_UNSIGNED_BYTE, data);
		}

		delete[] data;
		data=NULL;
	}
	
	//Function: ~imageTGA
	//empty destructor
	imageTGA::~imageTGA()
	{
		//no need to do anything right now
	}

	//Function: getId
	//Returns the ID of the texture which is used to access it under OpenGL
	//
	//Returns:
	//	ID of the texture, 0 if an error occured
	GLuint imageTGA::getId()
	{
		return m_id;
	}

	//Function: hasAlpha
	//Returns true if the texture image contains an alpha channel
	//
	//Returns:
	//	true if texture has alpha channel
	bool imageTGA::hasAlpha()
	{
		if(m_bpp==4) return true;
		else return false;
	}


	//Function: createSampler
	//empty constructor, creates a default sampler
	imageSampler::imageSampler()
	{
		m_magFilter=GL_NEAREST;
		m_minFilter=GL_NEAREST;
		m_wrapS=GL_REPEAT;
		m_wrapT=GL_REPEAT;
		m_ansiotropic=0;
		m_mipmap=FALSE;

	}

	//Function: createSampler
	//constructor which set custom filter and wrap methods
	//
	//Parameters:
	//	magFilter - the mag filter to be used in this sampler
	//	minFilter - the mag filter to be used in this sampler
	//  wrapS - texture wrap method for the S axis
	//  wrapT - texture wrap method for the T axis
	imageSampler::imageSampler(unsigned int magFilter, unsigned int minFilter, unsigned int wrapS, unsigned int wrapT)
	{
		m_magFilter=magFilter;
		m_minFilter=minFilter;
		m_wrapS=wrapS;
		m_wrapT=wrapT;
		m_ansiotropic=0;
		m_mipmap=FALSE;

		if(m_minFilter==GL_NEAREST_MIPMAP_NEAREST) m_mipmap=TRUE;
		if(m_minFilter==GL_LINEAR_MIPMAP_NEAREST) m_mipmap=TRUE;
		if(m_minFilter==GL_NEAREST_MIPMAP_LINEAR) m_mipmap=TRUE;
		if(m_minFilter==GL_LINEAR_MIPMAP_LINEAR) m_mipmap=TRUE;
		
	}

	
	//Function: ansiotropicFiltering
	//
	//Parameters:
	//	ansio - the ansiotropic filtering level 
	void imageSampler::ansiotropicFiltering(unsigned int ansio)
	{
		m_ansiotropic=0;
		// Get the openGL extension string
		const unsigned char *pszExtensions = glGetString( GL_EXTENSIONS );

		// Search the extensions String For the existance of GL_EXT_texture_filter_anisotropic
		if(strstr( reinterpret_cast<const char *> (pszExtensions), "GL_EXT_texture_filter_anisotropic" )) 
		{
			m_ansiotropic=ansio;
		}
	}


	//Function: ~imageTGA
	//empty destructor
	imageSampler::~imageSampler()
	{
		//no need to do anything right now
	}



}
