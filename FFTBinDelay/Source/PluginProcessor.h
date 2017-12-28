/////////////////
//Notes:
//////
//TODO:
// - Fix: The FFT size now has to be 8 in all seperate files. Make it changeble

#pragma once

#include <iostream>
#include <fstream>

using namespace std;

#include "../JuceLibraryCode/JuceHeader.h"
#include "overlapFFT.h"
#include "blockDelay.h"


//==============================================================================
/**
*/
class FftbinDelayAudioProcessor : public AudioProcessor
{
public:

    //==============================================================================
	FftbinDelayAudioProcessor();
    ~FftbinDelayAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect () const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	// GUI functions.
	void setFeedbackValue(double feedback);
	float getFeedbackValue() const;
	void setBinDelayTime(int index, float value);
	const float* getBinDelayArray() const;

	void playStopButtonClicked();
	void openButtonClicked();

	float getPanValue() { return panLR; };

	// filePlaying variables
	AudioFormatManager formatManager;
	ScopedPointer<AudioFormatReaderSource> readerSource;
	AudioTransportSource transportSource;

	// GUI variable
	bool bypass = false;
	float delayTime = 100;

	ScopedPointer<OverlapFFT> oFFT[2]; //channels
private:
	int sampleRate = 44100;

	// FFT variables.
	dsp::FFT fftFunction;
	//array< OverlapFFT*, 2 > oFFT;

	// logging
	ofstream myfile;
	int n = 0;

	// GUI controlled parameters
	float panLR = 0.5f;
	float delayArray[MainVar::numBins]; //numBins

    //==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FftbinDelayAudioProcessor)
};
