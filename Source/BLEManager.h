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
    SimpleBLE::Peripheral& getActivePeripheral() { return peripherals[activeIndex]; }
    virtual void setActiveIndex (int i) {
        if (i != activeIndex && activeIndex >= 0 && peripherals[activeIndex].is_connected()) {
            peripherals[activeIndex].disconnect();
        }
        if (i >= 0) {
            peripherals[i].connect();
        }
        activeIndex = i;
    }

    void startScan();
    void stopScan();
    bool getIsScanning() {
        return isScanning;
    }

protected:
    SimpleBLE::Adapter adapter;
    std::vector<SimpleBLE::Peripheral> peripherals;
    int activeIndex = -1;
    bool isScanning;
};
