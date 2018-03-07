#include "overlapFFT.h"
#include <stdlib.h>
#include <time.h> 

//OverlapFFT::OverlapFFT() 
//	: binDelay() {
//}

OverlapFFT::OverlapFFT(int chan)
	: channel(chan),
	binDelay(mVar::numOverlaps * (44100 / mVar::fftSize) * mVar::maxDelInSec, chan),
	inputMemory(30 * mVar::tdSize, mVar::tdSize, true, 1),
	outputMemory(30 * mVar::tdSize, mVar::tdSize, true, 1),
	fftFunction(mVar::fftOrder)
{
	timeBuffer = (dsp::Complex<float> *) malloc(mVar::fftSize * sizeof(dsp::Complex<float>));
	memset(timeBuffer, 0, sizeof(dsp::Complex<float>));
	spectralInBuffer = (dsp::Complex<float> *) malloc(mVar::fftSize * sizeof(dsp::Complex<float>));
	memset(spectralInBuffer, 0, sizeof(dsp::Complex<float>));
	spectralOutBuffer = (dsp::Complex<float> *) malloc(mVar::fftSize * sizeof(dsp::Complex<float>));
	memset(spectralOutBuffer, 0, sizeof(dsp::Complex<float>));

	// Initialize input & output memory
	//inputMemoryL = ForwardCircularDelay(20 * MainVar::fftSize, MainVar::fftSize, true, 1);
	//outputMemory = ForwardCircularDelay(20 * MainVar::fftSize, MainVar::fftSize, true, 1);

	hanningWindow.resize( mVar::tdSize);
	hanningWindowLong.resize(mVar::fftSize);
		createHanningWindow();
}

OverlapFFT::~OverlapFFT() {
}

// [1]
void OverlapFFT::pushDataIntoMemoryAndPerformFFTs(AudioSampleBuffer& buffer, int numSamples, int chan) {
	const float* channelData = buffer.getReadPointer(chan, 0);
	float* outBuffer = buffer.getWritePointer(chan, 0);
	this->channel = chan;
	dryWetFade.initFade(&dryWet, *par::dryWet, numSamples);

	for (int i = 0; i < numSamples; i++) {
		// [1]
		// This creates a delay of size: tdSize (now 256)
		inputMemory.pushSample(channelData[i]); //push new data into the inputMemoryL. The memory will shift it's own pointer.

		//count if enough samples are collected for the FFT calculations
		inputForFFTCounter++; 
		inputForFFTCounter %= mVar::tdSize;

		// [23467]
		if (inputForFFTCounter == 0) {
			runThroughFFTs(); // [2,3,4]
			adjustMemoryPointersAndClearOutputMemory(); // [6,7]
		}

		// hier staat de latency feitelijk ook in beschreven
		//dryWet = (*par::dryWet + oldDryWet) * 0.5f;
		outBuffer[i] = inputMemory.readSample(inputForFFTCounter - 1.5f * mVar::tdSize) * dryWet + (1.0f - dryWet) * getOutputData() * (1.0f / (mVar::numOverlaps / 2));
		//oldDryWet = dryWet;

		dryWetFade.fade();
	}
}

// [2,3,4]
void OverlapFFT::runThroughFFTs() { // 234
	// run through all overlaps
	for (int ovLap = 0; ovLap < mVar::numOverlaps; ovLap++) {

		// calculate the indices for the current overlap
		startIndex = ovLap * (mVar::tdSize / mVar::numOverlaps);
		endIndex = startIndex + mVar::tdSize;

		// fill the fft buffer
		fillFFTBuffer(startIndex, endIndex); //[2]

		applyFFT(ovLap); //[3]

		pushFFTDataIntoOutputDelayBuffer(startIndex, endIndex); //[4]
	}

}

// [2]
void OverlapFFT::fillFFTBuffer(int startindex, int endindex)
{
	// place fftSize samples in timeBuffer.
	for (int i = 0; i < mVar::tdSize; i++) {
		timeBuffer[i].real(inputMemory.readSample(i + startindex));
	}
}

