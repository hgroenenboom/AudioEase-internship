#include "blockDelay.h"

//BlockDelay::BlockDelay() {}

ForwardCircularDelay::ForwardCircularDelay(int delaySizeInSamples, int delay, bool delayTimeIsBufferLength)
	: memSize(delaySizeInSamples),
	delayTimeIsBufferLength(delayTimeIsBufferLength)
{
	delayBuffer.resize(delaySizeInSamples); //initialize buffersize
	fill(delayBuffer.begin(), delayBuffer.end(), 0); //zerofill buffer

	delayInSamples = delay;
	if (delayTimeIsBufferLength || delay == 0) {
		delayModulo = memSize;
		feedback = 0.0f;
	} else {
		delayModulo = delay;
	}

	currentPosition = 0;
	writePointer = 0;
	readPointer = (-1 * delay + memSize) % memSize;
}


// read a value offsetted from the read pointer. default reads the current delay sample
float ForwardCircularDelay::readValue(int index) {
	delayBuffer[(readPointer + delayInSamples + index + memSize) % memSize] += delayBuffer[(readPointer + index + memSize) % memSize] * feedback;
	//delayBuffer[(readPointer + delayInSamples + index + memSize) % memSize] *= feedback;
	return delayBuffer[(readPointer + index + memSize) % memSize];
};

// write a value offsetted from the write pointer. default write the current write value location.
void ForwardCircularDelay::overwriteValue(float value, int index) {
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

void ForwardCircularDelay::setDelayTime(int delayTime) {
	delayInSamples = delayTime;
	if (delayTimeIsBufferLength || delayTime == 0 || delayTime > memSize) {
		delayModulo = memSize;
		feedback = 0.0f;
	}
	else {
		delayModulo = delayTime;
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