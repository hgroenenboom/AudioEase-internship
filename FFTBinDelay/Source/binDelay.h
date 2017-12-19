/*
  ==============================================================================

    binDelay.h
    Created: 19 Dec 2017 10:50:18am
    Author:  Harold

  ==============================================================================
*/

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
		BinDelay::BinDelay(int fftSize = 512, int numOverlaps = 8);
		void createIndexArray();

		void pushMagnitudesIntoDelay(dsp::Complex<float>* inputFFT);
		void adjustPointers();
		void getOutputMagnitudes(dsp::Complex<float> * writeFFT);

		void setDelayTime(int * delayArray);
	private:
		ofstream myfile;

		int linR = 5;
		int linDR = 10;
		static constexpr int nBins = 30;

		dsp::Complex<float> outBuffer[512]; // TODO niet hardcoded
		ForwardCircularDelay* delays[nBins];
		int indexArray[nBins];
		int numBinsArray[nBins];

		int fftSize;
};
