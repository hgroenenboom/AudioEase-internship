#include "blockDelay.h"

//BlockDelay::BlockDelay() {}

ForwardCircularDelay::ForwardCircularDelay(int delaySizeInBlocks, int delayInBlocks, bool delayTimeIsBufferLength, int valuesPerBlock)
	: memSize(delaySizeInBlocks * valuesPerBlock),
	delayTimeIsBufferLength(delayTimeIsBufferLength),
	valuesPerBlock(valuesPerBlock),
	delayInBlocks(delayInBlocks)
{
	//initialize buffersize
	delayBuffer.resize(memSize); 
	//zerofill buffer
	fill(delayBuffer.begin(), delayBuffer.end(), 0); 

	delayInValues = delayInBlocks * valuesPerBlock;
	//currently unused	
	if (delayTimeIsBufferLength || delayInBlocks == 0) {
		delayModulo = memSize;
		feedback = 0.0f;
	} else {
		delayModulo = delayInBlocks;
	}

	currentPosition = 0;
	writePointer = 0;
	readPointer = (-1 * delayInValues + memSize) % memSize;
}



// read a value offsetted from the read pointer. default reads the current delay sample
float ForwardCircularDelay::readValue(int index) {
	//feedback
	//delayBuffer[(readPointer + delayInValues + index + memSize) % memSize] += delayBuffer[(readPointer + index + memSize) % memSize] * feedback;

	return delayBuffer[(readPointer + index + memSize) % memSize];
};

// write a value offsetted from the write pointer. default write the current write value location.
void ForwardCircularDelay::addValue(float value, int index) {
	//feedback
	//delayBuffer[(writePointer + index + memSize) % memSize] *= feedback;

	delayBuffer[(writePointer + index + memSize) % memSize] += value;
};

// adjust the pointers if a delay callback is complete (if the delay has written and read numSteps times).
void ForwardCircularDelay::adjustPointers(int numBlocks) {
	currentPosition += numBlocks * valuesPerBlock;
	currentPosition %= memSize;
	clearBufferData(numBlocks * valuesPerBlock, 2 * numBlocks * valuesPerBlock);

	writePointer = currentPosition;
	readPointer = (currentPosition - delayInValues + memSize) % memSize;
}

void ForwardCircularDelay::setDelayTime(int delayInBlocks) {
	this->delayInBlocks = delayInBlocks;
	delayInValues = delayInBlocks * valuesPerBlock;

	if (delayTimeIsBufferLength || delayInBlocks == 0 || delayInBlocks > memSize) {
		delayModulo = memSize;
		feedback = 0.0f;
	}
	else {
		delayModulo = delayInBlocks;
	} 
}



////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////  SPECIFICALLY USED FUNCTIONS //////////////////////////////////////////

// clear data from the delayBuffer by inputting the offset from the currentPosition
void ForwardCircularDelay::clearBufferData(int startOffset, int endOffset) {
	for (int i = startOffset; i < endOffset; i++) {
		delayBuffer[(i + currentPosition + memSize) % memSize] = 0.0f;
	}
}

// push newInput into the delayBuffer and move writePointer
void ForwardCircularDelay::pushSingleValue(float sample) {
	delayBuffer[writePointer] = sample;
	writePointer++;
	writePointer %= memSize;
}