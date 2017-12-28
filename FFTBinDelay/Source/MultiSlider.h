
#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

class MultiSlider : public Component
{
public:
	MultiSlider(FftbinDelayAudioProcessor& p);
	void paint(Graphics& g) override;
	void resized() override;

	virtual void mouseEnter(const MouseEvent& event) override;
	virtual void mouseExit(const MouseEvent& event) override;
	virtual void mouseDrag(const MouseEvent& event) override;
	virtual void mouseDown(const MouseEvent& event) override;
	void reactToMouseValues(const MouseEvent& event);

	void setBinDelayTimeValue(int index, float value);
	float* getSliderValues();

	void refreshBinDelayTimeValues();
	void refreshDelaySliderValues(float* newDelays);
private:

	FftbinDelayAudioProcessor& processor;
	bool mouseIsInsideComponent;

	// amount of sliders, value is changeble. Needs to be changed in binDelay too.
	static constexpr int nSliders = 20;

	float delaySliderValues[nSliders];
	int newPos;
};