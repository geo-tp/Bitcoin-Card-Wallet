#include "KeyboardLayoutSelection.h"

namespace selections {

KeyboardLayoutSelection::KeyboardLayoutSelection(CardputerView& display, CardputerInput& input)
    : display(display), input(input) {}

const uint8_t* KeyboardLayoutSelection::select() {
    char key = KEY_NONE;
    size_t lastIndex = -1;

    display.displayTopBar("Select layout", false, false, false, 15);

    while (key != KEY_OK) {
        if (lastIndex != currentIndex) {
            display.displayKeyboardLayout(layouts[currentIndex].first);
            lastIndex = currentIndex;
        }

        key = input.handler();
        switch (key) {
            case KEY_ARROW_RIGHT: // Arrow right
                currentIndex = (currentIndex + 1) % layouts.size();
                break;
            case KEY_RETURN_CUSTOM: // Arrow left
                currentIndex = (currentIndex == 0) ? layouts.size() - 1 : currentIndex - 1;
                break;
            case KEY_OK:
                return layouts[currentIndex].second;
        }
    }

    // Default
    return KeyboardLayout_en_US;
}

} // namespace selections
