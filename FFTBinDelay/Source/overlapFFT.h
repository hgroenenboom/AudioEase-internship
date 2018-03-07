#pragma once

#define _USE_MATH_DEFINES

#include <math.h>
#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"

#include "blockDelay.h"
#include "binDelay.h"
#include "MainVar.h"
#include "BineuralData.h"

class OverlapFFT {
	public:
		//OverlapFFT();
		OverlapFFT(int chan); //the used fft function and the amount of windows.
		~OverlapFFT();

		// [1]
		void pushDataIntoMemoryAndPerformFFTs(AudioSampleBuffer& buffer, int numSamples, int channel);

		// [2,3,4]
		void runThroughFFTs(); 
		void fillFFTBuffer(int startIndex, int endIndex); // [2]
		void applyFFT(int ovLap); // [3]
		void applyHannningWindowToFftBuffer(bool longOrShort); // [3.1]
		void carToPol(float* inReOutM, float* inImOutPhi); // [3.2]
		void polToCar(dsp::Complex<float>& val); // [3.3]
		void pushFFTDataIntoOutputDelayBuffer(int startIndex, int endIndex); // [4]
		
		// [5]
		float getOutputData();
		// [6, 7]
		void adjustMemoryPointersAndClearOutputMemory(); 

		void setBinDelayWithNewSampleRate(int sampleRate);
		void createHanningWindow();

		void printComplexArray(dsp::Complex<float> *array, int size = 1024, string str = "");

		void applyHalfHanningWindow() {
			//for (int i = 0; i < 5; i++) {
			//	timeBuffer[i].real( (float)i / 5.0f);
			//}
			for (int i = mVar::fftSize / 2; i < mVar::fftSize; i++) {
				timeBuffer[i].real( timeBuffer[i].real() * hanningWindowLong[i]);
			}
		}
			 
		//GUI variable:
		bool runFFTs = true;

		//temp
		float count = 0.0f, pan = 0.0f;

	private:
		int //numFFTs = 2,
			startIndex,
			endIndex,
			delayLength = 10;
		int channel = 0;
		float oldDryWet = 0.0f, dryWet = 0.0f;

		// Complexe buffers voor de fft berekeningen
		ScopedPointer<dsp::Complex<float>> timeBuffer;
		ScopedPointer<dsp::Complex<float>> spectralInBuffer;
		ScopedPointer<dsp::Complex<float>> spectralOutBuffer;

		// input en ouput memories
		ForwardCircularDelay outputMemory;
		ForwardCircularDelay inputMemory;
		int inputForFFTCounter = 0;

		std::vector<float> hanningWindow;
		std::vector<float> hanningWindowLong;

		// DE FFT PERFORM FUNCTIE ACCEPTEERT ALLEEN FLOATS
		dsp::FFT fftFunction;

		float interpolateWithNearMags(int index, int nBins, const dsp::Complex<float>* data) {
			float swag = 0.0f;
			for (int i = -nBins; i < nBins; i++) {
				if (index + i >= 0) {
					swag += data[index + i].real() * sin(float_Pi * (i + nBins) / (float)(nBins * 2));
				}
			}
			return swag;
		}

		Fade dryWetFade;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverlapFFT);

	public: BinDelay binDelay;
};