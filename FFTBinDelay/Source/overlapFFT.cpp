#include "overlapFFT.h"
#include <stdlib.h>
#include <time.h> 

//OverlapFFT::OverlapFFT() 
//	: binDelay() {
//}

OverlapFFT::OverlapFFT()
	: binDelay(MainVar::numOverlaps * (44100 / MainVar::fftSize) * MainVar::maxDelInSec),
	inputMemory(30 * MainVar::samplesForTd, MainVar::samplesForTd, true, 1),
	outputMemory(30 * MainVar::samplesForTd, MainVar::samplesForTd, true, 1),
	fftFunction(MainVar::fftOrder)
{
	timeData = (dsp::Complex<float> *) malloc(MainVar::fftSize * sizeof(dsp::Complex<float>));
	memset(timeData, 0.0f, sizeof(dsp::Complex<float>));
	spectralDataIn = (dsp::Complex<float> *) malloc(MainVar::fftSize * sizeof(dsp::Complex<float>));
	memset(spectralDataIn, 0.0f, sizeof(dsp::Complex<float>));
	spectralDataOut = (dsp::Complex<float> *) malloc(MainVar::fftSize * sizeof(dsp::Complex<float>));
	memset(spectralDataOut, 0.0f, sizeof(dsp::Complex<float>));

	// Initialize input & output memory
	//inputMemoryL = ForwardCircularDelay(20 * MainVar::fftSize, MainVar::fftSize, true, 1);
	//outputMemory = ForwardCircularDelay(20 * MainVar::fftSize, MainVar::fftSize, true, 1);

	hanningWindow.resize( MainVar::samplesForTd);
	hanningWindowTimes2.resize(MainVar::fftSize);
		createHanningWindow();
}

OverlapFFT::~OverlapFFT() {
}

// [1]
void OverlapFFT::pushDataIntoMemoryAndPerformFFTs(AudioSampleBuffer& buffer, int numSamples, int chan) {
	const float* channelData = buffer.getReadPointer(chan, 0);
	float* outBuffer = buffer.getWritePointer(chan, 0);
	this->channel = chan;

	for (int i = 0; i < numSamples; i++) {
		// [1]
		//inputMemory.applyFeedback(0, 0.0f); 
		inputMemory.pushSample(channelData[i]); //push new data into the inputMemoryL. The memory will shift it's own pointer.

		//count if enough samples are collected for the FFT calculations
		inputForFFTCounter++; 
		inputForFFTCounter %= MainVar::samplesForTd;

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
		startIndex = ovLap * (MainVar::samplesForTd / MainVar::numOverlaps);
		endIndex = startIndex + MainVar::samplesForTd;

		// fill the fft buffer
		fillFFTBuffer(startIndex, endIndex); //[2]

		applyFFT(ovLap); //[3]

		pushFFTDataIntoOutputDelayBuffer(startIndex, endIndex); //[4]
	}

}

// [2]
void OverlapFFT::fillFFTBuffer(int startindex, int endindex)
{
	// place fftSize samples in timeData.
	for (int i = 0; i < MainVar::samplesForTd; i++) {
		timeData[i]._Val[0] = inputMemory.readSample(i + startindex);
	}
}

