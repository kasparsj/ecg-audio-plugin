#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    if (!SimpleBLE::Adapter::bluetooth_enabled()) {
        status = BLUETOOTH_NOT_ENABLED;
    }
    else {
        adapters = SimpleBLE::Adapter::get_adapters();
        if (adapters.empty()) {
            status = NO_BLUETOOTH_ADAPTERS;
        }
        else {
            // Get the first adapter
            adapter = adapters[0];
            status = PERIPHERAL_NOT_CONNECTED;

            addAndMakeVisible(connectButton);
            connectButton.setButtonText ("Connect Sensor");
            connectButton.onClick = [this]() {
                showConnectDialog();
            };
        }
    }
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    closeConnectDialog();
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);

    String text = "";
    switch (status) {
        case BLUETOOTH_NOT_ENABLED:
            text = "Bluetooth is not enabled";
            break;
        case NO_BLUETOOTH_ADAPTERS:
            text = "No Bluetooth adapters found";
            break;
    }
    g.drawFittedText (text, getLocalBounds(), Justification::centred, 4);

    if (status == PERIPHERAL_NOT_CONNECTED) {
        connectButton.setButtonText ("Connect Sensor");
    }
    else if (status == PERIPHERAL_CONNECTED) {
        connectButton.setButtonText ("Sensor Connected");
    }
}

void AudioPluginAudioProcessorEditor::resized()
{
    connectButton.setTopRightPosition (20, 15);
    connectButton.setSize(200, 30);
}

void AudioPluginAudioProcessorEditor::showConnectDialog()
{
    DialogWindow::LaunchOptions options;
    options.content.setOwned (deviceList = new DeviceList(*this));

    Rectangle<int> area (0, 0, 300, 200);

    options.content->setSize (area.getWidth(), area.getHeight());

    options.dialogTitle                   = "Connect Sensor";
    options.dialogBackgroundColour        = getLookAndFeel().findColour (ResizableWindow::backgroundColourId);
    options.escapeKeyTriggersCloseButton  = true;
    options.useNativeTitleBar             = false;
    options.resizable                     = false;

    dialogWindow = options.launchAsync();

    if (dialogWindow != nullptr)
        dialogWindow->centreWithSize (300, 200);
}

void AudioPluginAudioProcessorEditor::closeConnectDialog()
{
    if (dialogWindow != nullptr)
    {
        dialogWindow->exitModalState (0);
        // JUCE will throw exception on delete dialogWindow if TableListBox has any rows
        if (deviceList != nullptr) {
            deviceList->setIsFinished();
        }
        delete dialogWindow;
        dialogWindow = nullptr;
        deviceList = nullptr;
    }
}

void AudioPluginAudioProcessorEditor::setConnectedIndex(int i) {
    if (i >= 0 || connectedIndex < 0 || !peripherals[connectedIndex].is_connected()) {
        BLEManager::setConnectedIndex(i);
    }
    status = connectedIndex >= 0 && peripherals[connectedIndex].is_connected() ? PERIPHERAL_CONNECTED : PERIPHERAL_NOT_CONNECTED;
    if (status == PERIPHERAL_CONNECTED) {
        connectButton.setColour(TextButton::ColourIds::buttonColourId, Colours::green);
    }
    else {
        connectButton.setColour(TextButton::ColourIds::buttonColourId, getLookAndFeel().findColour (TextButton::buttonColourId));
    }
    if (i >= 0) {
        closeConnectDialog();
    }
}
