#pragma once

#define _USE_MATH_DEFINES

#include <math.h>
#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"

#include "blockDelay.h"
#include "binDelay.h"
#include "MainVar.h"

class OverlapFFT {
	public:
		//OverlapFFT();
		OverlapFFT(dsp::FFT *fftFunctionP); //the used fft function and the amount of windows.
		~OverlapFFT();

		// [1]
		void pushDataIntoMemoryAndPerformFFTs(AudioSampleBuffer& buffer, int numSamples, int channel);

		// [2,3,4]
		void runThroughFFTs(); 
		void fillFFTBuffer(int startIndex, int endIndex); // [2]
		void applyFFT(int ovLap); // [3]
		void applyHannningWindowToFftBuffer(); // [3.1]
		void carToPol(float* inReOutM, float* inImOutPhi); // [3.2]
		void polToCar(float* inMOutRe, float* inPhiOutIm); // [3.3]
		void pushFFTDataIntoOutputDelayBuffer(int startIndex, int endIndex); // [4]
		
		// [5]
		float getOutputData();
		// [6, 7]
		void adjustMemoryPointersAndClearOutputMemory(); 

		void setBinDelayWithNewSampleRate(int sampleRate);
		void createHanningWindow();

	private:
		int //numFFTs = 2,
			startIndex,
			endIndex,
			channel,
			delayLength = 10;


		// Complexe buffers voor de fft berekeningen
		dsp::Complex<float> * timeData;
		dsp::Complex<float> * spectralDataIn;
		dsp::Complex<float> * spectralDataOut;

		// input en ouput memories
		ForwardCircularDelay outputMemory;
		ForwardCircularDelay inputMemory;
		int inputForFFTCounter = 0;

		std::vector<float> hanningWindow;

		// DE FFT PERFORM FUNCTIE ACCEPTEERT ALLEEN FLOATS
		dsp::FFT *fftFunctionP;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OverlapFFT);

	public: BinDelay binDelay;
};