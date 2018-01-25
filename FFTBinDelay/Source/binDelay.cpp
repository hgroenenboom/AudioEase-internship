
#include "binDelay.h"

BinDelay::BinDelay(int sizeInBlocks, int chan)
	: chan(chan)
{
	if (MainVar::fftSize <= 32) {
		dubLinInc = linInc;
	}

	createIndexArray();

	for (int i = 0; i < MainVar::numBands; i++) {
		// every block contains the assigned bininformation. Holding multiple magnitude and phase values.
		delayBands[i] = new ForwardCircularDelay(sizeInBlocks * 2, par::delayArray[i], false, numBinsPerBand[i] * 2);
	}
}

void BinDelay::createIndexArray() {
	for (int i = 0; i < MainVar::numBands; i++) {
		if (i < linInc)
			indexArray[i] = i + 1;
		if (i >= linInc && i < dubLinInc)
			indexArray[i] = linInc + (i - linInc) * 2 + 1;
		if (i >= dubLinInc)
			indexArray[i] = (int)(dubLinInc - linInc) * 2 + linInc //add start value + 15
				+ (i - dubLinInc) * 2 //add two for every index + 0 tot 40
				+ (int) (pow((i - dubLinInc) / (float) ((MainVar::numBands) - dubLinInc) , 7.0f) // exponential increase from 0-1
				* (MainVar::fftSize / 2 - indexArray[dubLinInc] - ((MainVar::numBands) - dubLinInc) * 2)); // 
	}

	for (int i = 0; i < MainVar::numBands; i++) {
		if (i == MainVar::numBands - 1) {
			numBinsPerBand[i] = MainVar::fftSize / 2 - 1 - indexArray[i];
		}
		else {
			numBinsPerBand[i] = indexArray[i + 1] - indexArray[i];
		}
	}
}

void BinDelay::writeIntoBinDelay(const dsp::Complex<float>* inputFFT) {
	// a bindelay bin contains numBinsPerBand[i] amount of complex numbers.
	// example with 3 fftbins: [Mag1, pha1, Mag2, pha2, Mag3, pha3]
	for (int i = 0; i < MainVar::numBands; i++) { //telt door de bindelay bins heen
		for (int b = 0; b < numBinsPerBand[i]; b++) { //telt door de assigned fftbins heen
			delayBands[i]->addSample(inputFFT[indexArray[i] + b]._Val[0], b * 2);
			if (phaseInDelay) {
				delayBands[i]->addSample( inputFFT[indexArray[i] + b]._Val[1], b * 2 + 1); //write Im values.
			}
		}
	}
}

void BinDelay::adjustPointers() {
	for (int i = 0; i < MainVar::numBands; i++) {
		delayBands[i]->adjustDelayCentre();
	}
}

void BinDelay::feedbackAndReadFromBinDelay(dsp::Complex<float> * inputComplexArray) {
	for (int i = 0; i < MainVar::numBands; i++) { //telt door de bindelay bins heen
		for (int b = 0; b < numBinsPerBand[i]; b++) { //telt door de assigned fftbins heen

			carToPol(delayBands[i]->getWriteSampleAdress(b * 2), delayBands[i]->getWriteSampleAdress(b * 2 + 1));

			/*if (i < 5) {
				delayBands[i]->applyFeedbackOnWritePos(bindata.fDbinData[(int)(panLocations[i] * 24.0f) * 2 * MainVar::fftSize
					+ chan*MainVar::fftSize + (indexArray[i] + b) * 2], b * 2);
				delayBands[i]->addSample(bindata.fDbinData[(int)(panLocations[i] * 24.0f) * 2 * MainVar::fftSize
					+ chan*MainVar::fftSize + (indexArray[i] + b) * 2 + 1], b * 2 + 1);
			}
			else {
				delayBands[i]->applyFeedbackOnWritePos(0.0f, b * 2);
			}*/

			delayBands[i]->applyFeedbackOnWritePos(*par::feedBack, b * 2); // FEEDBACK
			polToCar(delayBands[i]->getWriteSampleAdress(b * 2), delayBands[i]->getWriteSampleAdress(b * 2 + 1));

			// READ
			float irMag = bindata.fDbinData[(int)(panLocations[i] * 24.0f) * 2 * MainVar::fftSize + chan*MainVar::fftSize + (indexArray[i] + b) * 2];
				//bindata.fDbinData[(int)(panLocations[i] * 24.0f) * 2 * MainVar::fftSize + chan*MainVar::fftSize + (indexArray[i] + b) * 2 + 2] * 0.3 +
				//bindata.fDbinData[(int)(panLocations[i] * 24.0f) * 2 * MainVar::fftSize + chan*MainVar::fftSize + (indexArray[i] + b) * 2 - 2] * 0.4;
			//temp = interpolateFromBinData(i, b, 0, 5);

			float delReadRe = delayBands[i]->readSample(b * 2);
			float delReadIm = delayBands[i]->readSample(b * 2 + 1);
			carToPol(&delReadRe, &delReadIm);

			inputComplexArray[indexArray[i] + b]._Val[0] = delReadRe * irMag * (1.0f - par::ampArray[i]);
			if (phaseInDelay) {
				float irPha = bindata.fDbinData[(int)(panLocations[i] * 24.0f) * 2 * MainVar::fftSize + chan*MainVar::fftSize + (indexArray[i] + b) * 2 + 1];
				inputComplexArray[indexArray[i] + b]._Val[1] = delReadIm + irPha;
			}

		}
	}
}

void BinDelay::newBufferSize(int sizeinblocks) {
	for (int i = 0; i < MainVar::numBands; i++) {
		delayBands[i]->resizeBuffer(sizeinblocks * 2);
	}
}

void BinDelay::setDelayTime(int index, float value) {
	delayBands[index]->setDelayTime(value);
}