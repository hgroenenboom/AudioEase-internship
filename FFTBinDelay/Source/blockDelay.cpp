#include "blockDelay.h"

//BlockDelay::BlockDelay() {}

ForwardCircularDelay::ForwardCircularDelay(int delaySizeInBlocks, int delayinblocks, bool delayTimeIsBufferLength, int samplesperblock)
	: memsizeInSamples(delaySizeInBlocks * samplesperblock),
	delayTimeIsBufferLength(delayTimeIsBufferLength),
	samplesPerBlock(samplesperblock),
	delayInBlocks(delayinblocks)
{
	//initialize buffersize
	delayBuffer = (float *) malloc(sizeof(float) * (memsizeInSamples + samplesPerBlock) );
	//zerofill buffer
	memset(delayBuffer, 0.0, (memsizeInSamples + samplesPerBlock) * sizeof(float));
	DBG("allocated buffer: ");
	//Initialize delay parameters:
	setDelayTime(delayInBlocks);

	//Initialize delay pointers:
	currentPos = 0;
	writePos = 0;
	readPos = (-1 * delayInSamples + delayModulo) % delayModulo;
}

ForwardCircularDelay::~ForwardCircularDelay() 
{
	//free(delayBuffer);
	//DBG("freeing the delayBuffer causes errors");
}


// read a sample offsetted from the read pointer. default reads the current delay sample
float ForwardCircularDelay::readSample(int index) 
{
	return delayBuffer[(readPos + index + delayModulo) % delayModulo];
};

void ForwardCircularDelay::applyFeedback(int index, float feedback) 
{
	delayBuffer[(readPos + index + delayModulo) % delayModulo] *= (feedback * feedbackControl);
}

// write a sample offsetted from the write pointer. default write the current write sample location.
float ForwardCircularDelay::addSample(float sample, int index) 
{
	return delayBuffer[(writePos + index + delayModulo) % delayModulo] += sample;
};

// adjust the pointers if a delay callback is complete (if the delay has written and read numSteps times).
void ForwardCircularDelay::adjustDelayCentre(int numBlocks) 
{
	currentPos += numBlocks * samplesPerBlock;
	currentPos %= delayModulo;

	writePos = currentPos;
	readPos = (currentPos - delayInSamples + delayModulo) % delayModulo;
}

void ForwardCircularDelay::setDelayTime(float delayinblocks) 
{
	//DBG("New delayTime in blocks: " << delayinblocks);
	this->delayInBlocks = (int)delayinblocks;
	delayInSamples = this->delayInBlocks * samplesPerBlock;

	if (delayTimeIsBufferLength) {
		delayModulo = memsizeInSamples;
	}
	else if(this->delayInBlocks <= 0) {
		this->delayInBlocks = 1;
		DBG("Delay <= 0 !!!");
		feedbackControl = 0.0f;
		delayInSamples = this->delayInBlocks * samplesPerBlock;
		delayModulo = memsizeInSamples;
	}
	else if (delayInSamples > memsizeInSamples) {
		DBG("delay size out of range. ");
		delayModulo = memsizeInSamples;
	} else {
		if (feedbackControl == 0.0f) {
			feedbackControl = 1.0f;
		}
		//DBG("New delay sample. ");
		delayModulo = delayInSamples;
	} 
}


void ForwardCircularDelay::resizeBuffer(int sizeinblocks) 
{
	if (sizeinblocks * samplesPerBlock < 2097152)
	{
		free(delayBuffer);

		int MEMSIZE = sizeinblocks * samplesPerBlock;
		//initialize buffersize
		delayBuffer = (float *)malloc(sizeof(float) * sizeinblocks);
		//zerofill buffer
		memset(delayBuffer, 0.0, sizeinblocks * sizeof(float));
		//DBG("allocated new buffer to delayBuffer");
	}
}


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////  SPECIFICALLY USED FUNCTIONS //////////////////////////////////////////

// clear data from the delayBuffer by inputting the curPosOffset from the currentPos
void ForwardCircularDelay::clearBufferData(int startOffset, int endOffset) 
{
	for (int i = startOffset; i < endOffset; i++) {
		delayBuffer[(i + currentPos + delayModulo) % delayModulo] = 0.0f;
	}
}

// push newInput into the delayBuffer and move writePos
void ForwardCircularDelay::pushSample(float sample) 
{
	delayBuffer[writePos] = sample;
	writePos++;
	writePos %= delayModulo;
}

void ForwardCircularDelay::sortByTime()
{
	float* tempBuffer = new float[memsizeInSamples];

	for (int i = 0; i < memsizeInSamples; i++) {
		tempBuffer[i] = delayBuffer[i];
	}

	int curPosOffset = currentPos + memsizeInSamples;
	for (int i = 0; i < memsizeInSamples; i++) {
		delayBuffer[i] = tempBuffer[(i - curPosOffset) % memsizeInSamples];
	}

	for (int i = 0; i < memsizeInSamples; i++) {
		delayBuffer[i] = tempBuffer[i];
	}

	delete []tempBuffer;
}

float* ForwardCircularDelay::getTimeSortedVersion() const {
	float* tempBuffer = new float[memsizeInSamples];

	for (int i = 0; i < memsizeInSamples; i++) {
		tempBuffer[i] = delayBuffer[i];
	}

	int curPosOffset = currentPos + memsizeInSamples;
	for (int i = 0; i < memsizeInSamples; i++) {
		delayBuffer[i] = tempBuffer[(i - curPosOffset) % memsizeInSamples];
	}

	for (int i = 0; i < memsizeInSamples; i++) {
		delayBuffer[i] = tempBuffer[i];
	}

	return delayBuffer;
}

float* ForwardCircularDelay::getBufferData() const 
{
	return getTimeSortedVersion();
}

float* ForwardCircularDelay::getTimeSortedBufferData() const
{
	return delayBuffer;
}

int ForwardCircularDelay::getMemSize() const
{ 
	return memsizeInSamples; 
}

void ForwardCircularDelay::pushData(float* data, int dataLength)
{
	for (int i; i < dataLength; i++) {
		pushSample(data[i]);
	}
}

void ForwardCircularDelay::pushData(const float* data, int dataLength)
{
	for (int i = 0; i < dataLength; i++) {
		pushSample(data[i]);
	}
}