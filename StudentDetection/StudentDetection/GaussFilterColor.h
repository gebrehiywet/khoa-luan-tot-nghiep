#include "cv.h"
#include "highgui.h"
#include <fstream>
using namespace std;

//#define PI 3.14

#pragma once

class GaussFilterColor
{
public:	
	double InvCov[2][2];
	
	double sub[3];
	double rsl[3];
	double b;

	double phi;
	double logOfSqrt2PiVariance;
	double threshold;

	CvMat* mean;
	CvMat* corrvariant;

	void TrainData(char *prefix, char *suffix, int number_images, int start_index, int end_index, char* fileOutput);
	void LoadData(char* fileOutput);
	IplImage* Classify(IplImage *imgColor, IplImage *mask);
	IplImage* Classify(IplImage *imgColor, IplImage *mask, float thresh);
	IplImage* Classify(IplImage *imgColor);
	void SetThreshold(double threshold);

	GaussFilterColor(void);
	~GaussFilterColor(void);
};
