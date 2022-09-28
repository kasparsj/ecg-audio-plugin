#pragma once

#include "juce_gui_basics/juce_gui_basics.h"
#include "BLEOwner.h"

using namespace juce;

class DeviceList : public Component, private TableListBoxModel, public Timer
{
public:
    DeviceList(BLEOwner &owner);
    ~DeviceList() {
        stopScan();
    }

    void startScan();
    void stopScan();
    void timerCallback() override;
    void resized() override;
    void connect(int rowNumber);
    int getConnectedTo() { return connectedTo; }

private:
    std::function<void(SimpleBLE::Peripheral)> callback;
    TableListBox table;
    Font font           { 14.0f };
    bool isScanning;
    int connectedTo = -1;
    BLEOwner &owner;
    std::vector<SimpleBLE::Peripheral> peripherals;

    int getNumRows() override
    {
        if (connectedTo > -1) {
            return 0;
        }
        return (isScanning ? 1 : 0) + peripherals.size();
    }

    void paintRowBackground (juce::Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override
    {
        auto alternateColour = getLookAndFeel().findColour (juce::ListBox::backgroundColourId)
                                   .interpolatedWith (getLookAndFeel().findColour (juce::ListBox::textColourId), 0.03f);
        if (rowIsSelected)
            g.fillAll (juce::Colours::lightblue);
        else if (rowNumber % 2)
            g.fillAll (alternateColour);
    }

    void paintCell (juce::Graphics& g, int rowNumber, int columnId,
        int width, int height, bool rowIsSelected) override
    {
        g.setColour (rowIsSelected ? juce::Colours::darkblue : getLookAndFeel().findColour (juce::ListBox::textColourId));
        g.setFont (font);

        if (isScanning && rowNumber == 0) {
            if (columnId == 1) {
                g.drawText ("Scanning...", 2, 0, width - 4, height, juce::Justification::centredLeft, true);
                g.setColour (getLookAndFeel().findColour (juce::ListBox::backgroundColourId));
                g.fillRect (width - 1, 0, 1, height);
            }
        }
        else {
            if (columnId == 1) {
                SimpleBLE::Peripheral& peripheral = peripherals.at (rowNumber - (isScanning ? 1 : 0));
                g.drawText (peripheral.identifier(), 2, 0, width - 4, height, juce::Justification::centredLeft, true);
                g.setColour (getLookAndFeel().findColour (juce::ListBox::backgroundColourId));
                g.fillRect (width - 1, 0, 1, height);
            }
        }
    }

    void sortOrderChanged (int newSortColumnId, bool isForwards) override
    {
        table.updateContent();
    }

    class ConnectColumnComponent  : public Component
    {
    public:
        ConnectColumnComponent (DeviceList& td)
            : owner (td)
        {
            addAndMakeVisible (connectButton);
            connectButton.setButtonText ("Connect");

            connectButton.onClick = [this] {
                owner.connect(row);
            };
        }

        void resized() override
        {
            connectButton.setBoundsInset (BorderSize<int> (1));
        }

        void setRowAndColumn (int newRow, int newColumn)
        {
            row = newRow;
            columnId = newColumn;

            if (row == owner.getConnectedTo()) {
                connectButton.setButtonText ("Connected");
            }
        }
    private:
        DeviceList& owner;
        TextButton connectButton;
        int row, columnId;
    };

    Component* refreshComponentForCell (int rowNumber, int columnId, bool /*isRowSelected*/,
        Component* existingComponentToUpdate) override
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

    int getColumnAutoSizeWidth (int columnId) override
    {
        switch (columnId) {
            case 1:
                return 200;
            case 2:
            default:
                return 100;
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeviceList)
};
