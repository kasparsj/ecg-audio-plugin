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

    connectButton.setColour(TextButton::ColourIds::buttonColourId, getLookAndFeel().findColour (TextButton::buttonColourId));
    if (status < PERIPHERAL_NOT_CONNECTED) {
        String text = "";
        switch (status) {
            case BLUETOOTH_NOT_ENABLED:
                text = "Bluetooth is not enabled";
                break;
            case NO_BLUETOOTH_ADAPTERS:
                text = "No Bluetooth adapters found";
                break;
        }
        g.setColour (Colours::white);
        g.setFont (15.0f);
        g.drawFittedText (text, getLocalBounds(), Justification::centred, 4);
    }
    else if (status == PERIPHERAL_NOT_CONNECTED) {
        connectButton.setButtonText ("Connect Sensor");
    }
    else if (status == PERIPHERAL_CONNECTED) {
        connectButton.setButtonText ("Sensor Connected");
        connectButton.setColour(TextButton::ColourIds::buttonColourId, Colours::green);
    }

    if (activeIndex >= 0) {
        g.setColour (Colours::white);
        g.setFont (15.0f);
        g.drawFittedText (peripherals[activeIndex].identifier(), Rectangle(190, 20, 180, 20), Justification::left, 1);
    }

    if (hr > 0) {
        g.setColour (Colours::white);
        g.setFont (35.0f);
        g.drawFittedText ("HR: " + String(hr), Rectangle(20, 60, 360, 20), Justification::left, 1);
    }
}

void AudioPluginAudioProcessorEditor::resized()
{
    connectButton.setTopRightPosition (20, 15);
    connectButton.setSize(150, 30);
}

void AudioPluginAudioProcessorEditor::showConnectDialog()
{
    ModalWindow::LaunchOptions options;
    options.content.setOwned (deviceList = new DeviceList(*this));

    Rectangle<int> area (0, 0, 300, 200);

    options.content->setSize (area.getWidth(), area.getHeight());

    options.dialogTitle                   = "Connect Sensor";
    options.dialogBackgroundColour        = getLookAndFeel().findColour (ResizableWindow::backgroundColourId);
    options.escapeKeyTriggersCloseButton  = true;
    options.useNativeTitleBar             = false;
    options.resizable                     = false;

    startScan();

    dialogWindow = new ModalWindow(options);
    dialogWindow->enterModalState (true, nullptr, true);
    dialogWindow->onClose = [this]()
    {
        dialogClosing();
    };

    if (dialogWindow != nullptr)
        dialogWindow->centreWithSize (300, 200);
}

void AudioPluginAudioProcessorEditor::closeConnectDialog()
{
    if (dialogWindow != nullptr)
    {
        dialogWindow->exitModalState (0);
        if (deviceList != nullptr) {
            // todo: QUICKFIX: JUCE will throw exception on delete dialogWindow if TableListBox has any rows
            deviceList->setModelToNull();
            dialogClosing();
        }
        delete dialogWindow;
        dialogWindow = nullptr;
        deviceList = nullptr;
    }
}

void AudioPluginAudioProcessorEditor::dialogClosing() {
    stopScan();
    if (activeIndex < 0) {
        hr = 0;

        const MessageManagerLock mmLock;
        repaint();
    }
}

void AudioPluginAudioProcessorEditor::setActiveIndex (int i) {
    if (i != activeIndex) {
        if (activeIndex >= 0) {
            peripherals[activeIndex].unsubscribe("180d", "2a37");
        }
        BLEManager::setActiveIndex (i);
        status = activeIndex >= 0 && peripherals[activeIndex].is_connected() ? PERIPHERAL_CONNECTED : PERIPHERAL_NOT_CONNECTED;
        if (activeIndex >= 0) {
            peripherals[activeIndex].notify("180d", "2a37", [&](SimpleBLE::ByteArray bytes) {
                hr = (uint32_t)((uint8_t) bytes[1]);

                const MessageManagerLock mmLock;
                repaint();
            });
            peripherals[activeIndex].notify("180d", "2a37", [&](SimpleBLE::ByteArray bytes) {
                hr = (uint32_t)((uint8_t) bytes[1]);

                const MessageManagerLock mmLock;
                repaint();
            });
            closeConnectDialog();
        }
    }
}

