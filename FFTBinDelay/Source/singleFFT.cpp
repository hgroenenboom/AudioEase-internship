#include "singleFFT.h"

FFTProcessor::FFTProcessor() 
{}

FFTProcessor::FFTProcessor(int fftSize)
{
	this->fftSize = fftSize;

	//initialize buffer size
	//inputData.resize(fftSize);


	zeromem(fftData, sizeof(fftData));
	//zeromem(inputData, sizeof(inputData));
}
