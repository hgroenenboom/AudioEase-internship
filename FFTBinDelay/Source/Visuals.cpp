/*
  ==============================================================================

    Visuals.cpp
    Created: 25 Jan 2018 3:49:05pm
    Author:  Harold

  ==============================================================================
*/

#include "Visuals.h"

Visual::Visual() :
	center(getWidth() * 0.5f, getHeight() * 0.5f)
{
	startTimer(40);
}

void Visual::paint(Graphics& g) {
	if (!setFirstPaint) {
		g.setColour(Colour(0.0f, 0.0f, 0.0f, 0.9f));
		center.x = getWidth() * 0.5f;
		center.y = getHeight() * 0.5f;
		cDiam = getHeight() * 0.0166667f;
	}
	else {
		//g.restoreState();
		g.setColour(Colours::black);
		g.fillRect(0.0f, 0.0f, (float)getWidth(), (float)getHeight());
		setFirstPaint = false;
	}

	auto s = getLocalBounds();
	g.fillRect(0.0f, 0.0f, (float)getWidth(), (float)getHeight());
	for (int i = 0; i < mVar::nBands; i++) {
		//auto p = new Rectangle<float>(i * ((float)getHeight() / mVar::nBands), i * ((float)getHeight() / mVar::nBands), 5.0f, 5.0f);
		float phi = (par::panLocation[i] * 2.0f + 0.5f) * float_Pi;
		float x = cos(phi);
		float y = sin(phi) * 0.1f + 0.45f;
		float r = (1.0f - par::ampArray[i]) * (float)getHeight() * 0.5f;
		for (int j = 1; j < 3; j++) {
			float diam = cDiam * j * (abs(par::panLocation[i] - 0.5f) + 0.5f);
			g.setColour(Colour(i * 0.05f, 0.8f, 0.8f, (1.0f - par::ampArray[i]) * (1.0f / (float)(j+1)) ));
			g.drawEllipse(center.x + x * r - diam * 0.5f, 
				y * r - diam * 0.5f + par::delayArray[i] * (getHeight()),
				diam, diam, 2.0f);
		}

		/*
		for (int j = 0; j < 2; j++) {
			float phi = ((par::panLocation[i] + (0.5f - par::panSpeed[i]) * 0.1f * j) * 2.0f + 0.5f) * float_Pi;
			float tempx = cos(phi);
			float tempy = sin(phi);
			g.setColour(Colour(i * 0.05f, 0.8f, 0.8f, (1.0f - par::ampArray[i]) * (0.5f / (float)(j+1)) ));
			g.drawEllipse(center.x + tempx * r - 0.5f * cDiam, center.y + tempy * r - 0.5f * cDiam, cDiam, cDiam, 0.5f);
		}
		*/
	}
	//g.saveState();
}

void Visual::resized() {

}

void Visual::timerCallback() {
	repaint();
}