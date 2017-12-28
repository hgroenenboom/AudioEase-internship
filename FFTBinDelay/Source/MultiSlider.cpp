
#include "MultiSlider.h"

MultiSlider::MultiSlider(FftbinDelayAudioProcessor& p)
	: processor(p)
{
	// Init delay values
	for (int i = 0; i < MainVar::numBins; i++) {
		delaySliderValues[i] = 1.0;
	}
}

void MultiSlider::paint(Graphics& g) {
	g.setColour(Colours::grey);
	g.drawRect(getLocalBounds());

	g.setColour(Colours::white);
	for (int i = 0; i < MainVar::numBins; i++) {
		g.drawRect(i * (getWidth() / MainVar::numBins)
			, (int) (delaySliderValues[i] * getHeight())
			, getWidth() / MainVar::numBins
			, 1);
	}
};

void MultiSlider::resized() {
}

void MultiSlider::mouseEnter(const MouseEvent& event) {
	mouseIsInsideComponent = true;
};

void MultiSlider::mouseExit(const MouseEvent& event) {
	mouseIsInsideComponent = false;
};

void MultiSlider::mouseDrag(const MouseEvent& event) {
	reactToMouseValues(event);
};

void MultiSlider::mouseDown(const MouseEvent& event) {
	reactToMouseValues(event);
}

void MultiSlider::reactToMouseValues(const MouseEvent& event) {
	if (mouseIsInsideComponent) {
		int yPixel = max(min(event.getPosition().getY(), getHeight()), 0);
		int index = event.getPosition().getX() / (getWidth() / MainVar::numBins);
		index = min(max(index, 0), MainVar::numBins - 1);

		delaySliderValues[index] = (float)yPixel / getHeight();
		setBinDelayTimeValue(index, 
			min(1.0f, max(0.0f, (delaySliderValues[index])) )
		);

		repaint();
	}
};

void MultiSlider::setBinDelayTimeValue(int index, float value) {
	processor.setBinDelayTime(index, value);
}

const float* MultiSlider::getSliderValues() const {
	return delaySliderValues;
}

void MultiSlider::refreshGUIValues(const float* newValues) {
	for (int i = 0; i < MainVar::numBins; i++) {
		delaySliderValues[i] = newValues[i];
		//DBG("new value " << delaySliderValues[i]);
	}
	repaint();
}

void MultiSlider::refreshDataValues() {
	for (int i = 0; i < MainVar::numBins; i++) {
		setBinDelayTimeValue(i, delaySliderValues[i]);
	}
}