# HR/ECG audio plugin.

This audio plugin is based on [Pamplejuce](https://github.com/sudara/pamplejuce/actions) template. 
Pamplejuce is a template for creating and building JUCE plugins with CMake & CLion.

The initial idea was to build an audio plugin with which it would be possible to record live ECG (Electrocardiography) data into a DAW. For that I chose to work with Polar Bluetooth sensors, namely H9, H10, OH1 and Verity Sense, because these are very user friendly when compared to alternatives. Also they have an open source mobile SDK, which could be used as a source of reference.

For Bluetooth I chose to work with [SimpleBLE](https://github.com/OpenBluetoothToolbox/SimpleBLE) library, which also supports CMake and therefore was easy to integrate into the project. IMHO SimpleBLE has also an easy to learnd and understand API.

## Current state

Currently the audio plugin works only with HR as a source for automation (implemented as JUCE::AudioProcessorParameter) or as a parameter link (automation modulation source). ECG is not yet working. The reason for that was a previously unforeseen difficulty with receiving ECG data. Namely the MacOs CoreBluetooth framework does not support manually changing the MTU size for a Bluetooth connection. Althouhgh Linux and Windows do support it, at the time of writing also the SimpleBLE library does not support this feature [yet](https://github.com/OpenBluetoothToolbox/SimpleBLE/issues/83). The Polar sensors require this (to be set to 256 or a larger value), because they can stream live ECG data only using this private/specialized API (it's called PMD).

[Demo Video](https://www.youtube.com/watch?v=l_BRtQtrjJE)

## Source code

All source files are located inside the Source folder.

## Building

1. `git clone https://github.com/kasparsj/ecg-audio-plugin.git`
2. `cd ecg-audio-plugin`
3. `git submodule update --init`
4. `mkdir build && cd build`
5. `cmake ../`
