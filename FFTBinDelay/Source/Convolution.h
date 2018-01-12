/*
  ==============================================================================

    Convolution.h
    Created: 9 Jan 2018 11:01:55am
    Author:  Harold

  ==============================================================================
*/

#pragma once

#include "blockDelay.h"
#include "BineuralData.h"

class ConvolveTimeDomainForCallbacks {
	//IR = 256
	//inbuf = max 1024

public:
	ConvolveTimeDomainForCallbacks(int bufferSize);

	// werkt met audio callbacks. 
	void convolve(AudioSampleBuffer& inbuf, long bufferlength, float panlocation)
	{
		// Tijdelijke oplossing.
		if (panlocation >= 0.0f && panlocation <= 1.0f) {
			savedPan = panlocation;
			usedPan = savedPan * 24;
		}

		for (int c = 0; c < 2; c++) {
			const float* channelData = inbuf.getReadPointer(c);
			float* outBuffer = inbuf.getWritePointer(c);

			inputMemory[c].pushData(channelData, bufferlength);

			size_t s, k;
			weigth = usedPan - (float)(int)usedPan;
			iR1indx = 512 * (int)usedPan + 256 * c;
			iR2indx = 512 * (((int)usedPan + 1) % 24) + 256 * c;
			
			float increment = 0.0f;
			if(savedPan != oldPan) increment = (oldPan - savedPan) / bufferlength;

			// Loop door de samples heen
			for (s = 0; s < bufferlength; s++)
			{
				if (oldPan != savedPan) {
					//fade(increment, c);
				}
				
				currentValue = 0.0f;

				// i is de samplewaarde die uitgelezen wordt.
				for (int i = 0; i < 256; i++) {
					currentValue += inputMemory[c].readSample(s - i) * interpolate(binData.binData, weigth, i);
				}

				outBuffer[s] = currentValue;
			}

			inputMemory[c].adjustDelayCentre(bufferlength);
		}

		oldPan = savedPan;
	}

private:
	ForwardCircularDelay inputMemory[2];
	BinauralData binData;
	const int bufferSize;

	int iR1indx = 0, iR2indx = 0;
	float savedPan = 0.5f, weigth = 0.0f, currentValue = 0.0f, oldPan = 0.5f, usedPan = 12.0f;

	void fillArray(float* arrayToFill, float* data, int startIndex, int length) 
	{
		for (int i = 0; i < length; i++) {
			arrayToFill[i] = data[startIndex + i];
		}
	}

	float interpolate(float* IRs, float weigth, int index) 
	{
		float out = 0.0f;
		out = IRs[iR1indx + index] * (1.0f - weigth)
			+ IRs[iR2indx + index] * weigth;
		return out;
	}

	void fade(float increment, int channel) {
		if (channel == 0) {
			savedPan += increment;
			usedPan = savedPan * 24.0f;
		}
	}
};