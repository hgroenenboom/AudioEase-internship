/*
  ==============================================================================

    MultiSlider.cpp
    Created: 20 Dec 2017 10:56:59am
    Author:  Harold

  ==============================================================================
*/

#include "MultiSlider.h"

MultiSlider::MultiSlider(FftbinDelayAudioProcessor& p)
	: processor(p)
{

	for (int i = 0; i < numSliders; i++) {
		delayValues[i] = 1.0;

		multiSlider[i] = new Slider();
		//addAndMakeVisible( multiSlider[i]);
		//multiSlider[i]->setSliderStyle(Slider::SliderStyle::LinearVertical);
		//multiSlider[i]->addListener(this);
		//multiSlider[i]->setRange(0, 1000, int(1));
	}
}

void MultiSlider::paint(Graphics& g) {
	g.setColour(Colours::grey);
	g.drawRect(getLocalBounds());

	g.setColour(Colours::white);
	for (int i = 0; i < numSliders; i++) {
		g.drawRect(i * (getWidth() / numSliders), (int) (delayValues[i] * getHeight()), getWidth() / numSliders, 1);
	}
};

void MultiSlider::resized() {
	auto bounds = getLocalBounds();
	//bounds.reduce(20, 20);

	//auto slidersInBound = bounds;
	//for (int i = 0; i < numSliders; i++) {
	//	multiSlider[i]->setBounds(slidersInBound.removeFromLeft(bounds.getWidth() / numSliders) );
	//}
}

void MultiSlider::sliderValueChanged(Slider* slider) {
	for (int i = 0; i < numSliders; i++) {
		if (slider == multiSlider[i]) {
			//newDelaySliderValue(i);
		}
	}
}

void MultiSlider::newDelaySliderValue(int index, int value) {
	processor.setDelaySliderValue(index, value);
}