/*
  ==============================================================================

    MainVar.h
    Created: 28 Dec 2017 1:42:19pm
    Author:  Harold

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#pragma once

class MainVar {
public:
	enum {
		fftOrder = 10,
		fftSize = 1 << fftOrder,
		tdSize = 1 << (fftOrder - 2),
		numBands = 20,
		numOverlaps = 1 << 1,
		maxDelInSec = 5,

		// delay in overlaps
		delRangeShort = 25,
		delRangeLong = MainVar::numOverlaps * (44100 / MainVar::fftSize) * MainVar::maxDelInSec
	};

	//static const float temp = 10.0f;
};

namespace par {
	extern AudioParameterFloat* dryWet;
	extern AudioParameterFloat* feedBack;

	extern float delayArray[MainVar::numBands];
	extern float ampArray[MainVar::numBands];
}