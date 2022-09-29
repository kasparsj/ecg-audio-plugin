#pragma once

#include "BLEManager.h"
#include "PluginProcessor.h"
#include "DeviceList.h"
#include "ModalWindow.h"

using namespace juce;

//==============================================================================
class AudioPluginAudioProcessorEditor  : public AudioProcessorEditor, public BLEManager
{
public:
    enum EditorStatus
    {
        BLUETOOTH_NOT_ENABLED,
        NO_BLUETOOTH_ADAPTERS,
        PERIPHERAL_NOT_CONNECTED,
        PERIPHERAL_CONNECTED,
    };

    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    void showConnectDialog();
    void closeConnectDialog();
    void dialogClosing();

    void setActiveIndex (int i) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    EditorStatus status;
    TextButton connectButton;
    bool sensorConnected = false;
    SafePointer<ModalWindow> dialogWindow;
    std::vector<SimpleBLE::Adapter> adapters;
    DeviceList *deviceList = 0;
    int hr = 0;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
