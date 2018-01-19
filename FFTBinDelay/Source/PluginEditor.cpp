/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
FftbinDelayAudioProcessorEditor::FftbinDelayAudioProcessorEditor (FftbinDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
	, delaySliders(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

	//setSize(600, 400);

	addAndMakeVisible(&delaySliders);

	addAndMakeVisible(&openButton);
	openButton.setButtonText("Open...");
	openButton.addListener(this);

	addAndMakeVisible(&playStopButton);
	playStopButton.setButtonText("Play");
	playStopButton.addListener(this);
	playStopButton.setColour(TextButton::buttonColourId, Colours::green);
	playStopButton.setEnabled(true);

	addAndMakeVisible(&feedbackSlider);
	feedbackSlider.addListener(this);
	feedbackSlider.setRange(0.0, 1.0, 0.001);

	addAndMakeVisible(&mainBypass);
	mainBypass.addListener(this);
	mainBypass.setButtonText("ByPass");

	addAndMakeVisible(&delayRangeButton);
	delayRangeButton.addListener(this);
	delayRangeButton.setButtonText("delayTime * 100");

	addAndMakeVisible(&phaseDelayButton);
	phaseDelayButton.addListener(this);
	phaseDelayButton.setButtonText("Delay phase");

	addAndMakeVisible(&oFFTBypass);
	oFFTBypass.addListener(this);
	oFFTBypass.setColour(TextButton::buttonColourId, Colours::aliceblue);
	oFFTBypass.setButtonText("Run oFFT");
	addAndMakeVisible(&fftBypass);
	fftBypass.addListener(this);
	fftBypass.setColour(TextButton::buttonColourId, Colours::aliceblue);
	fftBypass.setButtonText("Run FFT");

	buttonInit(muteL, "mute L");
	buttonInit(muteR, "mute R");

	addAndMakeVisible(&panSlider);
	panSlider.addListener(this);
	panSlider.setRange(0.0, 0.99999, 0.001);

	addAndMakeVisible(&dryWetSlider);
	dryWetSlider.addListener(this);
	dryWetSlider.setRange(0.0, 1.0, 0.001);

	refreshButtons();
	refreshSliders();

	setSize(800, 800);

	processor.transportSource.addChangeListener(this);   // zorgt ervoor dat elke change in transportSource de listener functie acti
}

FftbinDelayAudioProcessorEditor::~FftbinDelayAudioProcessorEditor()
{
}

//==============================================================================
void FftbinDelayAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("2.7.2.2 Rebuild oFFT again. The lower bins modifications create too long IR's. Fixed to do's and added dry wet slider. Next focus is convolution", getLocalBounds(), Justification::bottomRight, 1);
}

void FftbinDelayAudioProcessorEditor::resized()
{
	auto space = getLocalBounds();

	int topHeaderSize = 80;
	auto topHeader = space.removeFromTop(topHeaderSize);
	openButton.setBounds(topHeader.removeFromLeft(getWidth() / nButtons));
	playStopButton.setBounds(topHeader.removeFromLeft(getWidth() / nButtons));
	mainBypass.setBounds(topHeader.removeFromLeft(getWidth() / nButtons));

	auto spaceForFFTBypass = topHeader.removeFromLeft(getWidth() / nButtons);
	oFFTBypass.setBounds(spaceForFFTBypass.removeFromTop(topHeaderSize  - (int)(topHeaderSize / 2.5f) ));
	fftBypass.setBounds(spaceForFFTBypass);
	
	auto spaceForMuteChan = topHeader.removeFromLeft(getWidth() / nButtons);
	muteL.setBounds(spaceForMuteChan.removeFromTop(topHeaderSize / 2));
	muteR.setBounds(spaceForMuteChan.removeFromTop(topHeaderSize));

	delayRangeButton.setBounds(topHeader.removeFromLeft(getWidth() / nButtons));
	phaseDelayButton.setBounds(topHeader.removeFromLeft(getWidth() / nButtons));



	auto midHeader = space.removeFromTop(150);
	feedbackSlider.setBounds(midHeader.removeFromTop(midHeader.getHeight() / 3) );
	panSlider.setBounds(midHeader.removeFromTop(midHeader.getHeight() / 2));
	dryWetSlider.setBounds(midHeader.removeFromTop(midHeader.getHeight()));

	space.reduce(40, 40);
	delaySliders.setBounds(space);
}

