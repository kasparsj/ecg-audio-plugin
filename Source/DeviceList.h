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

    int getNumRows() override;

    void paintRowBackground (Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override
    {
        auto alternateColour = getLookAndFeel().findColour (ListBox::backgroundColourId)
                                   .interpolatedWith (getLookAndFeel().findColour (ListBox::textColourId), 0.03f);
        if (rowIsSelected)
            g.fillAll (Colours::lightblue);
        else if (rowNumber % 2)
            g.fillAll (alternateColour);
    }

    void paintCell (Graphics& g, int rowNumber, int columnId,
        int width, int height, bool rowIsSelected) override
    {
        g.setColour (rowIsSelected ? Colours::darkblue : getLookAndFeel().findColour (ListBox::textColourId));
        g.setFont (font);

        if (isScanning && rowNumber == 0) {
            if (columnId == 1) {
                g.drawText ("Scanning...", 2, 0, width - 4, height, Justification::centredLeft, true);
                g.setColour (getLookAndFeel().findColour (ListBox::backgroundColourId));
                g.fillRect (width - 1, 0, 1, height);
            }
        }
        else {
            if (columnId == 1) {
                SimpleBLE::Peripheral& peripheral = peripherals.at (rowNumber - (isScanning ? 1 : 0));
                g.drawText (peripheral.identifier(), 2, 0, width - 4, height, Justification::centredLeft, true);
                g.setColour (getLookAndFeel().findColour (ListBox::backgroundColourId));
                g.fillRect (width - 1, 0, 1, height);
            }
        }
    }

    void sortOrderChanged (int newSortColumnId, bool isForwards) override
    {
        table.updateContent();
    }

    Component* refreshComponentForCell (int rowNumber, int columnId, bool /*isRowSelected*/, Component* existingComponentToUpdate) override;
    int getColumnAutoSizeWidth (int columnId) override;

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeviceList)
};
