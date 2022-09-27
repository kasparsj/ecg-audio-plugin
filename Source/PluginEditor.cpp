#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    if (!SimpleBLE::Adapter::bluetooth_enabled()) {
        std::cout << "Bluetooth is not enabled" << std::endl;
        return;
    }

    addAndMakeVisible(scanButton);
    scanButton.setButtonText ("Scan");
    scanButton.onClick = [this]() { startScan(); };

    adapters = SimpleBLE::Adapter::get_adapters();
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);

    juce::String text = "";
    if (adapters.empty()) {
        text = "No Bluetooth adapters found";
    }
    else {
        for (int i=0; i<adapters.size(); i++) {
            text += juce::String("Adapter identifier: " + adapters[i].identifier() + " address: " + adapters[i].address() + "\r\n");
        }
        for (auto peripheral : peripherals) {
            text += juce::String("Peripheral identifier: " + peripheral.identifier() + " address: " + peripheral.address() + "\r\n");
        }
    }
    g.drawFittedText (text, getLocalBounds(), juce::Justification::centred, 4);
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    area.reduce (40, 30);
    scanButton.setBounds (area.removeFromTop (35).reduced (100, 0));
}

void AudioPluginAudioProcessorEditor::startScan()
{
    // Get the first adapter
    SimpleBLE::Adapter adapter = adapters[0];

    // Scan for peripherals for 5000 milliseconds
    adapter.scan_for(5000);

    // Get the list of peripherals found
    peripherals = adapter.scan_get_results();

    repaint();
}