#include "DeviceList.h"

#include <utility>

DeviceList::DeviceList(SimpleBLE::Adapter &adapter, std::function<void(SimpleBLE::Peripheral)> callback) : adapter(adapter), callback(std::move(callback)) {
    setSize(300, 200);

    addAndMakeVisible (table);
    table.setModel (this);
    table.setColour (juce::ListBox::outlineColourId, juce::Colours::grey);
    table.setOutlineThickness (1);
    table.getHeader().addColumn ("Devices",
        1,
        200,
        50,
        200,
        TableHeaderComponent::notSortable);

    table.getHeader().addColumn ("",
        2,
        100,
        50,
        100,
        TableHeaderComponent::notSortable);
    table.setMultipleSelectionEnabled (false);

    adapter.set_callback_on_scan_start([this]() {
        isScanning = true;
        table.updateContent();
    });
    adapter.set_callback_on_scan_stop([this]() {
        isScanning = false;
        table.updateContent();
    });
    adapter.set_callback_on_scan_found([this](SimpleBLE::Peripheral peripheral) {
        peripherals.push_back(peripheral);
        table.updateContent();
    });
    adapter.set_callback_on_scan_updated([this](SimpleBLE::Peripheral peripheral) {
        for (int i=0; i<peripherals.size(); i++) {
            if (peripherals[i].identifier() == peripheral.identifier()) {
                peripherals[i] = peripheral;
            }
        }
        table.updateContent();
    });
    adapter.scan_start();

    startTimer(20000);
}

void DeviceList::timerCallback() {
    stopTimer();
    adapter.scan_stop();
}

void DeviceList::resized()
{
    table.setBoundsInset (BorderSize<int> (0));
}

void DeviceList::connect (int rowNumber)
{
    SimpleBLE::Peripheral peripheral = peripherals[rowNumber - (isScanning ? 1 : 0)];
    peripheral.connect();
    callback(peripheral);
}