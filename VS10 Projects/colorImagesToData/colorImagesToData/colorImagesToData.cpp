// colorImagesToData.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "CImg.h"
#include <direct.h>

using namespace std;
using namespace cimg_library;

int _tmain(int argc, wchar_t* argv[])
{
	int startFile = 0;
	int endFile = 0;
	if (argc != 3 && argc != 2) {
		return -1;
	}

	startFile = _wtoi(argv[1]);
	if (argc == 3) {
		endFile = _wtoi(argv[2]);
	} else {
		endFile = startFile;
	}

	char depthPath[256];
	char colorPath[256];
	char colorDataPath[256];

	ifstream pathFile("paths.txt");
	pathFile.getline(depthPath,256);
	pathFile.getline(colorPath,256);
	pathFile.getline(colorDataPath,256);
	pathFile.close();

	mkdir(colorDataPath);

	CImg<BYTE> colorImage;
	CImgDisplay main_disp(colorImage,"Color Images");

	for (int fileOn = startFile; fileOn < endFile+1; fileOn++) {
		FILE * dataRFile;
		FILE * dataGFile;
		FILE * dataBFile;
		char bufferFileName[128];
		unsigned int imageWidth = 640U;
		unsigned int imageHeight = 480U;
		sprintf(bufferFileName,"%s/%.6u.bmp",colorPath,fileOn);
		colorImage.load(bufferFileName);
		main_disp.display(colorImage);

		sprintf(bufferFileName,"%s/%.6u_R.csv",colorDataPath,fileOn);
		dataRFile = fopen (bufferFileName,"w");
		sprintf(bufferFileName,"%s/%.6u_G.csv",colorDataPath,fileOn);
		dataGFile = fopen (bufferFileName,"w");
		sprintf(bufferFileName,"%s/%.6u_B.csv",colorDataPath,fileOn);
		dataBFile = fopen (bufferFileName,"w");

		unsigned int y = 0;
		unsigned int x = 0;
		y = 0;
		for (; y < imageHeight; ++y) {
			x = 0;
			for (; x < imageWidth-1; ++x) {
				fprintf(dataRFile,"%u,",(unsigned int)colorImage(x,y,0,0));
				fprintf(dataGFile,"%u,",(unsigned int)colorImage(x,y,0,1));
				fprintf(dataBFile,"%u,",(unsigned int)colorImage(x,y,0,2));
			}
			fprintf(dataRFile,"%u\n",(unsigned int)colorImage(x,y,0,0));
			fprintf(dataGFile,"%u\n",(unsigned int)colorImage(x,y,0,1));
			fprintf(dataBFile,"%u\n",(unsigned int)colorImage(x,y,0,2));
		}

		fclose(dataRFile);
		fclose(dataGFile);
		fclose(dataBFile);
	}

	return 0;
}

