
#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

#include "blockDelay.h"

#include "../JuceLibraryCode/JuceHeader.h"

using namespace std;

class BinDelay {
	public:
		BinDelay::BinDelay(int* fftSize, int numOverlaps, int sizeInBlocks);
		void createIndexArray();

		void pushMagnitudesIntoDelay(dsp::Complex<float>* inputFFT);
		void adjustPointers();
		void getOutputMagnitudes(dsp::Complex<float> * writeFFT);

		void newBufferSize(int sizeinblocks);

		void setDelayTime(int index, int value);
		void setFeedback(float feedbck) { this->feedback = feedbck; }
		float getFeedback() { return feedback; }

	private:
		ofstream myfile;

		// indexing parameters
		static constexpr int nBins = 20; // amount of bin, value is changeble. Needs to be changed in multiSlider too.
		int linInc = nBins / 4;
		int dubLinInc = nBins / 2;

		ForwardCircularDelay* delays[nBins];
		int indexArray[nBins];
		int numBinsArray[nBins];

		float feedback = 0.0;

		int * fftSize;
};
