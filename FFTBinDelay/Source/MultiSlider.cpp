
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
	reactToMouseValues(event, true);
};

void MultiSlider::mouseDown(const MouseEvent& event) {
	reactToMouseValues(event, false);
}

void MultiSlider::reactToMouseValues(const MouseEvent& event, bool isMouseDragging) {
	if (!isMouseDragging) {
		xOld = event.getPosition().getX() / (getWidth() / MainVar::numBins);
		xOld = min(max(xOld, 0), MainVar::numBins - 1);
		yOld = max(min(event.getPosition().getY(), getHeight()), 0);

		delaySliderValues[xOld] = (float)yOld / getHeight();
		setBinDelayTimeValue(xOld, min(1.0f, max(0.0f, (delaySliderValues[xOld]))));

		repaint();
	}
	else {
		if (mouseIsInsideComponent) {
			// get x and y values.
			y = max(min(event.getPosition().getY(), getHeight()), 0);
			x = event.getPosition().getX() / (getWidth() / MainVar::numBins);
			x = min(max(x, 0), MainVar::numBins - 1);

			difference = x - xOld;

			if (difference >= 0) {
				for (int i = 0; i <= difference; i++) {
					float yVal;
					if (difference == 0) { yVal = yOld; }
					else {
						yVal = yOld + (y - yOld) * ((float)i / difference);
					}					
					
					delaySliderValues[i+ xOld] = yVal / getHeight();
					setBinDelayTimeValue(i+ xOld,
						min(1.0f, max(0.0f, (delaySliderValues[i+ xOld])))
					);
					repaint();
				}
			}
			else {
				for (int i = 0; i <= abs(difference); i++) {
					float yVal;
					if (difference == 0) { yVal = yOld; }
					else {
						yVal = yOld + (y - yOld) * ((float)i / abs(difference) );
					}

					delaySliderValues[xOld-i] = yVal / getHeight();
					setBinDelayTimeValue(xOld-i,
						min(1.0f, max(0.0f, (delaySliderValues[xOld-i])))
					);
					repaint();
				}
			}

			yOld = y;
			xOld = x;
		}
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