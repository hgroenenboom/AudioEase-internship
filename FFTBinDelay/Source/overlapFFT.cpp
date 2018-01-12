#include "overlapFFT.h"
#include <stdlib.h>
#include <time.h> 

//OverlapFFT::OverlapFFT() 
//	: binDelay() {
//}

OverlapFFT::OverlapFFT(dsp::FFT *fftfunctionpointer)
	: binDelay(MainVar::numOverlaps * (44100 / MainVar::fftSize) * MainVar::maxDelInSec),
	inputMemory(30 * MainVar::fftSize, MainVar::fftSize, true, 1),
	outputMemory(30 * MainVar::fftSize, MainVar::fftSize, true, 1)
{
	this->fftFunctionP = fftfunctionpointer;

	timeData = (dsp::Complex<float> *) malloc(MainVar::fftSize * sizeof(dsp::Complex<float>));
	memset(timeData, 0, sizeof(dsp::Complex<float>));
	spectralDataIn = (dsp::Complex<float> *) malloc(MainVar::fftSize * sizeof(dsp::Complex<float>));
	memset(spectralDataIn, 0, sizeof(dsp::Complex<float>));
	spectralDataOut = (dsp::Complex<float> *) malloc(MainVar::fftSize * sizeof(dsp::Complex<float>));
	memset(spectralDataOut, 0, sizeof(dsp::Complex<float>));

	// Initialize input & output memory
	//inputMemoryL = ForwardCircularDelay(20 * MainVar::fftSize, MainVar::fftSize, true, 1);
	//outputMemory = ForwardCircularDelay(20 * MainVar::fftSize, MainVar::fftSize, true, 1);

	hanningWindow.resize( MainVar::fftSize);
	createHanningWindow();
}

OverlapFFT::~OverlapFFT() {
	free(timeData);
	free(spectralDataIn);
	free(spectralDataOut);
}

// [1]
void OverlapFFT::pushDataIntoMemoryAndPerformFFTs(AudioSampleBuffer& buffer, int numSamples, int chan) {
	const float* channelData = buffer.getReadPointer(chan, 0);
	float* outBuffer = buffer.getWritePointer(chan, 0);
	this->channel = chan;

	for (int i = 0; i < numSamples; i++) {
		// [1]
		inputMemory.applyFeedback(0, 0.0f); 
		inputMemory.pushSample(channelData[i]); //push new data into the inputMemoryL. The memory will shift it's own pointer.

		//count if enough samples are collected for the FFT calculations
		inputForFFTCounter++; 
		inputForFFTCounter %= MainVar::fftSize;

		// [23467]
		if (inputForFFTCounter == 0) {
			runThroughFFTs(); // [2,3,4]
			adjustMemoryPointersAndClearOutputMemory(); // [6,7]
		}

		outBuffer[i] = getOutputData() * (1.0f / (MainVar::numOverlaps / 2));
	}
}

// [2,3,4]
void OverlapFFT::runThroughFFTs() { // 234
	// run through all overlaps
	for (int ovLap = 0; ovLap < MainVar::numOverlaps; ovLap++) {

		// calculate the indices for the current overlap
		startIndex = ovLap * (MainVar::fftSize / MainVar::numOverlaps);
		endIndex = startIndex + MainVar::fftSize;

		// fill the fft buffer
		fillFFTBuffer(startIndex, endIndex); //[2]

		applyFFT(ovLap); //[3]

		pushFFTDataIntoOutputDelayBuffer(startIndex, endIndex); //[4]
	}

}

// [2]
void OverlapFFT::fillFFTBuffer(int startindex, int endindex)
{
	for (int i = 0; i < MainVar::fftSize; i++) {
		timeData[i]._Val[0] = inputMemory.readSample(i + startindex);
	}
}

// [3]
void OverlapFFT::applyFFT(int ovLap) {
	//FFT
	applyHannningWindowToFftBuffer();

	for (int i = 0; i < MainVar::fftSize; i++) {
		timeData[i]._Val[1] = 0.0f;
	}

	//256 complex vectors and 256 complex conjugates of the first 256 vectors
	// i.e. 256 positive cycloids and 256 negative cycloids.
	fftFunctionP->perform(timeData, spectralDataIn, false);

	for (int i = 0; i < MainVar::fftSize; i++) {
		carToPol(&spectralDataIn[i]._Val[0], &spectralDataIn[i]._Val[1]);
		//if (i < 0.5 * fftsize) spectralDataIn[i]._Val[0] *= 2.0f;
		if (i == 0 || i > 0.5 * MainVar::fftSize) spectralDataIn[i]._Val[0] = 0.0f; //set negative real values to 0.
		spectralDataOut[i] = spectralDataIn[i];
	}


	//MODIFICATIONS	
	binDelay.getOutputFromBinDelay(spectralDataOut);
	binDelay.pushIntoBinDelay(spectralDataIn);
	binDelay.adjustPointers();

	for (int i = MainVar::fftSize / 2; i < MainVar::fftSize; i++) {
		spectralDataOut[i]._Val[0] = 0.0f;
	}


	// CHANNEL SPECIFIC MODIFICATIONS
	/*
	switch (chan) {
	case 0:
	case 1:
	}
	*/

	////IFFT
	for (int i = 0; i < MainVar::fftSize; i++) 
		polToCar(&spectralDataOut[i]._Val[0], &spectralDataOut[i]._Val[1]);

	fftFunctionP->perform(spectralDataOut, timeData, true);

	//Apply hanningWindow
	applyHannningWindowToFftBuffer();
}

// [3.1]
void OverlapFFT::applyHannningWindowToFftBuffer() {
	for (int i = 0; i < MainVar::fftSize; i++) { //run through each input amplitude value.
		timeData[i]._Val[0] *= hanningWindow[i];
	}
}

// [3.2]
void OverlapFFT::carToPol(float* inReOutM, float* inImOutPhi) {
	float re = *inReOutM;
	float im = *inImOutPhi;

	// phytagoras calculation
	*inReOutM = pow(pow(re, 2.0f) + pow(im, 2.0f), 0.5f);
	*inImOutPhi = atan2(im, re);
}

// [3.3]
void OverlapFFT::polToCar(float* inMOutRe, float* inPhiOutIm) {
	float mag = *inMOutRe;
	float phi = *inPhiOutIm;

	*inMOutRe = mag * cos(phi);
	*inPhiOutIm = mag * sin(phi);
}

// [4]
void OverlapFFT::pushFFTDataIntoOutputDelayBuffer(int startIndex, int endIndex) {
	for (int i = 0; i < MainVar::fftSize; i++) {
		outputMemory.addSample(timeData[i]._Val[0], i + startIndex);
	}
}

// [5]
float OverlapFFT::getOutputData() {
	return outputMemory.readSample(inputForFFTCounter);
}

// [6, 7]
void OverlapFFT::adjustMemoryPointersAndClearOutputMemory() {
	outputMemory.clearBufferData(-3 * MainVar::fftSize, -2 * MainVar::fftSize);

	inputMemory.adjustDelayCentre(MainVar::fftSize);
	outputMemory.adjustDelayCentre(MainVar::fftSize);
}

void OverlapFFT::setBinDelayWithNewSampleRate(int sampleRate) {
	binDelay.newBufferSize(MainVar::numOverlaps * (sampleRate / MainVar::fftSize) * MainVar::maxDelInSec);
}

// Initialize the hanning window
void OverlapFFT::createHanningWindow() {
	for (int i = 0; i < MainVar::fftSize; i++) {
		hanningWindow[i] = sin((float(i) / MainVar::fftSize) * float_Pi);
	}
}