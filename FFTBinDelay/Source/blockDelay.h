#pragma once

#include <iostream>
#include <vector>

#include "../JuceLibraryCode/JuceHeader.h"

using namespace std;

class ForwardCircularDelay {
	public:
		// The forward circular delay is a delay in samples that works with blocks. These blocks are not recognizible in the data, 
		// but functions like adjust pointers will automaticly adjust the pointers in multiples of the blockSize.
		// The constructor expects the amount of !blocks! used in the delay, the delay value in !blocks! and the amount of !samples! inside a !block!.
		// The correct order op operation: feedback, read, write, adjust pointers. Changing the order is risky.
		ForwardCircularDelay(int delaySizeInBlocks = 44100, int delayInBlocks = 512, bool delayTimeIsBufferLength = false, int samplesPerBlock = 1);
		~ForwardCircularDelay();


		// GENERAL FUNCTIONS
		//
		// read a value offsetted from the read pointer.
		float readSample(int index = 0);

		float* getReadSampleAdress(int index = 0) {
			return &delayBuffer[(readPos + index + delayModulo) % delayModulo];
		};
		// applyFeedback on the readpointer
		void applyFeedback(int index = 0, float feedback = 0);
		// write a value offsetted from the write pointer.
		float addSample(float value, int index = 0);
		// adjust the pointers after the delay has received the expected amount of samples.
		void adjustDelayCentre(int numSteps = 1);

		// Voor de duidelijkhied over default values:
		//void adjustDelayCentre();
		//void adjustDelayCentre(int numSteps);

		// setDelayTime;
		void setDelayTime(float delayTime);

		void resizeBuffer(int sizeInBlocks);

		// SPECIFICALLY USED FUNCIONS:
		//
		// zero fill a part of the delayBuffer using offsets from the currentPos. 
		void clearBufferData(int startOffset, int endOffset);
		// push new input into the delayBuffer and move the readpointer. Use this function inside your callback.
		void pushSample(float sample);
		// 
		void sortByTime();
		float* getTimeSortedVersion() const;

		void pushData(float* data, int dataLength);
		void pushData(const float* data, int dataLength);

		float* getBufferData() const;
		float* getTimeSortedBufferData() const;
		int getMemSize() const;

		float* getSamPnterFromCenPos(int offset = 0) {
			return &delayBuffer[(currentPos + offset + delayModulo) % delayModulo];
		}

		float* getSamPnterFromReadPos(int offset = 0) {
			return &delayBuffer[(readPos + offset + delayModulo) % delayModulo];
		}

		void applyFeedbackOnWritePos(float fdback, int offset = 0) {
			delayBuffer[(writePos + offset + delayModulo) % delayModulo] *= (fdback *feedbackControl);
		}

		float* getWriteSampleAdress(int index = 0) {
			return &delayBuffer[(writePos + index + delayModulo) % delayModulo];
		};

	private:
		//The resizable buffer used by the delay
		ScopedPointer<float> delayBuffer;

		// delay constants
		bool delayTimeIsBufferLength;
		int memsizeInSamples;

		float feedback = 0.0;
		float feedbackControl = 1.0f; // used to kill the sound if irresponsible values are presented

		// delay length variables.
		int delayInBlocks;
		int delayInSamples;
		int delayModulo;
		int samplesPerBlock = 1;

		// delay location markers.
		int currentPos;
		int readPos = 0;
		int writePos = 0;

};

class Fade {
public:

	void initFade(float *oldval, float targetVal, int numcalculations) {
		if (*oldval != targetVal) {
			startValue = *oldval;
			endValue = targetVal;
			numCalculations = numcalculations;
			increment = (endValue - startValue) / (float)numCalculations;
			count = 0;
			value = oldval;
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