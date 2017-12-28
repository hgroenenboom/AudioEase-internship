
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "MultiSlider.h"

//==============================================================================
/**
*/
class FftbinDelayAudioProcessorEditor  : public AudioProcessorEditor
										, public ChangeListener
										, public Button::Listener
										, public Slider::Listener
{
public:
    FftbinDelayAudioProcessorEditor (FftbinDelayAudioProcessor&);
    ~FftbinDelayAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

	// overrided abstract functions
	void changeListenerCallback(ChangeBroadcaster* source) override;
	void sliderValueChanged(Slider* slider) override;

	// button functions
	void refreshButtons();
	void buttonClicked(Button* button) override;
	void playButtonClicked();
	void openButtonClicked();
	void bypassButtonClicked();
	void rangeButtonClicked();
	void refreshRangeButton();

	// slider functions
	void newFeedbackSliderValue();
	void refreshSliders();

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	FftbinDelayAudioProcessor& processor;

	int nButtons = 4;
	TextButton openButton;
	TextButton playStopButton;
	TextButton bypassButton;

	TextButton delayRangeButton;
	int rangeHigh = 1000;
	int rangeLow = 100;

	Slider feedbackSlider;

	MultiSlider mSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FftbinDelayAudioProcessorEditor)
};
