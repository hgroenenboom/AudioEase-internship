
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Visuals.h"
#include "PluginProcessor.h"
#include "MultiSlider.h"

//==============================================================================
/**
*/
class FftbinDelayAudioProcessorEditor  : public AudioProcessorEditor
										, public ChangeListener
										, public Button::Listener
										, public Slider::Listener,
	private Timer
{
public:
    FftbinDelayAudioProcessorEditor (FftbinDelayAudioProcessor&);
    ~FftbinDelayAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
	void timerCallback() override;

	void buttonInit(TextButton &button, std::string str);

	// overrided abstract functions
	void changeListenerCallback(ChangeBroadcaster* source) override;
	void sliderValueChanged(Slider* slider) override;

	// button functions
	void refreshButtons();
	void refreshButton(TextButton& button, bool condition);

	void buttonClicked(Button* button) override;
	void changeButtonColour(TextButton &button, bool condition, bool greenIsTrueOrFalse = true);
	void playButtonClicked();
	void openButtonClicked();
	void bypassButtonClicked();
	void rangeButtonClicked();
	void setUseButtonClicked();

	//void multiSliderValueChanged(MultiSlider &m, int index, float value) {
	//	if (&m == &delaySliders) {
	//		processor.setBinDelayTime(index, value);
	//		DBG("lol slider");
	//	}
	//	else if (&m == &panSliders) {
	//
	//	}
	//}

	// slider functions
	void newFeedbackSliderValue();
	void refreshSliders();

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	FftbinDelayAudioProcessor& processor;
	//AudioProcessorValueTreeState processorState;

	TextButton openButton;
	TextButton micOnButton;
	TextButton playStopButton;
	TextButton mainBypass;
	TextButton oFFTBypass, fftBypass;
	TextButton muteL, muteR;
	TextButton delayRangeButton;
	TextButton setUseButton;
	int nButtons = 7;

	Slider feedbackSlider;
	Label feedbackLabel;
	Slider panSlider;
	Label panLabel;
	Slider dryWetSlider;
	Label dryWetLabel;

	MultiSlider delaySliders;
	std::function<void(int, float)> delayF = nullptr;
	Label delayLabel;
	MultiSlider panSliders;
	std::function<void(int, float)> panF = nullptr;
	Label autopanLabel;
	MultiSlider ampSliders;
	std::function<void(int, float)> ampF = nullptr;
	Label ampLabel;

	Visual visual;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FftbinDelayAudioProcessorEditor)
};
