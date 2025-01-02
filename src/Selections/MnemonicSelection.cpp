#include "MnemonicSelection.h"

namespace selections {

MnemonicSelection::MnemonicSelection(CardputerView& display, CardputerInput& input)
    : display(display), input(input) {}

void MnemonicSelection::select(const std::vector<std::string>& mnemonic) {
    char key = KEY_NONE;
    size_t lastIndex = -1;

    while (key != KEY_ESC_CUSTOM) {
        if (lastIndex != currentIndex) {
            display.displayMnemonicWord(mnemonic[currentIndex], currentIndex);
            lastIndex = currentIndex;
        }

        key = input.handler();
        switch (key) {
            case KEY_ARROW_RIGHT: // Arrow right
                if (currentIndex < mnemonic.size() - 1) {
                    currentIndex++;
                }
                break;
            case KEY_RETURN_CUSTOM: // Arrow left
                if (currentIndex > 0) {
                    currentIndex--;
                }
                break;
        }
    }

    currentIndex = 0; // reset index
}

} // namespace selections
