#pragma once

#include "BLEManager.h"
#include "juce_gui_basics/juce_gui_basics.h"

using namespace juce;

class DeviceList : public Component, private TableListBoxModel, public Timer
{
public:
    DeviceList(BLEManager&owner);

    void timerCallback() override;
    void resized() override;
    void setConnecting(int rowNumber);
    SimpleBLE::Peripheral &getPeripheral(int rowNumber) {
        return owner.getPeripherals()[rowNumber - (owner.getIsScanning() ? 1 : 0)];
    }
    void setModelToNull() {
        table.setModel(nullptr);
    }

private:
    std::function<void(SimpleBLE::Peripheral)> callback;
    TableListBox table;
    Font font           { 14.0f };
    BLEManager& owner;

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

        if (owner.getIsScanning() && rowNumber == 0) {
            if (columnId == 1) {
                g.drawText ("Scanning...", 2, 0, width - 4, height, Justification::centredLeft, true);
                g.setColour (getLookAndFeel().findColour (ListBox::backgroundColourId));
                g.fillRect (width - 1, 0, 1, height);
            }
        }
        else {
            if (columnId == 1) {
                SimpleBLE::Peripheral& peripheral = getPeripheral(rowNumber);
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
        ConnectColumnComponent (DeviceList& td) : owner (td) {
            addAndMakeVisible (connectButton);
        }

        void resized() override
        {
            connectButton.setBoundsInset (BorderSize<int> (1));
        }

        void setRowAndColumn (int newRow, int newColumn)
        {
            row = newRow;
            columnId = newColumn;

            updateButton();

            connectButton.onClick = [this] {
                SimpleBLE::Peripheral& peripheral = owner.getPeripheral(row);
                if (peripheral.is_connected()) {
                    owner.setConnecting(-1);
                }
                else {
                    owner.setConnecting(row);
                }
            };
        }
    private:
        DeviceList& owner;
        TextButton connectButton;
        int row, columnId;

        void updateButton() {
            if (owner.getPeripheral(row).is_connected()) {
                connectButton.setButtonText ("Disconnect");
            }
            else {
                connectButton.setButtonText ("Connect");
            }
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConnectColumnComponent)
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeviceList)
};
