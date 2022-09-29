#include "DeviceList.h"

#include <utility>

DeviceList::DeviceList(BLEManager&owner) : owner(owner) {
    setSize(300, 200);

    addAndMakeVisible (table);
    table.setModel (this);
    table.setColour (juce::ListBox::outlineColourId, juce::Colours::grey);
    table.setOutlineThickness (1);
    table.getHeader().addColumn ("Devices",
        1,
        196,
        50,
        196,
        TableHeaderComponent::notSortable);

    table.getHeader().addColumn ("",
        2,
        80,
        50,
        100,
        TableHeaderComponent::notSortable);
    table.setMultipleSelectionEnabled (false);

    startScan();
}

void DeviceList::startScan() {
    SimpleBLE::Adapter &adapter = owner.getAdapter();
    adapter.set_callback_on_scan_start([this]() {
        isScanning = true;

        const MessageManagerLock mmLock;
        table.updateContent();
    });
    adapter.set_callback_on_scan_stop([this]() {
        isScanning = false;

        const MessageManagerLock mmLock;
        table.updateContent();
    });
    adapter.set_callback_on_scan_found([this](SimpleBLE::Peripheral peripheral) {
        if (peripheral.identifier().size() > 0) {
            owner.addPeripheral(peripheral);

            const MessageManagerLock mmLock;
            table.updateContent();
        }
    });
    adapter.set_callback_on_scan_updated([this](SimpleBLE::Peripheral peripheral) {
        owner.addPeripheral(peripheral);

        const MessageManagerLock mmLock;
        table.updateContent();
    });
    adapter.scan_start();

    startTimer(20000);
}

void DeviceList::stopScan()
{
    SimpleBLE::Adapter &adapter = owner.getAdapter();
    adapter.scan_stop();
    adapter.set_callback_on_scan_start(nullptr);
    adapter.set_callback_on_scan_stop(nullptr);
    adapter.set_callback_on_scan_found(nullptr);
    adapter.set_callback_on_scan_updated(nullptr);
}

void DeviceList::timerCallback() {
    stopTimer();
    stopScan();
}

void DeviceList::resized()
{
    table.setBoundsInset (BorderSize<int> (10));
}

void DeviceList::onConnected(int rowNumber)
{
    owner.setConnectedIndex(rowNumber - (isScanning ? 1 : 0));
}

int DeviceList::getNumRows()
{
    if (isFinished) {
        return 0;
    }
    return (isScanning ? 1 : 0) + owner.getPeripherals().size();
}

Component* DeviceList::refreshComponentForCell (int rowNumber, int columnId, bool /*isRowSelected*/, Component* existingComponentToUpdate)
{
    if ((!isScanning || rowNumber > 0) && columnId == 2)
    {
        auto* connectComp = static_cast<ConnectColumnComponent*> (existingComponentToUpdate);

        if (connectComp == nullptr)
            connectComp = new ConnectColumnComponent (*this);

        connectComp->setRowAndColumn (rowNumber, columnId);
        return connectComp;
    }

    jassert (existingComponentToUpdate == nullptr);
    return nullptr;
}

int DeviceList::getColumnAutoSizeWidth (int columnId)
{
    switch (columnId) {
        case 1:
            return 196;
        case 2:
        default:
            return 80;
    }
}