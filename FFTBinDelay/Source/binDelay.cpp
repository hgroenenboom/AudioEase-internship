
#include "binDelay.h"

BinDelay::BinDelay(int* fftSize, int numOverlaps, int sizeInBlocks)
			: fftSize(fftSize)
{
	createIndexArray();

	for (int i = 0; i < nBins; i++) {
		delays[i] = new ForwardCircularDelay(sizeInBlocks, i * 2, false, numBinsArray[i]);
	}

	if (*fftSize <= 32) {
		dubLinInc = linInc;
	}
}

void BinDelay::createIndexArray() {
	for (int i = 0; i < nBins; i++) {
		if (i < linInc)
			indexArray[i] = i;
		if (i >= linInc && i < dubLinInc)
			indexArray[i] = linInc + (i - linInc) * 2;
		if (i >= dubLinInc)
			indexArray[i] = (int)(dubLinInc - linInc) * 2 + linInc //add start value + 15
				+ (i - dubLinInc) * 2 //add two for every index + 0 tot 40
				+ pow((i - dubLinInc) / (float) (nBins - dubLinInc) , 7.0f) // exponential increase from 0-1
				* (*fftSize / 2 - indexArray[dubLinInc] - (nBins - dubLinInc) * 2); // 
	}

	for (int i = 0; i < nBins; i++) {
		if (i == nBins - 1) {
			numBinsArray[i] = *fftSize / 2 - indexArray[i];
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

void BinDelay::newBufferSize(int sizeinblocks) {
	for (int i = 0; i < nBins; i++) {
		delays[i]->resizeBuffer(sizeinblocks);
	}
}

void BinDelay::setDelayTime(int index, int value) {
	delays[index]->setDelayTime(value);
}