// callback when something changes in the processor.
void FftbinDelayAudioProcessorEditor::changeListenerCallback(ChangeBroadcaster* source)
{
	if (source == &processor.transportSource) {
		refreshButtons();
		refreshSliders();
	}
}

// the overrided abstract button clicked function. Called when a button is clicked.
void FftbinDelayAudioProcessorEditor::buttonClicked(Button* button)
{
	if (button == &openButton) {
		openButtonClicked();
	}
	if (button == &playStopButton) {
		playButtonClicked();
	}
	if (button == &mainBypass) {
		bypassButtonClicked();
	}
	if (button == &delayRangeButton) {
		rangeButtonClicked();
	}
	if (button == &phaseDelayButton) {
		phaseInDelayButtonClicked();
	}
	if (button == &oFFTBypass) {
		processor.runoFFT = !processor.runoFFT;
		changeButtonColour(oFFTBypass, processor.runoFFT, true);
		fftBypass.setEnabled(processor.runoFFT);
		if (!processor.runoFFT) {
			fftBypass.setColour(TextButton::ColourIds::buttonColourId, Colours::darkred);
		}
		else {
			refreshButton(fftBypass, processor.oFFT[0]->runFFTs);
		}
	}
	if (button == &fftBypass) {
		if (processor.oFFT[0]->runFFTs == true) {
			processor.oFFT[0]->runFFTs = false;
			processor.oFFT[1]->runFFTs = false;
		}
		else {
			processor.oFFT[0]->runFFTs = true;
			processor.oFFT[1]->runFFTs = true;
		}
		changeButtonColour(fftBypass, processor.oFFT[0]->runFFTs);
	}

	if (button == &muteL) {
		processor.muteL = !processor.muteL;
		changeButtonColour(muteL, processor.muteL);
	}

	if (button == &muteR) {
		processor.muteR = !processor.muteR;
		changeButtonColour(muteR, processor.muteR);
	}

}

void FftbinDelayAudioProcessorEditor::changeButtonColour(TextButton& button, bool condition, bool greenIsTrueOrFalse) {
	if (condition == greenIsTrueOrFalse) {
		button.setColour(TextButton::buttonColourId, Colours::green);
	}
	else {
		button.setColour(TextButton::buttonColourId, Colours::black);
	}
}

void FftbinDelayAudioProcessorEditor::refreshButtons() {
	if (processor.transportSource.getLengthInSeconds() <= 0.0) {
		playStopButton.setEnabled(false);
	}
	else {
		playStopButton.setEnabled(true);
		openButton.setColour(TextButton::buttonColourId, Colours::green);
	}

	if (processor.transportSource.isPlaying()) {
		playStopButton.setButtonText("Stop");
		openButton.setEnabled(false);
	}
	else {
		playStopButton.setButtonText("Play");
		openButton.setEnabled(true);
	}

	if (processor.transportSource.hasStreamFinished()) {
		processor.transportSource.setPosition(0.0);
	}

	if (processor.bypass == true) {
		mainBypass.setColour(TextButton::buttonColourId, Colours::green);
	}
	else {
		mainBypass.setColour(TextButton::buttonColourId, Colours::black);
	}

	refreshButton(delayRangeButton, processor.delayTime == MainVar::delRangeLong);
	refreshButton(phaseDelayButton, processor.oFFT[0]->binDelay.phaseInDelay);
	refreshButton(oFFTBypass, processor.runoFFT);
	refreshButton(fftBypass, processor.oFFT[0]->runFFTs);

	if (!processor.runoFFT) {
		fftBypass.setColour(TextButton::ColourIds::buttonColourId, Colours::darkred);
	}
	else {
		refreshButton(fftBypass, processor.oFFT[0]->runFFTs);
	}

	refreshButton(muteL, processor.muteL);
	refreshButton(muteR, processor.muteR);
}