// [3]
void OverlapFFT::applyFFT(int ovLap) {
	//FFT
	applyHannningWindowToFftBuffer(false);

	for (int i = 0; i < mVar::fftSize; i++) {
		timeBuffer[i].imag( 0.0f);
	}
	for (int i = mVar::tdSize; i < mVar::fftSize; i++) {
		timeBuffer[i].real(0.0f);
	}

	if (runFFTs) {
		// (fftSize*0.5) complex vectors and (fftSize*0.5) complex conjugates of the first (fftSize*0.5) vectors
		// i.e. fftSize*0.5 positive cycloids and fftSize*0.5 negative cycloids. This is done to cancel the imaginairy values when using the inverse fft.
		fftFunction.perform(timeBuffer, spectralInBuffer, false);
		//printComplexArray(spectralInBuffer, 256);

		for (int i = 0; i < mVar::fftSize; i++) {
			spectralOutBuffer[i].real( 0.0f);
			spectralOutBuffer[i].imag(0.0f);
			timeBuffer[i].real( 0.0f);
			timeBuffer[i].imag(0.0f);

			//spectralOutBuffer[i]._Val[0] = spectralInBuffer[i]._Val[0];
			//spectralOutBuffer[i]._Val[1] = spectralInBuffer[i]._Val[1];
		}


		//MODIFICATIONS
		

		////IFFT
		
		// BinDelay
		// Because it uses cyclical buffers, the read and write pointers point to the same values. This is why the order of operations is very important.
		// Correct order of operations: feedback -> read -> write -> adjustpointers.
		binDelay.feedbackAndReadFromBinDelay(spectralOutBuffer); //mag,phase
		binDelay.writeIntoBinDelay(spectralInBuffer); //re, im
		binDelay.adjustPointers();

		
		for (int i = 0; i < mVar::fftSize / 2; i++) {
			//dsp::Complex<float> temp[MainVar::fftSize];
			//memcpy(temp, spectralOutBuffer, MainVar::fftSize / 2 * sizeof(dsp::Complex<float>));
			//spectralOutBuffer[i]._Val[0] = interpolateWithNearMags(i, 3, temp);
			//carToPol(&spectralOutBuffer[i]._Val[0], &spectralOutBuffer[i]._Val[1]);

			//spectralOutBuffer[i]._Val[0] *= bindata.fDbinData[(int)(pan*24.0f) * 2*MainVar::fftSize
			//	+ channel*MainVar::fftSize + i * 2];
			//spectralOutBuffer[i]._Val[1] += bindata.fDbinData[(int)(pan*24.0f) * 2*MainVar::fftSize
			//	+ channel*MainVar::fftSize + i * 2 + 1];

			polToCar(spectralOutBuffer[i]);
		} 

		int center = 40;
		int range = 30;
		// create the complex conjugates
		for (int i = 0; i < mVar::fftSize/2; i++) {
			/*
			if (true) {
				carToPol(&spectralOutBuffer[i]._Val[0], &spectralOutBuffer[i]._Val[1]);
				if (i > (center - range) && i < center + range) {
					float temp = (float)(int)(abs(center - i)) / (float)range;
					spectralOutBuffer[i]._Val[0] *= temp;
				}
				else {
					spectralOutBuffer[i]._Val[0] *= 0.0f;
				}
				polToCar(&spectralOutBuffer[i]._Val[0], &spectralOutBuffer[i]._Val[1]);
			}
			else {
				carToPol(&spectralOutBuffer[i]._Val[0], &spectralOutBuffer[i]._Val[1]);
				if (i != center) {
					spectralOutBuffer[i]._Val[0] *= 0.0f;
				}
				polToCar(&spectralOutBuffer[i]._Val[0], &spectralOutBuffer[i]._Val[1]);
			} */

			if (!true) {
				spectralOutBuffer[min(mVar::fftSize - 1, mVar::fftSize - 1 - i)].real(spectralOutBuffer[i].real() );
				spectralOutBuffer[min(mVar::fftSize - 1, mVar::fftSize - 1 - i)].imag( -spectralOutBuffer[i].imag() );
			}
		} 

		// zero fill the other values
		//printComplexArray(spectralOutBuffer, 1024);

		//spectralOutBuffer[0].real(0.0f);
		//spectralOutBuffer[mVar::fftSize-1].real(0.0f);

		fftFunction.perform(spectralOutBuffer, timeBuffer, true);	

	}

	// Originele apply hanningWindow
	// applyHalfHanningWindow();
	// applyHannningWindowToFftBuffer(true);

	// Hoe apply ik hanning met ongelijke buffersizes. TDlength  != FDlength
	// Ik kan simpelweg de nageconvolueerde data negeren. Dan kan ik gewoon weer een simpele hanningwindow over de TDlength plaatsen en de rest zeropadden. Hiermee verzwak ik de filter.
	applyHannningWindowToFftBuffer(false);
	for (int i = mVar::tdSize; i < mVar::fftSize; i++) {
		timeBuffer[i].real(0.0f);
	}
	//
	// Nog een optie is om elke tdSamples buffer te hanningwindowen, hiermee verklein je het effect in het laag nauwelijks:
	//
	//for (int j = 0; j < 4; j++) {
	//	for (int i = 0; i < MainVar::tdSize; i++) { //run through each input amplitude value.
	//		timeBuffer[j * mVar::tdSize + i].real(hanningWindow[i] * timeBuffer[j * mVar::tdSize + i].real);
	//	}
	//}
	

	//printComplexArray(timeBuffer, 1024);
	count += 0.01f;
	if (count >= 12.0f) count = 0.0f;
}

