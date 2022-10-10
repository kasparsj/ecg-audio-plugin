#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    hr = dynamic_cast<AudioParameterInt*>(processor.getParameters()[0]);

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

    g.setColour (Colours::white);
    g.setFont (35.0f);
    g.drawFittedText ("HR: " + String(hr->get()), Rectangle(20, 60, 360, 20), Justification::left, 1);
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
        try {
            if (activeIndex >= 0) {
                peripherals[activeIndex].unsubscribe(HR_SERVICE, HR_DATA);
                peripherals[activeIndex].unsubscribe(PMD_SERVICE, PMD_DATA);
                peripherals[activeIndex].unsubscribe(PMD_SERVICE, PMD_CP);
                peripherals[activeIndex].write_request(PMD_SERVICE, PMD_CP, std::string(1, CMD_STOP_STREAM) + TYPE_ECG);
                // todo: stop stream
            }
            BLEManager::setActiveIndex (i);
            status = activeIndex >= 0 && peripherals[activeIndex].is_connected() ? PERIPHERAL_CONNECTED : PERIPHERAL_NOT_CONNECTED;
            if (activeIndex >= 0) {
                peripherals[activeIndex].notify(HR_SERVICE, HR_DATA, [&](SimpleBLE::ByteArray bytes) {
                    *hr = (int) (uint8_t) bytes[1];

                    const MessageManagerLock mmLock;
                    repaint();
                });
                peripherals[activeIndex].notify(PMD_SERVICE, PMD_DATA, [&](SimpleBLE::ByteArray bytes) {
                    std::cout << "ecg " << bytes << std::endl;
                });
//                for (int i=0; i<peripherals[activeIndex].services().size(); i++) {
//                    for (int j=0; j<peripherals[activeIndex].services()[i].characteristics().size(); j++) {
//                        std::cout << peripherals[activeIndex].services()[i].uuid() << " " << peripherals[activeIndex].services()[i].characteristics()[j].uuid() << std::endl;
//                    }
//                }
                peripherals[activeIndex].indicate(PMD_SERVICE, PMD_CP, [&](SimpleBLE::ByteArray bytes) {
                    std::cout << "cp ";
                    for (int i=0; i<bytes.length(); i++) {
                        std::cout << std::hex <<  bytes[i] << " ";
                    }
                    std::cout << std::endl;
                });
//                std::cout << "cp read: " << peripherals[activeIndex].read(PMD_SERVICE, PMD_CP) << std::endl;
//                peripherals[activeIndex].write_request(PMD_SERVICE, PMD_CP, std::string(1, CMD_GET_SETTINGS) + TYPE_ECG);
                // todo: need to set MTU 512, but not possible with MacOs
                //0x02, 0x00, 0x01, 0x01, 0x0E, 0x00, 0x00, 0x01, 0x82, 0x00
                peripherals[activeIndex].write_request(PMD_SERVICE, PMD_CP, std::string(1, CMD_START_STREAM) + TYPE_ECG + "11140011300");
                closeConnectDialog();
            }
        }
        catch (std::exception& e) {
            AlertWindow::showMessageBoxAsync (
                AlertWindow::WarningIcon,
                "Error connecting to sensor:",
                e.what(),
                "OK");
        }
    }
}

