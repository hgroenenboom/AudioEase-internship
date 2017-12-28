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
		fftOrder = 8,
		fftSize = 1 << fftOrder,
		numBins = 20,
		numOverlaps = 1 << 3,
		maxDelInSec = 1,

		// delay in overlaps
		delRangeShort = 100,
		delRangeLong = 1000
	};
};
