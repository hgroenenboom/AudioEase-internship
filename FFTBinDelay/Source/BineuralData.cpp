/*
  ==============================================================================

    BineuralData.cpp
    Created: 11 Jan 2018 4:35:03pm
    Author:  Harold

  ==============================================================================
*/

#include "BineuralData.h"

float* BinauralData::readBinData(string path, int binSize) {
	DBG("Reading bindata...");

	binData = new float[datalength / sizeof(float)];
	
	char * temp = (char*)binData.get();
	for (int i = 0; i < datalength / sizeof(float); i++) {
		for (int c = 0; c < sizeof(float); c++) {
			temp[i * sizeof(float) + c] = charBuffer[i * sizeof(float) + c];
		}
	}

	for (int i = 0; i < 24; i++) {
		for (int c = 0; c < 2; c++) {
			int startIndex = (c * binSize) + (i * binSize * 2);
			char chan;
			if (c == 0) {
				chan = 'L';
			}
			else {
				chan = 'R';
			}
			//printArray(binData, datalength / sizeof(float), startIndex + binSize, startIndex, to_string(i * 15) + chan + " \t = ");
			//DBG("Length = " << datalength << " and startIndex = " << startIndex);
		}
	}

	return binData;
}