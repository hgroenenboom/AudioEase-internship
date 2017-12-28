
#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

#include "MainVar.h"
#include "blockDelay.h"

#include "../JuceLibraryCode/JuceHeader.h"

using namespace std;

class BinDelay {
	public:
		BinDelay::BinDelay(int sizeInBlocks);
		void createIndexArray();

		void pushIntoBinDelay(const dsp::Complex<float>* inputFFT);
		void adjustPointers();
		void getOutputFromBinDelay(dsp::Complex<float> * writeFFT);

		void newBufferSize(int sizeinblocks);

		void setDelayTime(int index, float value);
		void setFeedback(float feedbck) { this->feedback = feedbck; }
		float getFeedback() { return feedback; }

		bool phaseInDelay = true;
	private:
		//ofstream myfile;

		// indexing parameters
		int linInc = MainVar::numBins / 4;
		int dubLinInc = MainVar::numBins / 2;

		ForwardCircularDelay* delays[MainVar::numBins];
		int indexArray[MainVar::numBins];
		int numBinsArray[MainVar::numBins];

		float feedback = 0.0;
};
