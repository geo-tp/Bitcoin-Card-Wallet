#include "StringPromptSelection.h"

namespace selections {

StringPromptSelection::StringPromptSelection(CardputerView& display, CardputerInput& input)
    : display(display), input(input) {}

std::string StringPromptSelection::select(std::string description, size_t offsetX, bool backButton, bool password) {
    std::string output;
    char key = KEY_NONE;
    auto limit = globalContext.getMaxInputCharCount();
    display.displayStringPrompt(description, output, offsetX, backButton);

    if (password) {
        limit = globalContext.getMaxInputCharPasswordCount();
    }

    while (key != KEY_OK || output.length() < 3) {
        key = input.handler();
        if (key == KEY_DEL) {
            if (!output.empty()) {
                output.pop_back();
            }
        }
        else if (key == KEY_RETURN_CUSTOM && backButton) {
            return ""; // empty string will not save
        }
        else if (password && key != KEY_NONE && output.size() < limit) {
            output += key;
        }
        else if (isalnum(key) && output.size() < limit) {
            output += key;
        }

        if (key != KEY_NONE) {
            display.displayStringPrompt(description, output, offsetX, backButton);
        }
    }
    return output;
}

}