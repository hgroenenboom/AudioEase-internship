#include "blockDelay.h"

//BlockDelay::BlockDelay() {}

ForwardCircularDelay::ForwardCircularDelay(int delaySizeInBlocks, int delayInBlocks, bool delayTimeIsBufferLength, int valuesPerBlock)
	: MEMSIZE(delaySizeInBlocks * valuesPerBlock),
	delayTimeIsBufferLength(delayTimeIsBufferLength),
	valuesPerBlock(valuesPerBlock),
	delayInBlocks(delayInBlocks)
{
	//initialize buffersize
	delayBuffer = (double *) malloc(sizeof(double) * MEMSIZE);
	//zerofill buffer
	memset(delayBuffer, 0.0, MEMSIZE * sizeof(double));
	DBG("allocated buffer: ");
	//Initialize delay parameters:
	setDelayTime(delayInBlocks);

	//Initialize delay pointers:
	currentPosition = 0;
	writePointer = 0;
	readPointer = (-1 * delayInValues + delayModulo) % delayModulo;
}

ForwardCircularDelay::~ForwardCircularDelay() {
	//free(delayBuffer);
	DBG("freeing the delayBuffer causes errors");
}


// read a value offsetted from the read pointer. default reads the current delay sample
float ForwardCircularDelay::readValue(int index) {
	return delayBuffer[(readPointer + index + delayModulo) % delayModulo];
};

void ForwardCircularDelay::feedBack(int index, float feedback) {
	delayBuffer[(readPointer + index + delayModulo) % delayModulo] *= (feedback * feedbackControl);
}

// write a value offsetted from the write pointer. default write the current write value location.
void ForwardCircularDelay::addValue(float value, int index) {
	delayBuffer[(writePointer + index + delayModulo) % delayModulo] += value;
};

// adjust the pointers if a delay callback is complete (if the delay has written and read numSteps times).
void ForwardCircularDelay::adjustPointers(int numBlocks) {
	currentPosition += numBlocks * valuesPerBlock;
	currentPosition %= delayModulo;

	writePointer = currentPosition;
	readPointer = (currentPosition - delayInValues + delayModulo) % delayModulo;
}

void ForwardCircularDelay::setDelayTime(int delayInBlocks) {
	//DBG("New delayTime in blocks: " << delayInBlocks);
	this->delayInBlocks = delayInBlocks;
	delayInValues = delayInBlocks * valuesPerBlock;

	if (delayTimeIsBufferLength) {
		delayModulo = MEMSIZE;
	}
	else if(delayInBlocks <= 0) {
		delayInBlocks = 1;
		DBG("Delay <= 0 !!!");
		//feedbackControl = 0.0f;
		delayInValues = delayInBlocks * valuesPerBlock;
		delayModulo = MEMSIZE;
	}
	else if (delayInValues > MEMSIZE) {
		DBG("delay size out of range. ");
		delayModulo = MEMSIZE;
	} else {
		//DBG("New delay value. ");
		delayModulo = delayInValues;
	} 
}


void ForwardCircularDelay::resizeBuffer(int sizeinblocks) {
	free(delayBuffer);
	
	int memSize = sizeinblocks;
	//initialize buffersize
	delayBuffer = (double *)malloc(sizeof(double) * memSize);
	//zerofill buffer
	memset(delayBuffer, 0.0, memSize * sizeof(double));
	//DBG("allocated buffer: ");
}


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////  SPECIFICALLY USED FUNCTIONS //////////////////////////////////////////

// clear data from the delayBuffer by inputting the offset from the currentPosition
void ForwardCircularDelay::clearBufferData(int startOffset, int endOffset) {
	for (int i = startOffset; i < endOffset; i++) {
		delayBuffer[(i + currentPosition + delayModulo) % delayModulo] = 0.0f;
	}
}

// push newInput into the delayBuffer and move writePointer
void ForwardCircularDelay::pushSingleValue(float sample) {
	delayBuffer[writePointer] = sample;
	writePointer++;
	writePointer %= delayModulo;
}