// File: NeheTGATexture.h
// Copyright 2006 GameDev.net LLC, Carsten Haubold
//
// Title: A class for loading TGA-textures
//
// Version History:
//		12 July 06	-	created
//
#ifndef _NeHe_TGA
#define _NeHe_TGA

//include Windows (for VC++)
#ifdef _WIN32
	#include <windows.h>
	#define WIN32_LEAN_AND_MEAN		// trim the excess fat from Windows
	#define WIN32_EXTRA_LEAN
#endif


// Define the ansiotropic filtering if it's not set by glExt.h
#ifndef GL_EXT_texture_filter_anisotropic
	#define GL_TEXTURE_MAX_ANISOTROPY_EXT     0x84FE
#endif


// OpenGL headers.
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
//#include <GL/freeglut.h>

#include <iostream>
#include <fstream>
#include <sstream>



using namespace std;

namespace NeHe{
	//Class: imageSampler
	//a class which contains sampler info for use when loading textures
	class imageSampler{
		public:
			//Function: createSampler
			//empty constructor, creates a default sampler
			imageSampler();

			//Function: createSampler
			//constructor which set custom filter and wrap methods
			//
			//Parameters:
			//	magFilter - the mag filter to be used in this sampler
			//	minFilter - the mag filter to be used in this sampler
			//  wrapS - texture wrap method for the S axis
			//  wrapT - texture wrap method for the T axis
			imageSampler(unsigned int magFilter, unsigned int minFilter, unsigned int wrapS, unsigned int wrapT);

			//Function: ansiotropicFiltering
			//
			//Parameters:
			//	ansio - the ansiotropic filtering level 
			void ansiotropicFiltering(unsigned int ansio);

			//Function: ~createSampler
			//empty destructor
			~imageSampler();

			//The mag filter
			unsigned int m_magFilter;
			//The min filter
			unsigned int m_minFilter;
			//GL_WRAP_S
			unsigned int m_wrapS;
			//GL_WRAP_T
			unsigned int m_wrapT;
			//boolean to tell if a mipmap is used
			bool m_mipmap;
			// variable for ansiotropic filtering level
			unsigned int m_ansiotropic;
	};


	//Class: imageTGA
	//a class which loads a TGA file and binds it to an OpenGL texture ID
	class imageTGA{
		public:
			//Function: imageTGA
			//empty constructor
			imageTGA();

			//Function: imageTGA
			//constructor which loads the specified TGA file
			//
			//Parameters:
			//	filename - full or relative path to the texture, has to be .tga
			//	sampler - sets the samplet to be used
			imageTGA(string filename, imageSampler sampler);

			//Function: ~imageTGA
			//empty destructor
			~imageTGA();

			//Function: getId
			//Returns the ID of the texture which is used to access it under OpenGL
			//
			//Returns:
			//	ID of the texture, 0 if an error occured
			GLuint getId();

			//Function: hasAlpha
			//Returns true if the texture image contains an alpha channel
			//
			//Returns:
			//	true if texture has alpha channel
			bool hasAlpha();
		private:
			//The width of the texture image
			int m_width;
			//height of the texture image
			int m_height;
			//bits per pixel
        	int m_bpp;
			//the ID
			GLuint m_id;
	};
}

#endif 
