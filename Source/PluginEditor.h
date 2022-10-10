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

    const std::string HR_SERVICE = "180d";
    const std::string HR_DATA = "2a37";

    const std::string PMD_SERVICE = "FB005C80-02E7-F387-1CAD-8ACD2D8DF0C8";
    const std::string PMD_CP = "FB005C81-02E7-F387-1CAD-8ACD2D8DF0C8";
    const std::string PMD_DATA = "FB005C82-02E7-F387-1CAD-8ACD2D8DF0C8";

    const char CMD_GET_SETTINGS = '1';
    const char CMD_START_STREAM = '2';
    const char CMD_STOP_STREAM = '3';

    const char TYPE_ECG = '0';

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
    AudioPluginAudioProcessor& processor;
    juce::AudioProcessorParameter* hr;

    EditorStatus status;
    TextButton connectButton;
    bool sensorConnected = false;
    SafePointer<ModalWindow> dialogWindow;
    std::vector<SimpleBLE::Adapter> adapters;
    DeviceList *deviceList = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
