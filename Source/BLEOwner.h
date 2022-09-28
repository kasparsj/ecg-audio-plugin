#pragma once

#include <simpleble/SimpleBLE.h>

class BLEOwner
{
public:
    SimpleBLE::Adapter& getAdapter() { return adapter; }
    SimpleBLE::Peripheral& getPeripheral() { return peripheral; }
    virtual void setPeripheral(SimpleBLE::Peripheral &newPeripheral) { peripheral = newPeripheral; }

protected:
    SimpleBLE::Adapter adapter;
    SimpleBLE::Peripheral peripheral;
};
