

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FftbinDelayAudioProcessor::FftbinDelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
			#if ! JucePlugin_IsMidiEffect
			 #if ! JucePlugin_IsSynth
				.withInput("Input", AudioChannelSet::stereo(), true)
			#endif
				.withOutput("Output", AudioChannelSet::stereo(), true)
			#endif 
				)
#endif
{
	FFTFUNCTIONP = new dsp::FFT(fftOrder);

	formatManager.registerBasicFormats(); 
	myfile.open("myLog.txt");

	for (int i = 0; i < (sizeof(delayArray) / sizeof(delayArray[0])); i++) {
		delayArray[i] = 1;
	}

	for (int channel = 0; channel < 2; channel++) {
		oFFT[channel] = new overlapFFT(FFTFUNCTIONP, numFFTOverlaps, fftSize);
	}
}



FftbinDelayAudioProcessor::~FftbinDelayAudioProcessor()
{
	myfile.close();
}

//==============================================================================
const String FftbinDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FftbinDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FftbinDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FftbinDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FftbinDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FftbinDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FftbinDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FftbinDelayAudioProcessor::setCurrentProgram (int index)
{
}

const String FftbinDelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void FftbinDelayAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void FftbinDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	transportSource.prepareToPlay(samplesPerBlock, sampleRate);
}

void FftbinDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FftbinDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void FftbinDelayAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    
	/* for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples()); */

    //for (int channel = 0; channel < totalNumInputChannels; ++channel)
	
	const AudioSourceChannelInfo inputInfo(buffer);
	if (readerSource == nullptr)
	{
		//inputInfo.clearActiveBufferRegion();
	}
	else {
		transportSource.getNextAudioBlock(inputInfo);
	}

	if (bypass == false) {
		for (int channel = 0; channel < 2; ++channel) //NOTE: nog mono
		{
			// POINTERS TO BUFFER

			// FFT INPUT, MODIFICATIONS & OUTPUT: loop through every sample of the buffer and perform fft if fftSize samples have been received
			oFFT[channel]->pushDataIntoMemoryAndPerformFFTs(buffer, buffer.getNumSamples(), channel);

			// LOG TO FILE
			if (panLR != 0.5) myfile << "panning: " << panLR << "\n";
			if (buffer.getNumSamples() != 512) {
				myfile << "current callbackSize: " << buffer.getNumSamples() << "\n";
				myfile << "current callback: " << n++ << "\n";
			}
			else { n++; }
		}
	}

	//buffer.clear(1, 0, buffer.getNumSamples());
}

//==============================================================================
bool FftbinDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* FftbinDelayAudioProcessor::createEditor()
{
    return new FftbinDelayAudioProcessorEditor (*this);
}

//==============================================================================
void FftbinDelayAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FftbinDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FftbinDelayAudioProcessor();
}

void FftbinDelayAudioProcessor::setDelayValue(int delay) {
	this->delay = delay;
	for (int c = 0; c < 2; c++) {
		for (int i = 0; i < 30; i++) {
			delayArray[i] = delay * + i * delay;
		}
		oFFT[c]->binDelay.setDelayTime(delayArray);
	}
}

void FftbinDelayAudioProcessor::playStopButtonClicked() {
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

void FftbinDelayAudioProcessor::openButtonClicked() {
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
