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

    startTimer(1000);
}

void DeviceList::timerCallback() {
    table.updateContent();
}

void DeviceList::resized()
{
    table.setBoundsInset (BorderSize<int> (10));
}

void DeviceList::setConnecting(int rowNumber)
{
    owner.setActiveIndex (rowNumber - (owner.getIsScanning() ? 1 : 0));
}

int DeviceList::getNumRows()
{
    return (owner.getIsScanning() ? 1 : 0) + owner.getPeripherals().size();
}

Component* DeviceList::refreshComponentForCell (int rowNumber, int columnId, bool /*isRowSelected*/, Component* existingComponentToUpdate)
{
    if ((!owner.getIsScanning() || rowNumber > 0) && columnId == 2)
    {
        auto* connectComp = static_cast<ConnectColumnComponent*> (existingComponentToUpdate);

        if (connectComp == nullptr)
            connectComp = new ConnectColumnComponent (*this);

        connectComp->setRowAndColumn (rowNumber, columnId);
        return connectComp;
    }
    else if (existingComponentToUpdate != nullptr) {
        delete existingComponentToUpdate;
        existingComponentToUpdate = nullptr;
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