#include "StringPromptSelection.h"

namespace selections {

StringPromptSelection::StringPromptSelection(CardputerView& display, CardputerInput& input)
    : display(display), input(input) {}

std::string StringPromptSelection::select(std::string description, size_t offsetX, bool backButton) {
    std::string output;
    char key = KEY_NONE;
    display.displayStringPrompt(description, output, offsetX, backButton);

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
        else if (isalnum(key) && output.size() < globalContext.getMaxInputCharCount()) {
            output += key;
        }

        if (key != KEY_NONE) {
            display.displayStringPrompt(description, output, offsetX, backButton);
        }
    }

    return output;
}

}