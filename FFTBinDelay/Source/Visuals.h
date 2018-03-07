/*
  ==============================================================================

    Visuals.h
    Created: 25 Jan 2018 3:49:05pm
    Author:  Harold

  ==============================================================================
*/

#pragma once
#include "PluginProcessor.h"
#include "../JuceLibraryCode/JuceHeader.h"

class Visual : public Component,
	private Timer
{
public:
	Visual();

	void paint(Graphics& g) override;
	void resized() override;

	void timerCallback() override;

	Point<float> center;
	float cDiam = 5.0f;
	bool setFirstPaint = true;
};