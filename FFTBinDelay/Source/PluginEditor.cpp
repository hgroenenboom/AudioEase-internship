/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

//#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
FftbinDelayAudioProcessorEditor::FftbinDelayAudioProcessorEditor(FftbinDelayAudioProcessor& p)
	: AudioProcessorEditor(&p), processor(p)
	, delaySliders(p, "delay\ntime\n")
	, panSliders(p, "panning", true)
	, ampSliders(p, "amplitudes")
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
	startTimer(30);

	addAndMakeVisible(visual);
	//visual.setOpaque(true);

	delayF = [=](int a, float b) { this->processor.setBinDelayTime(a, b); };
	addAndMakeVisible(&delaySliders);
	delaySliders.setFunction(delayF);
	addAndMakeVisible(&delayLabel);
	delayLabel.setText("Delay time\n(s)", dontSendNotification);
	delayLabel.attachToComponent(&delaySliders, true);
	delayLabel.setJustificationType(Justification::centred);

	panF = [=](int a, float b) { this->processor.setPanValues(a, b); };
	addAndMakeVisible(&panSliders);
	panSliders.setFunction(panF);
	addAndMakeVisible(&autopanLabel);
	autopanLabel.setText("Autopan speed\n", dontSendNotification);
	autopanLabel.attachToComponent(&panSliders, true);
	autopanLabel.setJustificationType(Justification::centred);

	ampF = [=](int a, float b) { this->processor.setBinDelayAmps(a, b); };
	addAndMakeVisible(&ampSliders);
	ampSliders.setFunction(ampF);
	addAndMakeVisible(&ampSliders);
	ampLabel.setText("Amplitude\nsliders", dontSendNotification);
	ampLabel.attachToComponent(&ampSliders, true);
	ampLabel.setJustificationType(Justification::centred);

	addAndMakeVisible(&openButton);
	openButton.setButtonText("Open...");
	openButton.addListener(this);
	buttonInit(micOnButton, "Mic On");

	addAndMakeVisible(&playStopButton);
	playStopButton.setButtonText("Play");
	playStopButton.addListener(this);
	playStopButton.setColour(TextButton::buttonColourId, Colours::green);
	playStopButton.setEnabled(true);

	addAndMakeVisible(&feedbackSlider);
	feedbackSlider.addListener(this);
	feedbackSlider.setRange(0.0, 1.0, 0.001);
	addAndMakeVisible(&feedbackLabel);
	feedbackLabel.setText("Feedback", dontSendNotification);
	feedbackLabel.attachToComponent(&feedbackSlider, true);

	addAndMakeVisible(&mainBypass);
	mainBypass.addListener(this);
	mainBypass.setButtonText("ByPass");

	addAndMakeVisible(&delayRangeButton);
	delayRangeButton.addListener(this);
	delayRangeButton.setButtonText("delayTime * 100");

	addAndMakeVisible(&setUseButton);
	setUseButton.addListener(this);
	setUseButton.setButtonText("Not Used");

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
	addAndMakeVisible(&panLabel);
	panLabel.setText("Not Used", dontSendNotification);
	panLabel.attachToComponent(&panSlider, true);

	addAndMakeVisible(&dryWetSlider);
	dryWetSlider.addListener(this);
	dryWetSlider.setRange(0.0, 1.0, 0.001);
	addAndMakeVisible(&dryWetLabel);
	dryWetLabel.setText("WetDry", dontSendNotification);
	dryWetLabel.attachToComponent(&dryWetSlider, true);

	refreshButtons();
	refreshSliders();

	//setSize(600, 600);
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
	const int topHeaderSize = 80;

	auto topHeader = space.removeFromTop(topHeaderSize);
	auto micOpenSpace = topHeader.removeFromLeft(getWidth() / nButtons);
	openButton.setBounds(micOpenSpace.removeFromTop(micOpenSpace.getHeight() / 2));
	micOnButton.setBounds(micOpenSpace);
	playStopButton.setBounds(topHeader.removeFromLeft(getWidth() / nButtons));
	mainBypass.setBounds(topHeader.removeFromLeft(getWidth() / nButtons));
	auto spaceForFFTBypass = topHeader.removeFromLeft(getWidth() / nButtons);
	oFFTBypass.setBounds(spaceForFFTBypass.removeFromTop(topHeaderSize - (int)(topHeaderSize / 2.5f)));
	fftBypass.setBounds(spaceForFFTBypass);
	auto spaceForMuteChan = topHeader.removeFromLeft(getWidth() / nButtons);
	muteL.setBounds(spaceForMuteChan.removeFromTop(topHeaderSize / 2));
	muteR.setBounds(spaceForMuteChan.removeFromTop(topHeaderSize));
	delayRangeButton.setBounds(topHeader.removeFromLeft(getWidth() / nButtons));
	setUseButton.setBounds(topHeader.removeFromLeft(getWidth() / nButtons));

	auto midHeader = space.removeFromTop(90);
	feedbackSlider.setBounds(midHeader.removeFromTop(midHeader.getHeight() / 3).removeFromRight(getWidth() - 80));
	panSlider.setBounds(midHeader.removeFromTop(midHeader.getHeight() / 2).removeFromRight(getWidth() - 80));
	dryWetSlider.setBounds(midHeader.removeFromTop(midHeader.getHeight()).removeFromRight(getWidth() - 80));

	space.reduce(20, 20);
	space.removeFromLeft(100);
	auto mSlidHeader = space.removeFromTop(space.getHeight() / 4 * 2);
	int numMSliders = 3;
	int remFromTop = (int)(mSlidHeader.getHeight() / (numMSliders + (float)numMSliders * 0.1f));
	int spce = (int)(mSlidHeader.getHeight() * ((0.2f) / (numMSliders + (float)numMSliders * 0.1f)));
	delaySliders.setBounds(mSlidHeader.removeFromTop(remFromTop));
	mSlidHeader.removeFromTop(spce);
	panSliders.setBounds(mSlidHeader.removeFromTop(remFromTop));
	mSlidHeader.removeFromTop(spce);
	ampSliders.setBounds(mSlidHeader.removeFromTop(remFromTop));

	if (space.getHeight() < space.getWidth()) {
		visual.setBounds(getWidth() * 0.5f - space.getHeight() * 0.5f, space.getY(), space.getHeight(), space.getHeight());
	}
}

