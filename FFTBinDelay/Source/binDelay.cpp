
#include "binDelay.h"

BinDelay::BinDelay(int sizeInBlocks)
{
	createIndexArray();

	for (int i = 0; i < MainVar::numBins; i++) {
		delays[i] = new ForwardCircularDelay(sizeInBlocks * 2, i * 5, false, numBinsArray[i] * 2);
	}

	if (MainVar::fftSize <= 32) {
		dubLinInc = linInc;
	}
}

void BinDelay::createIndexArray() {
	for (int i = 0; i < MainVar::numBins; i++) {
		if (i < linInc)
			indexArray[i] = i + 1;
		if (i >= linInc && i < dubLinInc)
			indexArray[i] = linInc + (i - linInc) * 2 + 1;
		if (i >= dubLinInc)
			indexArray[i] = (int)(dubLinInc - linInc) * 2 + linInc //add start value + 15
				+ (i - dubLinInc) * 2 //add two for every index + 0 tot 40
				+ pow((i - dubLinInc) / (float) ((MainVar::numBins) - dubLinInc) , 7.0f) // exponential increase from 0-1
				* (MainVar::fftSize - indexArray[dubLinInc] - ((MainVar::numBins) - dubLinInc) * 2); // 
	}

	for (int i = 0; i < MainVar::numBins; i++) {
		if (i == MainVar::numBins - 1) {
			numBinsArray[i] = MainVar::fftSize - 1 - indexArray[i];
		}
		else {
			numBinsArray[i] = indexArray[i + 1] - indexArray[i];
		}
	}
}

void BinDelay::pushIntoBinDelay(const dsp::Complex<float>* inputFFT) {
	for (int i = 0; i < MainVar::numBins; i++) {
		for (int b = 0; b < numBinsArray[i]; b++) { //telt door de fftbins heen
			delays[i]->addSample(inputFFT[indexArray[i] + b]._Val[0], b * 2);
			if (phaseInDelay) {
				delays[i]->applyFeedback(b * 2 + 1, 0.0); // SET FEEDBACK
				delays[i]->addSample(inputFFT[indexArray[i] + b]._Val[1], b * 2 + 1);
			}
		}
	}
}

void BinDelay::adjustPointers() {
	for (int i = 0; i < MainVar::numBins; i++) {
		delays[i]->adjustDelayCentre();
	}
}

void BinDelay::getOutputFromBinDelay(dsp::Complex<float> * writeFFT) {
	for (int i = 0; i < MainVar::numBins; i++) {
		for (int b = 0; b < numBinsArray[i]; b++) { //telt door de fftbins heen
			writeFFT[indexArray[i] + b]._Val[0] = delays[i]->readSample(b * 2);
			if (phaseInDelay) {
				writeFFT[indexArray[i] + b]._Val[1] = delays[i]->readSample(b * 2 + 1);
			}
			delays[i]->applyFeedback(b * 2, feedback); // SET FEEDBACK
		}
	}
}

void BinDelay::newBufferSize(int sizeinblocks) {
	for (int i = 0; i < MainVar::numBins; i++) {
		delays[i]->resizeBuffer(sizeinblocks * 2);
	}
}

void BinDelay::setDelayTime(int index, float value) {
	delays[index]->setDelayTime(value);
}