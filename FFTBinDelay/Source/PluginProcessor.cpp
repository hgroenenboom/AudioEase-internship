

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
	, fftFunction(mVar::fftOrder)
	, fft2Function(mVar::fftOrder + 2)
{
	addParameter(par::dryWet);
	addParameter(par::feedBack);

	// register wav files format for file playback
	formatManager.registerBasicFormats(); 
	// open log file
	myfile.open("myLog.txt");

	// create two instances of the overlap FFT class for each channel.
	for (int channel = 0; channel < 2; channel++) {
		oFFT[channel] = new OverlapFFT(channel);
		oFFT[channel]->binDelay.setPanLocations(par::panLocation);
	}

	// intialize delayArray
	for (int i = 0; i < (sizeof(par::delayArray) / sizeof(par::delayArray[0])); i++) {
		par::delayArray[i] = 1;
		//setBinDelayTime(i, 1);
	}

	// init panning arrays
	for (int i = 0; i < mVar::nBands; i++) {
		par::panSpeed[i] = 0.5f;
		par::panLocation[i] = 0.5f;
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
	// prepare the transportSource
	transportSource.stop();
	transportSource.prepareToPlay(samplesPerBlock, samplerate);
	// set samplerate
	if (samplerate != samplerate) {
		this->sampleRate = (int) samplerate;
	
		// create new delaybuffers corresponding to the new samplerate.
		for (int c = 0; c < 2; c++) {
			oFFT[c]->setBinDelayWithNewSampleRate( this->sampleRate);
		}
	}
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
	
	const AudioSourceChannelInfo inputInfo(buffer);
	if (!micOn) {
		inputInfo.clearActiveBufferRegion(); // needed for clean sound
		if(readerSource != nullptr)
			transportSource.getNextAudioBlock(inputInfo);
	}

	for (int i = 0; i < mVar::nBands; i++) {
		par::panLocation[i] += pow(par::panSpeed[i] * 0.2f - 0.1f, 2.0f);
		par::panLocation[i] = fmod(par::panLocation[i] + 1.0f, 1.0f);
		//DBG(par::panLocation[i]);
	}

	if (!bypass) {
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

		// TIME BASED Convolution, currently disabled
		//actualPan += ((panLR - 0.5f) * 0.03);
		//actualPan = (actualPan < 0.0f) ? actualPan + 1.0f : actualPan;
		//actualPan = (actualPan > 1.0f) ? actualPan - 1.0f : actualPan;
		//convolver.convolve(buffer, buffer.getNumSamples(), 0.5, ((panLR - 0.5f) * 0.1f), panLR, temp);
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
	// Create an outer XML element..
	XmlElement xml("GREEN-Settings:");

	//for(int i = 0; i <  )

	// add some attributes to it..
	//xml.setAttribute("uiWidth", lastUIWidth);
	//xml.setAttribute("uiHeight", lastUIHeight);

	// Store the values of all our parameters, using their param ID as the XML attribute
	for (auto* param : getParameters())
		if (auto* p = dynamic_cast<AudioProcessorParameterWithID*> (param))
			xml.setAttribute(p->paramID, p->getValue());

	getStateOfArray(par::delayArray, mVar::nBands, "delArr", xml);
	getStateOfArray(par::ampArray, mVar::nBands, "ampArr", xml);

	// then use this helper function to stuff it into the binary blob and return it..
	copyXmlToBinary(xml, destData);
}

void FftbinDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	// This getXmlFromBinary() helper function retrieves our XML from the binary blob..
	ScopedPointer<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState != nullptr)
	{
		// make sure that it's actually our type of XML object..
		if (xmlState->hasTagName("GREEN-Settings:"))
		{
			// ok, now pull out our last window size..
			//lastUIWidth = jmax(xmlState->getIntAttribute("uiWidth", lastUIWidth), 400);
			//lastUIHeight = jmax(xmlState->getIntAttribute("uiHeight", lastUIHeight), 200);

			// Now reload our parameters..
			for (auto* param : getParameters())
				if (auto* p = dynamic_cast<AudioProcessorParameterWithID*> (param))
					p->setValue((float)xmlState->getDoubleAttribute(p->paramID, p->getValue()));

			setStateOfArray(par::delayArray, mVar::nBands, "delArr", *xmlState);
			for (int i = 0; i < mVar::nBands; i++) setBinDelayTime(i, par::delayArray[i]);
			setStateOfArray(par::ampArray, mVar::nBands, "ampArr", *xmlState);

		}
	}
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
	par::delayArray[index] = value;

	for (int c = 0; c < 2; c++) {
		oFFT[c]->binDelay.setDelayTime(index, max(0.0f, (1.0f - value) * delayTime) );
	}
}

const float* FftbinDelayAudioProcessor::getBinDelayArray() const {
	return par::delayArray;
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
