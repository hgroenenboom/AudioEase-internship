#include "blockDelay.h"

//BlockDelay::BlockDelay() {}

forwardBlockDelay::forwardBlockDelay(int blockLengthInSamples, int amountOfBlocksInBuffer, int delay)
	: NUMBLOCKS(amountOfBlocksInBuffer)
	, blockSize(blockLengthInSamples)
	, memSize(NUMBLOCKS * blockSize)
{
	delayBuffer.resize(blockLengthInSamples * NUMBLOCKS); //initialize buffersize
	fill(delayBuffer.begin(), delayBuffer.end(), 0); //zerofill buffer

	currentPosition = 0;
	writePointer = 0;
	readPointer = (-1 * delay * blockSize + memSize) % memSize;

	delayInBlocks = delay;
}

void forwardBlockDelay::clearBufferData(int startOffset, int endOffset) {
	for (int i = startOffset; i < endOffset; i++) {
		delayBuffer[(i + currentPosition + memSize) % memSize] = 0.0f;
	}
}

// push newInput into the delayBuffer and move writePointer
void forwardBlockDelay::pushNewInput(float sample) {
	writePointer %= memSize;
	delayBuffer[writePointer] = sample;
	writePointer++;
}

// read outputValues without moving the readPointer
float forwardBlockDelay::readOutputValue(int index) {
	float outValue = delayBuffer[(readPointer + index + memSize) % memSize];
	return outValue;
}

// call adjustPointers if a new buffer block is ready
void forwardBlockDelay::adjustPointers(int bufferSize) {
	currentPosition += bufferSize;
	currentPosition %= memSize;
	writePointer = currentPosition;
	readPointer = (currentPosition - delayInBlocks * bufferSize + memSize) % memSize;
}

// add the inserted input to the existing buffervalue
void forwardBlockDelay::addInsertInput(int index, float sample){
	delayBuffer[(writePointer + index + memSize) % memSize] += sample;
}