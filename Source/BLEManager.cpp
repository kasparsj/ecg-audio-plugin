#include "BLEManager.h"

void BLEManager::startScan() {
    adapter.set_callback_on_scan_start([this]() {
        isScanning = true;
    });
    adapter.set_callback_on_scan_stop([this]() {
        isScanning = false;
    });
    adapter.set_callback_on_scan_found([this](SimpleBLE::Peripheral peripheral) {
        if (peripheral.identifier().size() > 0) {
            addPeripheral(peripheral);
        }
    });
    adapter.set_callback_on_scan_updated([this](SimpleBLE::Peripheral peripheral) {
        addPeripheral(peripheral);
    });
    adapter.scan_start();
}

void BLEManager::stopScan()
{
    adapter.scan_stop();
    adapter.set_callback_on_scan_start(nullptr);
    adapter.set_callback_on_scan_stop(nullptr);
    adapter.set_callback_on_scan_found(nullptr);
    adapter.set_callback_on_scan_updated(nullptr);
}