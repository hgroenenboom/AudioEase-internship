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

class Fade {
public:

	void initFade(float oldval, float *newval, int numcalculations) {
		if (oldval != *newval) {
			startValue = oldval;
			endValue = *newval;
			numCalculations = numcalculations;
			increment = (endValue - startValue) / (float)numCalculations;
			count = 0;
			value = newval;
			*value = oldval;
			isRunning = true;
			isActive = true;
		}
		else {
			isActive = false;
		}
	}

	void reinit() {
		if (isActive) {
			count = 0;
			*value = startValue;
			isRunning = true;
		}
	}

	void fade() {
		if (isRunning) {
			*value += increment;
			count++;
			if (count == numCalculations) {
				*value = endValue;
				count = 0;
				isRunning = false;
			}
		}
		else {
			//DBG("Fade is not active.");
		}
	}

	float startValue = 0.0f
		, endValue = 0.0f
		, increment = 0.0f;
	float* value = nullptr;
	int numCalculations = 0;
	int count = 0;
	bool isRunning = false,
		isActive = false;
};


class ConvolveTimeDomainForCallbacks {
	//IR = 256
	//inbuf = max 1024

public:
	ConvolveTimeDomainForCallbacks(int bufferSize);

	// werkt met audio callbacks. 
	void convolve(AudioSampleBuffer& inbuf, long bufferlength, float panlocation, float panincrement, float panlr, float feedbslider)
	{

		//DBG("fbslider = " << feedbslider);
		//DBG("panslider = " << panlr);

		// Tijdelijke oplossing.
		
		/*if (panlocation >= 0.0f && panlocation <= 1.0f) {
			newPan = panlocation;
			usedPan = newPan * 24;
		}

		float increment = 0.0f;
		if (newPan != oldPan) 
			increment = (newPan - oldPan) / (float)bufferlength;*/

		//float increment = panincrement / bufferlength;
		//newPan += panincrement;
		//modulo1Float(&newPan);
		//int offset = int(feedbslider * 20 - 10);

		size_t s, k;
		//float weigth1, weigth2;
		int width = 2, stepPerIr = 1;
		angle = panlr * (24.0f / (float)stepPerIr);

		speakerFade.initFade(oldAngle, &angle, bufferlength);

		float storedPanForMultiChannelCallback = usedPan;
		for (int c = 0; c < 2; c++) {
			//usedPan = storedPanForMultiChannelCallback;

			// creaters pointers to used buffers.
			const float* channelData = inbuf.getReadPointer(c);
			float* outBuffer = inbuf.getWritePointer(c);
			// push input into the memory
			inputMemory[c].pushData(channelData, bufferlength);
			
			//values for crossfading between samples.
			//float feedval = feedbslider * 16.0f - 8.0f;
			//float samOffsetWeigth = feedval - floor(feedval);
			//int samOffset = (int)floor(feedval);

			// generate IR interpolation values.
			angle = panlr * (24.0f / (float)stepPerIr);
			weigth = angle - floor(angle);
			iR1index = ( ( (int)floor(angle)*stepPerIr + c*width) % 24) * 512 + c * 256;
				//512 * 17 + 256 * c;
			iR2index = (((int)floor(angle)*stepPerIr + c*width + stepPerIr) % 24) * 512 + c * 256;
				//512 * 18 + 256 * c;
			float * ir1 = &binData.binData[iR1index];
			float * ir2 = &binData.binData[iR2index];

			speakerFade.reinit();

			//weigth2 = //cos(weigth * 0.5 * PI);
			//	weigth * 0.5;
			//weigth1 = //cos((1 - weigth) * 0.5 * PI);
			//	(1 - weigth) * 0.5;

			// Loop door de samples heen
			for (s = 0; s < bufferlength; s++)
			{
				//if (increment != 0.0f) {
				//	fade(increment);
					//DBG(weigth);
				//}


				speakerFade.fade();
				if (speakerFade.isRunning /*&& (s % 8 == 0 || s == bufferlength - 1)*/ ) {
					weigth = angle - floor(angle);
					iR1index = (((int)floor(angle)*stepPerIr + c*width) % 24) * 512 + c * 256;
						//512 * 17 + 256 * c;
					iR2index = (((int)floor(angle)*stepPerIr + c*width + stepPerIr) % 24) * 512 + c * 256;
						//512 * 18 + 256 * c;
					ir1 = &binData.binData[iR1index];
					ir2 = &binData.binData[iR2index];
				}
				//DBG(speakerFade.isRunning);
				
				currentValue = 0.0f;

				// i is de samplewaarde die uitgelezen wordt.
				for (int i = 0; i < 256; i++) {
					// lineair interpolation test: werkt
					//currentValue += inputMemory[c].readSample(s - i) * linInterp(binData.binData[iR1index + i], binData.binData[iR1index + i], weigth);
					// lineair interpolation:
					currentValue += inputMemory[c].readSample(s - i) * linInterp(ir1[i], ir2[i], weigth);

					// lineair interpolation, with a decimal index offset on the second IR value. Used to test if the phases are matching:
					//currentValue += inputMemory[c].readSample(s - i) * linInterp(binData.binData[iR1index + i], linInterp(binData.binData[iR2index + i + samOffset], binData.binData[iR2index + i + samOffset + 1], samOffsetWeigth), weigth);

				}

				outBuffer[s] = currentValue;
			}

			inputMemory[c].adjustDelayCentre(bufferlength);
		}

		//usedPan = newPan;
		//oldPan = newPan;
		oldAngle = angle;
	}

private:
	ForwardCircularDelay inputMemory[2];
	BinauralData binData;
	const int bufferSize;
	Fade speakerFade;

	ScopedPointer<float> ir1 = nullptr;

	const float PI = float_Pi;

	int iR1index = 0, iR2index = 0;
	float newPan = 0.5f, weigth = 0.0f, currentValue = 0.0f, oldPan = 0.5f, usedPan = newPan, angle = 12.0f, oldAngle = 0.0f;;

	void fillArray(float* arrayToFill, float* data, int startIndex, int length) 
	{
		for (int i = 0; i < length; i++) {
			arrayToFill[i] = data[startIndex + i];
		}
	}

	float linInterp(float val1, float val2, float weigth) {
		return val1 * (1.0f - weigth) + val2 * weigth;
	}

	void fade(float increment) {
		usedPan += increment;
		modulo1Float(&usedPan);

		angle = usedPan * 24.0f;
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

