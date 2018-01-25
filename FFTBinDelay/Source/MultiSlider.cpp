
#include "MultiSlider.h"

MultiSlider::MultiSlider(FftbinDelayAudioProcessor& p, string strng, bool gridOn)
	: processor(p),
	grid(gridOn)
{
	// Init delay values
	for (int i = 0; i < MainVar::numBands; i++) {
		sliderValues[i] = 1.0;
	}

	if (strng != "") {
		addAndMakeVisible(&label);
		label.setText(strng, dontSendNotification);
		//label.attachToComponent(this, left);
	}

	
}

void MultiSlider::paint(Graphics& g) {
	ss = getLocalBounds();
	//s = ss.removeFromLeft(50);
	//ss.reduce(10, 10); 
	
	g.setColour(Colours::grey);
	g.drawRect(ss);

	if (grid == true) {
		g.setColour(Colours::yellow.withAlpha(0.33f));
		g.fillRect(0, ss.getHeight() / 2, ss.getWidth(), 1);
		g.setColour(Colours::yellow.withAlpha(0.05f));
		for (int i = 0; i < 10; i++) {
			g.fillRect(0, ss.getHeight() / 10 * i, ss.getWidth(), 1);
		}
	}

	g.setColour(Colours::white);
	for (int i = 0; i < MainVar::numBands; i++) {
		g.drawRect(i * (ss.getWidth() / MainVar::numBands)
			, (int) (sliderValues[i] * ss.getHeight())
			, ss.getWidth() / MainVar::numBands
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
		xOld = event.getPosition().getX() / (getWidth() / MainVar::numBands);
		xOld = min(max(xOld, 0), MainVar::numBands - 1);
		yOld = max(min(event.getPosition().getY(), getHeight()), 0);

		sliderValues[xOld] = (float)yOld / getHeight();
		setTargetValues(xOld, min(1.0f, max(0.0f, (sliderValues[xOld]))));

		repaint();
	}
	else {
		if (mouseIsInsideComponent) {
			// get x and y values.
			y = max(min(event.getPosition().getY(), getHeight()), 0);
			x = event.getPosition().getX() / (getWidth() / MainVar::numBands);
			x = min(max(x, 0), MainVar::numBands - 1);

			difference = x - xOld;

			if (difference >= 0) {
				for (int i = 0; i <= difference; i++) {
					float yVal;
					if (difference == 0) { yVal = (float)yOld; }
					else {
						yVal = yOld + (y - yOld) * ((float)i / difference);
					}					
					
					sliderValues[i+ xOld] = yVal / getHeight();
					setTargetValues(i+ xOld,
						min(1.0f, max(0.0f, (sliderValues[i+ xOld])))
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

					sliderValues[xOld-i] = yVal / getHeight();
					setTargetValues(xOld-i,
						min(1.0f, max(0.0f, (sliderValues[xOld-i])))
					);
					repaint();
				}
			}

			yOld = y;
			xOld = x;
		}
	}
};

void MultiSlider::setTargetValues(int index, float value) {
	//variableValues[index] = value;
	//host->multiSliderValueChanged(this, index, value);
	if (function != nullptr) {
		(function)(index, value);
	}
}

const float* MultiSlider::getSliderValues() const {
	return sliderValues;
}

void MultiSlider::refreshGUIValues(const float* newValues) {
	for (int i = 0; i < MainVar::numBands; i++) {
		sliderValues[i] = newValues[i];
		//DBG("new value " << sliderValues[i]);
	}
	repaint();
}

void MultiSlider::refreshDataValues() {
	for (int i = 0; i < MainVar::numBands; i++) {
		setTargetValues(i, sliderValues[i]);
	}
}