// [3]
void OverlapFFT::applyFFT(int ovLap) {
	//FFT
	applyHannningWindowToFftBuffer(false);

	for (int i = 0; i < MainVar::fftSize; i++) {
		timeData[i]._Val[1] = 0.0f;
	}
	for (int i = MainVar::samplesForTd; i < MainVar::fftSize; i++) {
		timeData[i]._Val[0] = 0.0f;
	}

	if (runFFTs) {
		for (int i = 0; i < MainVar::fftSize; i++) {
			spectralDataIn[i]._Val[0] = 0.0f;
			spectralDataIn[i]._Val[1] = 0.0f;
		}
		// (fftSize*0.5) complex vectors and (fftSize*0.5) complex conjugates of the first (fftSize*0.5) vectors
		// i.e. fftSize*0.5 positive cycloids and fftSize*0.5 negative cycloids. This is done to cancel the imaginairy values when using the inverse fft.
		fftFunction.perform(timeData, spectralDataIn, false);
		//printComplexArray(spectralDataIn, 256);

		for (int i = 0; i < MainVar::fftSize; i++) {
			spectralDataOut[i]._Val[0] = 0.0f;
			spectralDataOut[i]._Val[1] = 0.0f;
			timeData[i]._Val[0] = 0.0f;
			timeData[i]._Val[1] = 0.0f;

			spectralDataOut[i]._Val[0] = spectralDataIn[i]._Val[0];
			spectralDataOut[i]._Val[1] = spectralDataIn[i]._Val[1];
		}


		//MODIFICATIONS
		
		////IFFT
		
		// BinDelay
		// Because it uses cyclical buffers, the read and write pointers point to the same values. This is why the order of operations is very important.
		// Correct order of operations: feedback -> read -> write -> adjustpointers.
		binDelay.feedbackAndReadFromBinDelay(spectralDataOut);
		binDelay.writeIntoBinDelay(spectralDataIn);
		binDelay.adjustPointers();



		// create the complex conjugates
		for (int i = 0; i < MainVar::fftSize/2; i++) {
			spectralDataOut[min(MainVar::fftSize - 1, MainVar::fftSize-1 - i)]._Val[0] = spectralDataOut[i]._Val[0];
			spectralDataOut[min(MainVar::fftSize - 1, MainVar::fftSize-1 - i)]._Val[1] = -1.0f * spectralDataOut[i]._Val[1];
		}
		// zero fill the other values
		//printComplexArray(spectralDataOut, 1024);

		//spectralDataOut[0]._Val[0] = 0.0f;
		//spectralDataOut[MainVar::fftSize-1]._Val[0] = 0.0f;

		fftFunction.perform(spectralDataOut, timeData, true);	

	}

	//Apply hanningWindow
	applyHalfHanningWindow();
	applyHannningWindowToFftBuffer(true);

	//printComplexArray(timeData, 1024);

}

// [3.1]
void OverlapFFT::applyHannningWindowToFftBuffer(bool longOrShort) {
	if (longOrShort) {
		for (int i = 0; i < MainVar::fftSize; i++) { //run through each input amplitude value.
			timeData[i]._Val[0] *= hanningWindowTimes2[i];
		}
	}
	else {
		for (int i = 0; i < MainVar::samplesForTd; i++) { //run through each input amplitude value.
			timeData[i]._Val[0] *= hanningWindow[i];
		}
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
	outputMemory.clearBufferData(-1 * MainVar::samplesForTd, 0 * MainVar::samplesForTd);

	inputMemory.adjustDelayCentre(MainVar::samplesForTd);
	outputMemory.adjustDelayCentre(MainVar::samplesForTd);
}

void OverlapFFT::setBinDelayWithNewSampleRate(int sampleRate) {
	binDelay.newBufferSize(MainVar::numOverlaps * (sampleRate / MainVar::samplesForTd) * MainVar::maxDelInSec);
}

// Initialize the hanning window
void OverlapFFT::createHanningWindow() {
	for (int i = 0; i < MainVar::samplesForTd; i++) {
		hanningWindow[i] = sin((float(i) / MainVar::samplesForTd) * float_Pi);
		//hanningWindow[i] = pow(hanningWindow[i], log2((float)MainVar::numOverlaps));

	}

	for (int i = 0; i < MainVar::fftSize; i++) {
		hanningWindowTimes2[i] = sin((float(i) / MainVar::fftSize) * float_Pi);
		//hanningWindowTimes2[i] = pow(hanningWindowTimes2[i], log2((float)MainVar::numOverlaps));
	}
}

void OverlapFFT::printComplexArray(dsp::Complex<float> *array, int size, string str) {
	DBG("||||||||||||||||||\n" << str);
	for (int i = 0; i < size; i++) {
		DBG("number " << i << " = " << array[i]._Val[0] << " + " << array[i]._Val[1] << "*i");
	}
}