// [3.1]
void OverlapFFT::applyHannningWindowToFftBuffer(bool longOrShort) {
	if (longOrShort) {
		for (int i = 0; i < mVar::fftSize; i++) { //run through each input amplitude value.
			timeBuffer[i].real(timeBuffer[i].real() * hanningWindowLong[i]);
		}
	}
	else {
		for (int i = 0; i < mVar::tdSize; i++) { //run through each input amplitude value.
			timeBuffer[i].real(timeBuffer[i].real() * hanningWindow[i]);
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
void OverlapFFT::polToCar(dsp::Complex<float>& val) {
	float mag = val.real();
	float phi = val.imag();

	val.real(mag * cos(phi));
	val.imag(mag * sin(phi));
}

// [4]
void OverlapFFT::pushFFTDataIntoOutputDelayBuffer(int strtindex, int endindex) {
	for (int i = 0; i < mVar::fftSize; i++) {
		outputMemory.addSample(timeBuffer[i].real(), i + strtindex);
	}
}

// [5]
float OverlapFFT::getOutputData() {
	return outputMemory.readSample(inputForFFTCounter);
}

// [6, 7]
void OverlapFFT::adjustMemoryPointersAndClearOutputMemory() {
	outputMemory.clearBufferData(-1 * mVar::tdSize, 0 * mVar::tdSize);

	inputMemory.adjustDelayCentre(mVar::tdSize);
	outputMemory.adjustDelayCentre(mVar::tdSize);
}

void OverlapFFT::setBinDelayWithNewSampleRate(int sampleRate) {
	binDelay.newBufferSize(mVar::numOverlaps * (sampleRate / mVar::tdSize) * mVar::maxDelInSec);
}

// Initialize the hanning window
void OverlapFFT::createHanningWindow() {
	for (int i = 0; i < mVar::tdSize; i++) {
		hanningWindow[i] = sin((float(i) / mVar::tdSize) * float_Pi);

		// Blackman-Harris Window
		//hanningWindow[i] = 0.35875f - 0.48829 * cos((2.0f * float_Pi * i) / (mVar::tdSize - 1))
		//	+ 0.14128 * cos((4.0f * float_Pi * i) / (mVar::tdSize - 1))
		//	+ 0.01168 * cos((6.0f * float_Pi * i) / (mVar::tdSize - 1));
	}

	for (int i = 0; i < mVar::fftSize; i++) {
		if (false) {
			//hanningWindowTimes2[i] = sin((float)(i - MainVar::fftSize / 4) / MainVar::fftSize * 2.0f * float_Pi);
			if (i < mVar::fftSize / 4) {
				hanningWindowLong[i] = sin((float)(i - mVar::fftSize / 4) / mVar::fftSize * float_Pi);
			}
			else {
				hanningWindowLong[i] = sin((float)(i - mVar::fftSize / 4) / (mVar::fftSize / 4 * 3) * float_Pi + 0.5f * float_Pi);
			}

			hanningWindowLong[i] = (hanningWindowLong[i] + 1.0f) / 2.0f;

			DBG("at " << i << ": " << hanningWindowLong[i]);
		}
		else {
			hanningWindowLong[i] = sin((float(i) / mVar::fftSize) * float_Pi);
		}
	}
}

void OverlapFFT::printComplexArray(dsp::Complex<float> *array, int size, string str) {
	DBG("||||||||||||||||||\n" << str);
	for (int i = 0; i < size; i++) {
		DBG("number " << i << " = " << array[i].real() << " + " << array[i].imag() << "*i");
	}
}
