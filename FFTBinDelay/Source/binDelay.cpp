/*
  ==============================================================================

    binDelay.cpp
    Created: 19 Dec 2017 10:50:18am
    Author:  Harold

  ==============================================================================
*/

#include "binDelay.h"

BinDelay::BinDelay(int fftSize, int numOverlaps)
			: fftSize(fftSize) {
	createIndexArray();

	for (int i = 0; i < nBins; i++) {
		delays[i] = new ForwardCircularDelay(numOverlaps * (44100 / fftSize) * 4, i * 16, false, numBinsArray[i]);
	}
}

void BinDelay::createIndexArray() {
	for (int i = 0; i < nBins; i++) {
		if (i < linR)
			indexArray[i] = i;
		if (i >= linR & i < linDR)
			indexArray[i] = linR + (i - linR) * 2;
		if (i >= linDR)
			indexArray[i] = (linDR - linR) * 2 + linR //add start value + 15
				+ (i - linDR) * 2 //add two for every index + 0 tot 40
				+ pow((i - linDR) / (float) (nBins - linDR) , 3.0f) // exponential increase from 0-1
				* (fftSize * 0.4); // 
	}

	for (int i = 0; i < nBins; i++) {
		if (i == nBins - 1) {
			numBinsArray[i] = fftSize / 2 - indexArray[i];
		}
		else {
			numBinsArray[i] = indexArray[i + 1] - indexArray[i];
		}
	}
}

void BinDelay::pushMagnitudesIntoDelay(dsp::Complex<float>* inputFFT) {
	for (int i = 0; i < nBins; i++) {
		for (int b = 0; b < numBinsArray[i]; b++) { //telt door de fftbins heen
			delays[i]->addValue(inputFFT[indexArray[i] + b]._Val[0], b);
		}
	}
}

void BinDelay::adjustPointers() {
	for (int i = 0; i < nBins; i++) {
		delays[i]->adjustPointers();
	}
}

void BinDelay::getOutputMagnitudes(dsp::Complex<float> * writeFFT) {
	for (int i = 0; i < nBins; i++) {
		for (int b = 0; b < numBinsArray[i]; b++) { //telt door de fftbins heen
			writeFFT[indexArray[i] + b]._Val[0] = delays[i]->readValue(b);
			delays[i]->feedBack(b, feedback); // SET FEEDBACK
		}
	}
}

void BinDelay::setDelayTime(int * delayArray) {
	for (int i = 0; i < nBins; i++) {
		delays[i]->setDelayTime(delayArray[i]);
	}
}