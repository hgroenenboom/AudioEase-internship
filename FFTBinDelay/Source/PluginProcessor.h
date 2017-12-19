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

	void setPanValue(float pan) { panLR = pan; };
	float getPanValue() { return panLR; };
	
	void setDelayValue(int delay) { 
		this->delay = delay; 
		for (int c = 0; c < 1; c++) {
			for (int i = 0; i < fftSize; i++) {
				oFFT[c]->fftDelays[i]->setDelayTime((float) delay * i * 0.01 + 1.0f);
			}
		}
	};

	void playStopButtonClicked() {
		if (transportSource.getLengthInSeconds() != 0.0) {
			if (transportSource.isPlaying()) {
				transportSource.stop();
				transportSource.setPosition(0.0);
			}
			else {
				transportSource.start();
			}
		}
	}

	void openButtonClicked()
	{
		FileChooser chooser("Select a Wave file to play...",
			File::nonexistent,
			"*.wav");                                        // [7]

		if (chooser.browseForFileToOpen())                                    // [8]
		{
			File file(chooser.getResult());                                  // [9]
			AudioFormatReader* reader = formatManager.createReaderFor(file); // [10]

			if (reader != nullptr)
			{
				ScopedPointer<AudioFormatReaderSource> newSource = new AudioFormatReaderSource(reader, true); // [11]
				transportSource.setSource(newSource, 0, nullptr, reader->sampleRate);                         // [12]
				//playButton.setEnabled(true);                                                                  // [13]
				transportSource.sendChangeMessage();
				readerSource = newSource.release();                                                            // [14]
			}
		}
	}

	AudioFormatManager formatManager;
	ScopedPointer<AudioFormatReaderSource> readerSource;
	AudioTransportSource transportSource;

private:
	enum {
		fftOrder = 9,
		fftSize = 1 << fftOrder
	};

	ScopedPointer<dsp::FFT> fftFunctionP;

	int numFFTOverlaps = 12;

	overlapFFT *oFFT[2]; //channels

	float* inputData[2048];

	ofstream myfile;
	int n = 0;

	float panLR = 0.5;
	int delay = 1;

    //==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FftbinDelayAudioProcessor)
};
