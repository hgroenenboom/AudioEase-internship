#pragma once

#define _USE_MATH_DEFINES

#include <vector>
#include <math.h>
#include "../JuceLibraryCode/JuceHeader.h"

class FFTProcessor
{
	public:
		FFTProcessor();
		FFTProcessor(int fftSize);

		//FFT Buffers:
		//std::vector<float> inputData; // fftSize
		float fftData[1024]; // 2 * fftSize = 1024
		//std::vector<float> audioFFTData; //2 * fftSize = 1024

	private:
		int fftSize = 512;

		dsp::FFT *fftFunctionP; //pointer to the FFT Function class.

};