void FftbinDelayAudioProcessorEditor::timerCallback()  
{ 
	dryWetSlider.setValue(*par::dryWet, dontSendNotification);
	feedbackSlider.setValue(*par::feedBack, dontSendNotification);
	refreshSliders();
}



void FftbinDelayAudioProcessorEditor::buttonInit(TextButton &button, std::string str) {
	addAndMakeVisible(&button);
	button.addListener(this);
	button.setColour(TextButton::buttonColourId, Colours::aliceblue);
	button.setButtonText(str);
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
	if (button == &micOnButton) {
		processor.micOn = !processor.micOn;
		changeButtonColour(micOnButton, processor.micOn, true);
		if (processor.micOn) {
			openButton.setColour(TextButton::ColourIds::buttonColourId, Colours::darkred);
			openButton.setEnabled(false);
		}
		else {
			changeButtonColour(openButton, processor.transportSource.isPlaying(), true);
			openButton.setEnabled(true);
		}
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
	if (button == &setUseButton) {
		setUseButtonClicked();
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

	refreshButton(delayRangeButton, processor.delayTime == mVar::delRangeLong);
	refreshButton(setUseButton, par::setUse);
	refreshButton(oFFTBypass, processor.runoFFT);
	refreshButton(fftBypass, processor.oFFT[0]->runFFTs);
	refreshButton(micOnButton, processor.micOn);

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
	if (processor.delayTime == mVar::delRangeLong) {
		delayRangeButton.setColour(TextButton::buttonColourId, Colours::black);
		processor.delayTime = mVar::delRangeShort;
		delayRangeButton.setButtonText("Short");
		delayRangeButton.setColour(TextButton::textColourOffId, Colours::lightblue);
	}
	else {
		delayRangeButton.setColour(TextButton::buttonColourId, Colours::green);
		processor.delayTime = mVar::delRangeLong;
		delayRangeButton.setButtonText("Long");
		delayRangeButton.setColour(TextButton::textColourOffId, Colours::white);
	}

	delaySliders.refreshDataValues();
}

void FftbinDelayAudioProcessorEditor::setUseButtonClicked() {
	if (par::setUse) {
		setUseButton.setColour(TextButton::buttonColourId, Colours::black);
		par::setUse = false;
		setUseButton.setColour(TextButton::textColourOffId, Colours::lightblue);
	}
	else {
		setUseButton.setColour(TextButton::buttonColourId, Colours::green);
		par::setUse = true;
		setUseButton.setColour(TextButton::textColourOffId, Colours::white);
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
		processor.oFFT[0]->pan = (float)panSlider.getValue();
		processor.oFFT[1]->pan = (float)panSlider.getValue();
	}

	if (slider == &dryWetSlider) {
		//processor.oFFT[0]->dryWet = (float)dryWetSlider.getValue();
		par::dryWet->setValueNotifyingHost( (float)dryWetSlider.getValue());
		//processor.oFFT[1]->dryWet = (float)dryWetSlider.getValue();
	}
}

void FftbinDelayAudioProcessorEditor::newFeedbackSliderValue() {
	//processor.setFeedbackValue( (float)feedbackSlider.getValue());
	par::feedBack->setValueNotifyingHost((float)feedbackSlider.getValue());
};

void FftbinDelayAudioProcessorEditor::refreshSliders() {
	feedbackSlider.setValue( *par::feedBack);
	delaySliders.refreshGUIValues(processor.getBinDelayArray());
	panSliders.refreshGUIValues(processor.getPanValues());
	ampSliders.refreshGUIValues(par::ampArray);
	panSlider.setValue(processor.getPanValue());
	dryWetSlider.setValue( *par::dryWet);
};

