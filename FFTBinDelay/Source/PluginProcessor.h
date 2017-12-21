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
#include "singleFFT.h"
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

	void setFeedbackValue(float feedback) {
		for (int i = 0; i < 2; i++) {
			oFFT[i]->binDelay.setFeedback(feedback);
		}
	}

	float getPanValue() { return panLR; };
	
	void setDelaySliderValue(int index, int value) {
		delayArray[index] = value;
		for (int c = 0; c < 2; c++) {
			oFFT[c]->binDelay.setDelayTime(delayArray);
		}
	}
	void setDelayValue(int delay);
	int getDelayValue() { return delay;  }

	void playStopButtonClicked();
	void openButtonClicked();

	// filePlaying
	AudioFormatManager formatManager;
	ScopedPointer<AudioFormatReaderSource> readerSource;
	AudioTransportSource transportSource;
private:
	enum {
		fftOrder = 9,
		fftSize = 1 << fftOrder
	};

	// FFT variables.
	ScopedPointer<dsp::FFT> FFTFUNCTIONP;
	int numFFTOverlaps = 8;
	overlapFFT *oFFT[2]; //channels

	// unused?
	float* inputData[2048];

	// logging
	ofstream myfile;
	int n = 0;

	// GUI controlled parameters
	public: bool bypass = false;
	private: float panLR = 0.5;
	int delay = 1;
	int delayArray[fftSize];

    //==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FftbinDelayAudioProcessor)
};