void FftbinDelayAudioProcessorEditor::refreshButton(TextButton& button, bool condition) {
	if (condition) {
		button.setColour(TextButton::buttonColourId, Colours::green);
	}
	else {
		button.setColour(TextButton::buttonColourId, Colours::black);
	}
}


void FftbinDelayAudioProcessorEditor::playButtonClicked() {
	playStopButton.setColour(TextButton::buttonColourId, Colours::rebeccapurple); //test de functie call.
	processor.playStopButtonClicked();
};

void FftbinDelayAudioProcessorEditor::openButtonClicked() {
	openButton.setColour(TextButton::buttonColourId, Colours::rebeccapurple);
	processor.openButtonClicked();
};

void FftbinDelayAudioProcessorEditor::bypassButtonClicked() {
	if (processor.bypass == false) {
		mainBypass.setColour(TextButton::buttonColourId, Colours::green);
	}
	else {
		mainBypass.setColour(TextButton::buttonColourId, Colours::black);
	}

	processor.bypass = !processor.bypass;
};

void FftbinDelayAudioProcessorEditor::rangeButtonClicked() {
	if (processor.delayTime == MainVar::delRangeLong) {
		delayRangeButton.setColour(TextButton::buttonColourId, Colours::black);
		processor.delayTime = MainVar::delRangeShort;
		delayRangeButton.setButtonText("Short");
		delayRangeButton.setColour(TextButton::textColourOffId, Colours::lightblue);
	}
	else {
		delayRangeButton.setColour(TextButton::buttonColourId, Colours::green);
		processor.delayTime = MainVar::delRangeLong;
		delayRangeButton.setButtonText("Long");
		delayRangeButton.setColour(TextButton::textColourOffId, Colours::white);
	}

	delaySliders.refreshDataValues();
}

void FftbinDelayAudioProcessorEditor::phaseInDelayButtonClicked() {
	if (processor.oFFT[0]->binDelay.phaseInDelay) {
		phaseDelayButton.setColour(TextButton::buttonColourId, Colours::black);
		processor.oFFT[0]->binDelay.phaseInDelay = false;
		processor.oFFT[1]->binDelay.phaseInDelay = false;
		phaseDelayButton.setButtonText("Phase is not in delay");
		phaseDelayButton.setColour(TextButton::textColourOffId, Colours::lightblue);
	}
	else {
		phaseDelayButton.setColour(TextButton::buttonColourId, Colours::green);
		processor.oFFT[0]->binDelay.phaseInDelay = true;
		processor.oFFT[1]->binDelay.phaseInDelay = true;
		phaseDelayButton.setButtonText("Phase is in delay");
		phaseDelayButton.setColour(TextButton::textColourOffId, Colours::white);
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


// the overrided abstract sliderValueChanged function. Called if a slider value is changed.
void FftbinDelayAudioProcessorEditor::sliderValueChanged(Slider* slider) {
	if (slider == &feedbackSlider) {
		//slider->setColour(slider->thumbColourId, Colours::rosybrown);
		newFeedbackSliderValue();
	}

	if (slider == &panSlider) {
		processor.setPanValue( (float)panSlider.getValue());
	}

	if (slider == &dryWetSlider) {
		processor.oFFT[0]->dryWet = (float)dryWetSlider.getValue();
		processor.oFFT[1]->dryWet = (float)dryWetSlider.getValue();
	}
}

void FftbinDelayAudioProcessorEditor::newFeedbackSliderValue() {
	processor.setFeedbackValue( (float)feedbackSlider.getValue());
};

void FftbinDelayAudioProcessorEditor::refreshSliders() {
	feedbackSlider.setValue( processor.getFeedbackValue());
	delaySliders.refreshGUIValues(processor.getBinDelayArray());
	panSlider.setValue(processor.getPanValue());
	dryWetSlider.setValue( processor.oFFT[0]->dryWet);
};

