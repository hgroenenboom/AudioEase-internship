/*
  ==============================================================================

    MainVar.h
    Created: 28 Dec 2017 1:42:19pm
    Author:  Harold

  ==============================================================================
*/

#pragma once

class MainVar {
public:
	enum {
		fftOrder = 10,
		fftSize = 1 << fftOrder,
		tdSize = 1 << (fftOrder - 2),
		numBins = 20,
		numOverlaps = 1 << 1,
		maxDelInSec = 1,

		// delay in overlaps
		delRangeShort = MainVar::numOverlaps * (44100 / MainVar::fftSize) * MainVar::maxDelInSec / 2,
		delRangeLong = MainVar::numOverlaps * (44100 / MainVar::fftSize) * MainVar::maxDelInSec * 2
	};
};
