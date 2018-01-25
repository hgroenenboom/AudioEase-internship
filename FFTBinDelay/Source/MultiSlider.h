
#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
//#include "PluginEditor.h"

class MultiSlider : public Component
{
public:
	MultiSlider(FftbinDelayAudioProcessor& p, string strng = "", bool gridOn = false);
	void setFunction(std::function<void(int, float)> callbackF) {
		function = callbackF;
	}

	void paint(Graphics& g) override;
	void resized() override;

	virtual void mouseEnter(const MouseEvent& event) override;
	virtual void mouseExit(const MouseEvent& event) override;
	virtual void mouseDrag(const MouseEvent& event) override;
	virtual void mouseDown(const MouseEvent& event) override;
	void reactToMouseValues(const MouseEvent& event, bool isMouseDragging);

	void setTargetValues(int index, float value);
	const float* getSliderValues() const;

	void refreshDataValues();
	void refreshGUIValues(/* const array<float, numSliders >& newDelays */ const float* newDelays);

	//void createListener(FftbinDelayAudioProcessorEditor* host) {
	//	this->host = host;
	//}
private:

	FftbinDelayAudioProcessor& processor;
	bool mouseIsInsideComponent, grid;
	Label label;

	Rectangle<int> s;
	Rectangle<int> ss;

	// amount of sliders, value is changeble. Needs to be changed in binDelay too.

	float sliderValues[MainVar::numBands];
	int newPos = 0;

	// saved mouse positions for interpolation
	int x = 0, xOld = 0;
	int y, yOld = 0;
	int difference = 0;

	//FftbinDelayAudioProcessorEditor* host;
	std::function<void(int, float)> function = nullptr;
};