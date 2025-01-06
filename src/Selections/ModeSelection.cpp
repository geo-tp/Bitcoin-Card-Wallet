#include "ModeSelection.h"

namespace selections {

ModeSelection::ModeSelection(CardputerView& display, CardputerInput& input)
    : display(display), input(input) {}

SelectionModeEnum ModeSelection::select() {
    display.displayTopBar(globalContext.getAppName());
    display.drawBitcoinIcon(20, 3);
    display.displaySelection(getSelectionModeStrings(), selectionIndex, getSelectionModeDescriptionStrings());
    selectionIndex = 0;
    lastIndex = -1;
    
    char key = KEY_NONE;
    while (key != KEY_OK && key != KEY_ARROW_RIGHT) {
        if (lastIndex != selectionIndex) {
            display.displaySelection(getSelectionModeStrings(), selectionIndex, getSelectionModeDescriptionStrings());
            lastIndex = selectionIndex;
        }

        key = input.handler();
        switch (key) {
            case KEY_ARROW_DOWN:
                selectionIndex = (selectionIndex < static_cast<uint8_t>(SelectionModeEnum::COUNT) - 1) ? selectionIndex + 1 : 0;
                break;
            case KEY_ARROW_UP:
                selectionIndex = (selectionIndex > 0) ? selectionIndex - 1 : static_cast<uint8_t>(SelectionModeEnum::COUNT) - 1;
                break;
        }
    }

    return static_cast<SelectionModeEnum>(selectionIndex);
}


const std::string ModeSelection::getSelectionModeToString(SelectionModeEnum mode) {
    switch (mode) {
        case SelectionModeEnum::PORTFOLIO:
            return "PORTFOLIO";
        case SelectionModeEnum::CREATE_WALLET:
            return "NEW WALLET";
        case SelectionModeEnum::LOAD_SD:
            return "LOAD WALLET";
        case SelectionModeEnum::LOAD_SEED:
            return "RESTORE SEED";
        case SelectionModeEnum::INFOS:
            return "      INFORMATIONS";
        default:
            return "UNKNOWN";
    }
}

const std::string ModeSelection::getSelectionModeDescription(SelectionModeEnum mode) {
    switch (mode) {
        case SelectionModeEnum::PORTFOLIO:
            return "      btc wallets";
        case SelectionModeEnum::CREATE_WALLET:
            return "     btc seed";
        case SelectionModeEnum::LOAD_SD:
            return "  from sd";
        case SelectionModeEnum::LOAD_SEED:
            return "secret";
        case SelectionModeEnum::INFOS:
            return "";
        default:
            return "UNKNOWN";
    }
}

const std::vector<std::string> ModeSelection::getSelectionModeStrings() {
    std::vector<std::string> modeStrings;
    for (int i = 0; i < static_cast<int>(SelectionModeEnum::COUNT); ++i) {
        modeStrings.push_back(getSelectionModeToString(static_cast<SelectionModeEnum>(i)));
    }
    return modeStrings;
}

const std::vector<std::string> ModeSelection::getSelectionModeDescriptionStrings() {
    std::vector<std::string> descriptionStrings;
    for (int i = 0; i < static_cast<int>(SelectionModeEnum::COUNT); ++i) {
        descriptionStrings.push_back(getSelectionModeDescription(static_cast<SelectionModeEnum>(i)));
    }
    return descriptionStrings;
}

}
