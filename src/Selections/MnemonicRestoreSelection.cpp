#include "MnemonicRestoreSelection.h"

namespace selections {

MnemonicRestoreSelection::MnemonicRestoreSelection(CardputerView& display, CardputerInput& input)
    : display(display), input(input) {}

std::string MnemonicRestoreSelection::select(size_t index, size_t size) {
    std::string word;
    char key = KEY_NONE;
    size_t lastWordSize = -1;
    auto defaultMaxInput = globalContext.getMaxInputCharCount();
    globalContext.setMaxInputCharCount(8);
    display.displayTopBar("Word " + std::to_string(index+1), false, false, false, 13);

    while (key != KEY_OK) {
        key = input.handler();

        if (word.size() != lastWordSize) {
            display.displayMnemonicWord(word, index, size, false, true);
            lastWordSize = word.size();
        }

        if (isalnum(key) && word.size() < globalContext.getMaxInputCharCount()) {
            word += key;
        }

        if (key == KEY_DEL) {
            if (!word.empty()) {
                word.pop_back();
            }
        }
    }

    globalContext.setMaxInputCharCount(defaultMaxInput);
    return word;
}

} // namespace selections
