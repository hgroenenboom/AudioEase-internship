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
	void convolve(AudioSampleBuffer& inbuf, long bufferlength, float panlocation, float panincrement)
	{
		// Tijdelijke oplossing.
		
		/*if (panlocation >= 0.0f && panlocation <= 1.0f) {
			newPan = panlocation;
			usedPan = newPan * 24;
		}

		float increment = 0.0f;
		if (newPan != oldPan) 
			increment = (newPan - oldPan) / (float)bufferlength;*/

		float increment = panincrement / bufferlength;
		newPan += panincrement;
		modulo1Float(&newPan);

		size_t s, k;
		float weigth1, weigth2;

		float storedPanForMultiChannelCallback = usedPan;
		for (int c = 0; c < 2; c++) {
			usedPan = storedPanForMultiChannelCallback;

			const float* channelData = inbuf.getReadPointer(c);
			float* outBuffer = inbuf.getWritePointer(c);

			inputMemory[c].pushData(channelData, bufferlength);

			iR1indx = 512 * (int)multPan + 256 * c;
			iR2indx = 512 * (((int)multPan + 1) % 24) + 256 * c;
	

			// Loop door de samples heen
			for (s = 0; s < bufferlength; s++)
			{
				if (increment != 0.0f) {
					fade(increment);
					//DBG(weigth);
				}
				weigth = multPan - (float)(int)multPan;
				iR1indx = 512 * (int)multPan + 256 * c;
				iR2indx = 512 * (((int)multPan + 1) % 24) + 256 * c;
				weigth2 = cos(weigth * 0.5 * PI);
				weigth1 = cos((1 - weigth) * 0.5 * PI);
				
				currentValue = 0.0f;

				// i is de samplewaarde die uitgelezen wordt.
				for (int i = 0; i < 256; i++) {
					//currentValue += inputMemory[c].readSample(s - i) * interpolate(binData.binData, weigth2, weigth1, i);
					currentValue += inputMemory[c].readSample(s - i) * interpolate(binData.binData, 1.0f - weigth, weigth, i);
				}

				outBuffer[s] = currentValue;
			}

			inputMemory[c].adjustDelayCentre(bufferlength);
		}

		usedPan = newPan;
		oldPan = newPan;
	}

private:
	ForwardCircularDelay inputMemory[2];
	BinauralData binData;
	const int bufferSize;

	const float PI = float_Pi;

	int iR1indx = 0, iR2indx = 0;
	float newPan = 0.5f, weigth = 0.0f, currentValue = 0.0f, oldPan = 0.5f, usedPan = newPan, multPan = 12.0f;

	void fillArray(float* arrayToFill, float* data, int startIndex, int length) 
	{
		for (int i = 0; i < length; i++) {
			arrayToFill[i] = data[startIndex + i];
		}
	}

	float interpolate(float* IRs, float wgth1, float wgth2, int index)
	{
		float out = 0.0f;
		out = IRs[iR1indx + index] * wgth1
			+IRs[iR2indx + index] * wgth2;
		return out;
	}

	void fade(float increment) {
		usedPan += increment;
		modulo1Float(&usedPan);

		multPan = usedPan * 24.0f;
	}



	template <class numType>
	void printArray(numType* array, int sizeInformation, int endIndex, int startIndex = 0, std::string prefix = "", int maxPrintAmount = 50)
	{
		std::string strng = "";
		std::string end = "";

		int arrayLen = sizeInformation;
		int runLength = endIndex - startIndex;

		if (runLength > 0 && arrayLen >= endIndex) {
			if (runLength > maxPrintAmount) {
				runLength = maxPrintAmount;
				end = ", etc....";
			}

			for (int i = 0; i < runLength - 1; i++) {
				auto t = std::to_string(array[startIndex + i]);

				// clip to 15 chars per input sample
				if (t.size() > 15) {
					t = t.substr(0, 15);
					strng += t + ".. , ";
				}
				else {
					strng += t + ", ";
				}

				if ((i + 1) % 10 == 0) strng += "\n";
			}

			auto t = std::to_string(array[startIndex + runLength - 1]);
			strng += t;

			DBG("\nPrint array: " << prefix << strng << end);
		}
	}

	void modulo1Float(float* value) {
		if (*value > 0.0f || *value < 1.0f) {
			if (*value < 0.0f) {
				*value -= (float)(int)*value - 1;
			}
			*value = *value - (float)(int)*value;
		}
	}
};