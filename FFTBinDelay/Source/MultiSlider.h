/*
  ==============================================================================

    MultiSlider.h
    Created: 20 Dec 2017 10:56:58am
    Author:  Harold

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

class MultiSlider : public Component
	, public Slider::Listener
{
public:
	MultiSlider(FftbinDelayAudioProcessor& p);
	void paint(Graphics& g) override;
	void resized() override;

	void sliderValueChanged(Slider* slider) override;
	void newDelaySliderValue(int index, int value);

	virtual void mouseEnter(const MouseEvent& event) override {
		mouseIsInsideComponent = true;
	};

	virtual void mouseExit(const MouseEvent& event) override {
		mouseIsInsideComponent = false;
	};

	virtual void mouseDrag(const MouseEvent& event) override {
		if (mouseIsInsideComponent) {
			int y = max(min(event.getPosition().getY(), getHeight()), 0);
			int index = event.getPosition().getX() / (getWidth() / numSliders);
			index = min(max(index, 0), 30);
			repaint();
			delayValues[index] = (float)y / getHeight();
			newDelaySliderValue(index, max(1.0f, (1 - delayValues[index]) * 1000));
		}
	};

private:

	FftbinDelayAudioProcessor& processor;
	bool mouseIsInsideComponent;

	static constexpr int numSliders = 30;
	ScopedPointer<Slider> multiSlider[numSliders];
	float delayValues[numSliders];
	int newPos;
};