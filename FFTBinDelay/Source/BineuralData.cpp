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

	tDbinData = new float[tDdatalength / sizeof(float)];
	fDbinData = new float[BinaryData::fdBinauralData_binSize / sizeof(float)];

	char * temp = (char*)tDbinData.get();
	for (int i = 0; i < tDdatalength / sizeof(float); i++) {
		for (int c = 0; c < sizeof(float); c++) {
			temp[i * sizeof(float) + c] = charBuffer[i * sizeof(float) + c];
		}
	}

	char * temp2 = (char*)fDbinData.get();
	for (int i = 0; i < fDdatalength / sizeof(float); i++) { // loop door de hoeveelheid float waardes
		for (int c = 0; c < sizeof(float); c++) { // loop through the individual chars
			temp2[i * sizeof(float) + c] = BinaryData::fdBinauralData_bin[i * sizeof(float) + c];
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
			//printArray(tDbinData, tDdatalength / sizeof(float), startIndex + binSize, startIndex, to_string(i * 15) + chan + " \t = ");
			//DBG("Length = " << tDdatalength << " and startIndex = " << startIndex);
		}
	}

	return tDbinData;
}