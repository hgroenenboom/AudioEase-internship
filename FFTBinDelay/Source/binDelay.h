
#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

#include "MainVar.h"
#include "blockDelay.h"

#include "../JuceLibraryCode/JuceHeader.h"

using namespace std;

class BinDelay {
	public:
		BinDelay::BinDelay(int sizeInBlocks);
		void createIndexArray();

		void writeIntoBinDelay(const dsp::Complex<float>* inputFFT);
		void adjustPointers();
		void feedbackAndReadFromBinDelay(dsp::Complex<float> * writeFFT);

		void newBufferSize(int sizeinblocks);

		void setDelayTime(int index, float value);
		void setFeedback(float feedbck) { this->feedback = feedbck; }
		float getFeedback() { return feedback; }

		bool phaseInDelay = true;
	private:
		//ofstream myfile;

		// indexing parameters
		int linInc = MainVar::numBins / 4;
		int dubLinInc = MainVar::numBins / 2;

		ScopedPointer<ForwardCircularDelay> delays[MainVar::numBins];
		int indexArray[MainVar::numBins];
		int numBinsArray[MainVar::numBins];

		float feedback = 0.8;

		// [3.2]
		void carToPol(float* inReOutM, float* inImOutPhi) {
			float re = *inReOutM;
			float im = *inImOutPhi;

			// phytagoras calculation
			*inReOutM = pow(pow(re, 2.0f) + pow(im, 2.0f), 0.5f);
			*inImOutPhi = atan2(im, re);
		}

		// [3.3]
		void polToCar(float* inMOutRe, float* inPhiOutIm) {
			float mag = *inMOutRe;
			float phi = *inPhiOutIm;

			*inMOutRe = mag * cos(phi);
			*inPhiOutIm = mag * sin(phi);
		}
};
