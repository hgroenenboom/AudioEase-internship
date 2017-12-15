#pragma once

#include <iostream>
#include <vector>

#include "../JuceLibraryCode/JuceHeader.h"

using namespace std;

class forwardBlockDelay {
	public:
		forwardBlockDelay(int blockLengthInSamples = 512, int amountOfBlocksInBuffer = 300, int delay = 2);

		// zero fill a part of the delayBuffer using offsets from the currentPosition.
		void clearBufferData(int startOffset, int endOffset);
		// push new input into the delayBuffer and move the readpointer. Use this function inside your callback.
		void pushNewInput(float sample);
		// read a buffer value. The index is a offset from the currentPosition.
		float readOutputValue(int index);
		// adjust the pointers after de delay has received blockSize samples.
		void adjustPointers(int bufferSize);
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
		int delayInBlocks;

		// delay location markers.
		int currentPosition;
		int readPointer = 0;
		int writePointer = blockSize;

};