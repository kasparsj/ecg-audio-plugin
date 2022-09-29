#pragma once

#include <simpleble/SimpleBLE.h>

class BLEManager
{
public:
    SimpleBLE::Adapter& getAdapter() { return adapter; }
    void addPeripheral(SimpleBLE::Peripheral &newPeripheral) {
        for (int i=0; i<peripherals.size(); i++) {
            if (peripherals[i].identifier() == newPeripheral.identifier()) {
                peripherals[i] = newPeripheral;
                return;
            }
        }
        peripherals.push_back(newPeripheral);
    }
    std::vector<SimpleBLE::Peripheral>& getPeripherals() {
        return peripherals;
    }
    SimpleBLE::Peripheral& getPeripheral() { return peripherals[connectedIndex]; }
    virtual void setConnectedIndex(int i) {
        if (i != connectedIndex && connectedIndex >= 0 && peripherals[connectedIndex].is_connected()) {
            peripherals[connectedIndex].disconnect();
        }
        connectedIndex = i;
    }

protected:
    SimpleBLE::Adapter adapter;
    std::vector<SimpleBLE::Peripheral> peripherals;
    int connectedIndex = -1;
};
