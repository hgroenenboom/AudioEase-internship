#pragma once

#include <iostream>
#include <vector>

#include "../JuceLibraryCode/JuceHeader.h"

using namespace std;

class ForwardCircularDelay {
	public:
		ForwardCircularDelay(int delaySizeInSamples = 44100, int delay = 512);

		// GENERAL FUNCTIONS
		//
		// read a value offsetted from the read pointer.
		float readValue(int index = 0);
		// write a value offsetted from the write pointer.
		void overwriteValue(float value, int index = 0, float feedback = 0.5);
		// adjust the pointers after dthe delay has received the expected amount of samples.
		void adjustPointers(int bufferSize = 1);

		// SPECIFICALLY USED FUNCIONS:
		//
		// zero fill a part of the delayBuffer using offsets from the currentPosition.
		void clearBufferData(int startOffset, int endOffset);
		// push new input into the delayBuffer and move the readpointer. Use this function inside your callback.
		void pushNewInput(float sample);
		// read a buffer value. The index is a offset from the currentPosition.
		float readOutputValue(int index);
		// add the sample to the current buffer value. The corresponding location is index + currentPosition
		void addInsertInput(int index, float sample);

	private:
		//The buffer used by the delay
		std::vector<float> delayBuffer;

		// delay constants
		int NUMBLOCKS;
		int blockSize;
		int memSize;

		// delay length in blocks.
		int delayInSamples;

		// delay location markers.
		int currentPosition;
		int readPointer = 0;
		int writePointer = blockSize;

};