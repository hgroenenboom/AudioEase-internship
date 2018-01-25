
#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

#include "MainVar.h"
#include "blockDelay.h"
#include "BineuralData.h"
#include "../JuceLibraryCode/JuceHeader.h"

using namespace std;

class BinDelay {
	public:
		BinDelay::BinDelay(int sizeInBlocks, int chan);
		void createIndexArray();

		void writeIntoBinDelay(const dsp::Complex<float>* inputFFT);
		void adjustPointers();
		void feedbackAndReadFromBinDelay(dsp::Complex<float> * writeFFT);

		void newBufferSize(int sizeinblocks);

		void setDelayTime(int index, float value);
		void setFeedback(float feedbck) { this->feedback = feedbck; }
		float getFeedback() { return feedback; }

		bool phaseInDelay = true;

		void setPanLocations(float* panarry) {
			panLocations = panarry;
		}

		//float ampsBinsPerBand[MainVar::numBands];
	private:
		//ofstream myfile;

		// indexing parameters
		int linInc = MainVar::numBands / 4;
		int dubLinInc = MainVar::numBands / 2;

		ScopedPointer<ForwardCircularDelay> delayBands[MainVar::numBands];
		int indexArray[MainVar::numBands];
		int numBinsPerBand[MainVar::numBands];

		int chan = 0;
		float* panLocations;

		float feedback = 0.5f;

		// [3.2]
		void carToPol(float* inReOutM, float* inImOutPhi) {
			float re = *inReOutM;
			float im = *inImOutPhi;

			// phytagoras calculation
			//*inReOutM = pow(pow(re, 2.0f) + pow(im, 2.0f), 0.5f);
			*inReOutM = sqrt(re * re + im * im);
			*inImOutPhi = atan2(im, re);
		}

		// [3.3]
		void polToCar(float* inMOutRe, float* inPhiOutIm) {
			float mag = *inMOutRe;
			float phi = *inPhiOutIm;

			*inMOutRe = mag * cos(phi);
			*inPhiOutIm = mag * sin(phi);
		}

		float readFromBinData(int index, int bin, int re0im1) {
			return bindata.fDbinData[(int)(panLocations[index] * 24.0f) * 2 * MainVar::fftSize + chan*MainVar::fftSize + (indexArray[index] + bin) * 2 + re0im1];
		}

		float interpolateFromBinData(int index, int bin, int re0im1, int nBins) {
			float returnVal = 0.0f;
			for (int i = 0; i < nBins; i++) {
				if (indexArray[index] + bin >= 0) {
					returnVal += readFromBinData(index, bin, re0im1) * sin(float_Pi * i / (float)nBins);
				}
			}
			return returnVal;
		}

public: BinauralData bindata;

};
