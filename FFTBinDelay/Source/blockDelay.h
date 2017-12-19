#pragma once

#include <iostream>
#include <vector>

#include "../JuceLibraryCode/JuceHeader.h"

using namespace std;

class ForwardCircularDelay {
	public:
		ForwardCircularDelay(int delaySizeInSamples = 44100, int delay = 512, bool delayTimeIsBufferLength = false, int valuesPerStep = 1);

		// GENERAL FUNCTIONS
		//
		// read a value offsetted from the read pointer.
		float readValue(int index = 0);
		// write a value offsetted from the write pointer.
		void addValue(float value, int index = 0);
		// adjust the pointers after the delay has received the expected amount of samples.
		void adjustPointers(int numSteps = 1);
		// setDelayTime;
		void setDelayTime(int delayTime);

		// SPECIFICALLY USED FUNCIONS:
		//
		// zero fill a part of the delayBuffer using offsets from the currentPosition.
		void clearBufferData(int startOffset, int endOffset);
		// push new input into the delayBuffer and move the readpointer. Use this function inside your callback.
		void pushSingleValue(float sample);

	private:
		//The buffer used by the delay
		std::vector<double> delayBuffer;

		// delay constants
		bool delayTimeIsBufferLength;
		int memSize;

		float feedback = 0.0;

		// delay length in blocks.
		int delayInBlocks;
		int delayInValues;
		int delayModulo;
		int valuesPerBlock = 1;

		// delay location markers.
		int currentPosition;
		int readPointer = 0;
		int writePointer = 0;

};