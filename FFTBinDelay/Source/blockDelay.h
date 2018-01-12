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
		void applyFeedback(int index = 0, float feedback = 0);
		// write a value offsetted from the write pointer.
		void addSample(float value, int index = 0);
		// adjust the pointers after the delay has received the expected amount of samples.
		void adjustDelayCentre(int numSteps = 1);
		// setDelayTime;
		void setDelayTime(float delayTime);

		void resizeBuffer(int sizeInBlocks);

		// SPECIFICALLY USED FUNCIONS:
		//
		// zero fill a part of the delayBuffer using offsets from the currentPosition. 
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

	private:
		//The resizable buffer used by the delay
		float * delayBuffer;

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
		int currentPosition;
		int readPointer = 0;
		int writePointer = 0;

};