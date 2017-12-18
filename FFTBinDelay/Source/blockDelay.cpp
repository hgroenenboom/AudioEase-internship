#include "blockDelay.h"

//BlockDelay::BlockDelay() {}

ForwardCircularDelay::ForwardCircularDelay(int delaySizeInSamples, int delay)
	: memSize(delaySizeInSamples)
{
	delayBuffer.resize(delaySizeInSamples); //initialize buffersize
	fill(delayBuffer.begin(), delayBuffer.end(), 0); //zerofill buffer

	currentPosition = 0;
	writePointer = 0;
	readPointer = (-1 * delay + memSize) % memSize;

	delayInSamples = delay;
}


// read a value offsetted from the read pointer. default reads the current delay sample
float ForwardCircularDelay::readValue(int index) {
	return delayBuffer[(readPointer + index + memSize) % memSize];
};

// write a value offsetted from the write pointer. default write the current write value location.
void ForwardCircularDelay::overwriteValue(float value, int index, float feedback) {
	delayBuffer[(writePointer + index + memSize) % memSize] *= feedback;
	delayBuffer[(writePointer + index + memSize) % memSize] += value;
};

// adjust the pointers if a delay callback is complete (if the delay has written and read numSteps times).
void ForwardCircularDelay::adjustPointers(int numSteps) {
	currentPosition += numSteps;
	currentPosition %= memSize;
	writePointer = currentPosition;
	readPointer = (currentPosition - delayInSamples + memSize) % memSize;
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
void ForwardCircularDelay::pushNewInput(float sample) {
	writePointer %= memSize;
	delayBuffer[writePointer] = sample;
	writePointer++;
}

// read outputValues without moving the readPointer
float ForwardCircularDelay::readOutputValue(int index) {
	return delayBuffer[(readPointer + index + memSize) % memSize];
}

// add the inserted input to the existing buffervalue
void ForwardCircularDelay::addInsertInput(int index, float sample){
	delayBuffer[(writePointer + index + memSize) % memSize] += sample;
}