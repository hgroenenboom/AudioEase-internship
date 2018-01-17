
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
	void refreshButton(TextButton& button, bool condition);

	void buttonClicked(Button* button) override;
	void changeButtonColour(TextButton &button, bool condition, bool greenIsTrueOrFals);
	void playButtonClicked();
	void openButtonClicked();
	void bypassButtonClicked();
	void rangeButtonClicked();
	void phaseInDelayButtonClicked();

	// slider functions
	void newFeedbackSliderValue();
	void refreshSliders();

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	FftbinDelayAudioProcessor& processor;

	TextButton openButton;
	TextButton playStopButton;
	TextButton mainBypass;
	TextButton oFFTBypass;
	TextButton fftBypass;
	TextButton delayRangeButton;
	TextButton phaseDelayButton;
	int nButtons = 6;

	Slider feedbackSlider;
	Slider panSlider;
	MultiSlider delaySliders;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FftbinDelayAudioProcessorEditor)
};
