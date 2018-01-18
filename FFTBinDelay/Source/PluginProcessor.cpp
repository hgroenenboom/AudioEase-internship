

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
	, fftFunction(MainVar::fftOrder)
	, fft2Function(MainVar::fftOrder + 2)
	, convolver(1 << 12)
{

	formatManager.registerBasicFormats(); 
	myfile.open("myLog.txt");

	for (int i = 0; i < (sizeof(delayArray) / sizeof(delayArray[0])); i++) {
		delayArray[i] = 1;
	}

	for (int channel = 0; channel < 2; channel++) {
		oFFT[channel] = new OverlapFFT();
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
void FftbinDelayAudioProcessor::prepareToPlay (double samplerate, int samplesPerBlock)
{
	transportSource.prepareToPlay(samplesPerBlock, samplerate);
	if (samplerate != samplerate) {
		this->sampleRate = (int) samplerate;
	
		for (int c = 0; c < 2; c++) {
			oFFT[c]->setBinDelayWithNewSampleRate( this->sampleRate);
		}
	}
	//openButtonClicked();
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
		// do nothing 
	}
	else {
		inputInfo.clearActiveBufferRegion(); // needed for clean sound
		transportSource.getNextAudioBlock(inputInfo);
	}

	if (bypass == false) {
		for (int channel = 0; channel < 2; ++channel)
		{
			// FFT INPUT, MODIFICATIONS & OUTPUT: loop through every sample of the buffer and perform fft if fftSize samples have been received
			if (runoFFT == true) {
				oFFT[channel]->pushDataIntoMemoryAndPerformFFTs(buffer, buffer.getNumSamples(), channel);
			}

			// LOG TO FILE
			/*if (panLR != 0.5) myfile << "panning: " << panLR << "\n";
			if (buffer.getNumSamples() != 512) {
				myfile << "current callbackSize: " << buffer.getNumSamples() << "\n";
				myfile << "current callback: " << n++ << "\n";
			}
			else { n++; }*/
		}

		//actualPan += ((panLR - 0.5f) * 0.03);
		//actualPan = (actualPan < 0.0f) ? actualPan + 1.0f : actualPan;
		//actualPan = (actualPan > 1.0f) ? actualPan - 1.0f : actualPan;
		convolver.convolve(buffer, buffer.getNumSamples(), 0.5, ((panLR - 0.5f) * 0.1f), panLR, temp);
	}

	if(muteL) buffer.clear(0, 0, buffer.getNumSamples());
	if(muteR) buffer.clear(1, 0, buffer.getNumSamples());
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

void FftbinDelayAudioProcessor::setFeedbackValue(float feedback) {
	for (int c = 0; c < 2; c++) {
		oFFT[c]->binDelay.setFeedback(feedback);
		temp = feedback;
	}
}

float FftbinDelayAudioProcessor::getFeedbackValue() const {
	return oFFT[0]->binDelay.getFeedback();
}

void FftbinDelayAudioProcessor::setBinDelayTime(int index, float value) {
	delayArray[index] = value;
	for (int c = 0; c < 2; c++) {
		oFFT[c]->binDelay.setDelayTime(index, max(0.0f, (1.0f - value) * delayTime) );
	}
}

const float* FftbinDelayAudioProcessor::getBinDelayArray() const {
	return delayArray;
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
