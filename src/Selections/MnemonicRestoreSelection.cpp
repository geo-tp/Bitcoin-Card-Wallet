#include "MnemonicRestoreSelection.h"

namespace selections {

MnemonicRestoreSelection::MnemonicRestoreSelection(CardputerView& display, CardputerInput& input)
    : display(display), input(input) {}

std::string MnemonicRestoreSelection::select(size_t index, size_t size) {
    std::string word;
    char key = KEY_NONE;
    size_t lastWordSize = -1;
    auto limit = 9;
    display.displayTopBar("Word " + std::to_string(index+1), false, false, false, 13);

    while (key != KEY_OK) {
        key = input.handler();

        if (word.size() != lastWordSize) {
            display.displayMnemonicWord(word, index, size, false, true);
            lastWordSize = word.size();
        }

        if (isalnum(key) && word.size() < limit) {
            word += key;
        }

        if (key == KEY_DEL) {
            if (!word.empty()) {
                word.pop_back();
            }
        }
    }

    return word;
}

} // namespace selections
