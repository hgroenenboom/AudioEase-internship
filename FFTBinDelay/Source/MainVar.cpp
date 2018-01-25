/*
  ==============================================================================

    MainVar.cpp
    Created: 24 Jan 2018 3:07:25pm
    Author:  Harold

  ==============================================================================
*/

#include "MainVar.h"

namespace par {
	AudioParameterFloat* dryWet = new AudioParameterFloat("wetd", "Wet Dry", 0.0f, 1.0f, 0.0f);
	AudioParameterFloat* feedBack = new AudioParameterFloat("feedb", "Feedback", 0.0f, 1.0f, 0.6f);

	float delayArray[MainVar::numBands];
	float ampArray[MainVar::numBands];
}