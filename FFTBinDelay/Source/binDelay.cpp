
#include "binDelay.h"

BinDelay::BinDelay(int sizeInBlocks)
{
	if (MainVar::fftSize <= 32) {
		dubLinInc = linInc;
	}

	createIndexArray();

	for (int i = 0; i < MainVar::numBins; i++) {
		// every block contains the assigned bininformation. Holding multiple magnitude and phase values.
		delays[i] = new ForwardCircularDelay(sizeInBlocks * 2, i * 5, false, numBinsArray[i] * 2);
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
				+ (int) (pow((i - dubLinInc) / (float) ((MainVar::numBins) - dubLinInc) , 7.0f) // exponential increase from 0-1
				* (MainVar::fftSize / 2 - indexArray[dubLinInc] - ((MainVar::numBins) - dubLinInc) * 2)); // 
	}

	for (int i = 0; i < MainVar::numBins; i++) {
		if (i == MainVar::numBins - 1) {
			numBinsArray[i] = MainVar::fftSize / 2 - 1 - indexArray[i];
		}
		else {
			numBinsArray[i] = indexArray[i + 1] - indexArray[i];
		}
	}
}

void BinDelay::writeIntoBinDelay(const dsp::Complex<float>* inputFFT) {
	// a bindelay bin contains numBinsArray[i] amount of complex numbers.
	// example with 3 fftbins: [Mag1, pha1, Mag2, pha2, Mag3, pha3]
	for (int i = 0; i < MainVar::numBins; i++) { //telt door de bindelay bins heen
		for (int b = 0; b < numBinsArray[i]; b++) { //telt door de assigned fftbins heen
			delays[i]->addSample(inputFFT[indexArray[i] + b]._Val[0], b * 2);
			if (phaseInDelay) {
				delays[i]->addSample( inputFFT[indexArray[i] + b]._Val[1], b * 2 + 1); //write Im values.
			}
		}
	}
}

void BinDelay::adjustPointers() {
	for (int i = 0; i < MainVar::numBins; i++) {
		delays[i]->adjustDelayCentre();
	}
}

void BinDelay::feedbackAndReadFromBinDelay(dsp::Complex<float> * inputComplexArray) {
	for (int i = 0; i < MainVar::numBins; i++) { //telt door de bindelay bins heen
		for (int b = 0; b < numBinsArray[i]; b++) { //telt door de assigned fftbins heen
			// FEEDBACK
			carToPol(delays[i]->getWriteSampleAdress(b * 2), delays[i]->getWriteSampleAdress(b * 2 + 1));
			delays[i]->applyFeedbackOnWritePos(feedback, b * 2);
			polToCar(delays[i]->getWriteSampleAdress(b * 2), delays[i]->getWriteSampleAdress(b * 2 + 1));

			// READ
			inputComplexArray[indexArray[i] + b]._Val[0] = delays[i]->readSample(b * 2); //replace the corresponding real values.
			if (phaseInDelay) {
				inputComplexArray[indexArray[i] + b]._Val[1] = delays[i]->readSample(b * 2 + 1);
			}
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