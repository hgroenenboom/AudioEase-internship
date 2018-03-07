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
#include "MainVar.h"

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
	void setFeedbackValue(float feedback);
	float getFeedbackValue() const;
	void setBinDelayTime(int index, float value);
	const float* getBinDelayArray() const;
	void setBinDelayAmps(const int a, const float b) {
		par::ampArray[a] = b;
		interpolateSurroundingIndices(par::ampArray, mVar::nBands, a, b, 5);

		for (int i = 0; i < 2; i++) {
			//oFFT[i]->binDelay.ampsBinsPerBand[a] = b;
		}
	};
	float* getPanValues() {
		return par::panSpeed;
	}
	void setPanValues(int index, float value) {
		par::panSpeed[index] = value;
	}

	void playStopButtonClicked();
	void openButtonClicked();

	void setPanValue(float p) { panLR = p;  }
	float getPanValue() { return panLR; };

	// filePlaying variables
	AudioFormatManager formatManager;
	ScopedPointer<AudioFormatReaderSource> readerSource;
	AudioTransportSource transportSource;

	// GUI variable
	bool bypass = false, runoFFT = true, muteL = false, muteR = false, micOn = false;
	float temp = 0.0f;

	// will vary between a long or a short delay time. Should be private
	float delayTime = 100;
	// Pointers to the overlap FFT objects.
	ScopedPointer<OverlapFFT> oFFT[2]; //channels
private:
	int sampleRate = 44100;

	// FFT variables.
	dsp::FFT fftFunction, fft2Function;

	// logging
	ofstream myfile;
	int n = 0;

	// GUI controlled parameters
	float panLR = 0.0f;
	float actualPan = 0.0f;
	float panSpeed[mVar::nBands];
	float panLocation[mVar::nBands];

	void getStateOfArray(float* array, int size, std::string prefix, XmlElement& x) {
		int arrLen = sizeof(array) / sizeof(array[0]);
		for (int i = 0; i < size; i++) {
			x.setAttribute(prefix + (String)i , array[i]);
		}
	}

	void setStateOfArray(float* array, int size, std::string prefix, XmlElement& x) {
		for (int i = 0; i < size; i++) {
			array[i] = ((float)x.getDoubleAttribute(prefix + (String)i, 0.0 ));
		}
	}

	void interpolateSurroundingIndices(float* array, int arraySize, int index, float value, int nval) {
		float fnval = (float)nval;
		for (int i = -nval; i < nval; i++) {
			if (i + index >= 0 && i + index < mVar::nBands) {
				if (i != 0) {
					array[i + index] += pow(sin((i + nval) / (float)(nval*2) * float_Pi) * 0.3f, 2.0f) * (array[index] - array[i + index]);
				}
			}
		}
	}

    //==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FftbinDelayAudioProcessor)
};
