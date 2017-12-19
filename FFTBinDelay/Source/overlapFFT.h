#pragma once

#define _USE_MATH_DEFINES

#include <math.h>
#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"

#include "singleFFT.h"
#include "blockDelay.h"

class overlapFFT {
	public:
		overlapFFT();
		overlapFFT(dsp::FFT *fftFunctionP, int numOverlaps = 2, int fftSize = 512); //the used fft function and the amount of windows.

		void setPanData(float* panData) { pan = panData; }

		// [1]
		void pushDataIntoMemoryAndPerformFFTs(AudioSampleBuffer& buffer, int numSamples, int channel);

		// [2,3,4]
		void runThroughFFTs(); 
		void fillFFTBuffer(int startIndex, int endIndex); // [2]
		void applyFFT(int ovLap); // [3]
		void applyHannningWindowToFftBuffer(); // [3.1]
		void carToPol(float* inReOutM, float* inImOutPhi); // [3.2]
		void polToCar(float* inMOutRe, float* inPhiOutIm); // [3.3]
		void applyHannningWindowToFftAudio(); // [3.4]
		void pushFFTDataIntoOutputDelayBuffer(int startIndex, int endIndex); // [4]
		
		// [5]
		float getOutputData(); 
		// [6, 7]
		void adjustMemoryPointersAndClearOutputMemory(); 

		void createHanningWindow();

		dsp::Complex<float> timeData[512];
		dsp::Complex<float> spectralData[512];

		ForwardCircularDelay* fftDelays[512];
	private:
		ForwardCircularDelay outputMemory;
		ForwardCircularDelay inputMemory;
		int inputCounter = 0;

		int fftSize;

		std::vector<float> hanningWindow;

		int window = 0,
			numOverlaps = 2,
			numFFTs = 2,
			startIndex,
			endIndex,
			bufferSize = 512,
			channel;

		FFTProcessor fft;

		dsp::FFT *fftFunctionP;

		float* pan;
};