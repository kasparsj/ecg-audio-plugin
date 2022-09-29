#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

using namespace juce;

class ModalWindow : public DialogWindow
{
public:
    ModalWindow (LaunchOptions& options)
        : DialogWindow (options.dialogTitle, options.dialogBackgroundColour,
            options.escapeKeyTriggersCloseButton, true,
            options.componentToCentreAround != nullptr
                ? Component::getApproximateScaleFactorForComponent (options.componentToCentreAround)
                : 1.0f)
    {
        if (options.content.willDeleteObject())
            setContentOwned (options.content.release(), true);
        else
            setContentNonOwned (options.content.release(), true);

        centreAroundComponent (options.componentToCentreAround, getWidth(), getHeight());
        setResizable (options.resizable, options.useBottomRightCornerResizer);

        setUsingNativeTitleBar (options.useNativeTitleBar);
        setAlwaysOnTop (true);
    }

    void closeButtonPressed() override
    {
        setVisible (false);
        if( onClose ) { onClose(); }
    }

    std::function<void()> onClose;

private:
    JUCE_DECLARE_NON_COPYABLE (ModalWindow)
};

