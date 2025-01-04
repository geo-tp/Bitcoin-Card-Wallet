#include "SeedRestorationSelection.h"

namespace selections {

SeedRestorationSelection::SeedRestorationSelection(CardputerView& display, CardputerInput& input)
    : display(display), input(input) {}

SeedRestorationModeEnum SeedRestorationSelection::select() {
    display.displaySelection(getModeStrings(), selectionIndex, getModeDescriptionStrings());

    char key = KEY_NONE;
    while (key != KEY_OK && key != KEY_ARROW_RIGHT) {
        if (lastIndex != selectionIndex) {
            display.displaySelection(getModeStrings(), selectionIndex, getModeDescriptionStrings());
            lastIndex = selectionIndex;
        }

        key = input.handler();
        switch (key) {
            case KEY_ARROW_DOWN:
                selectionIndex = (selectionIndex < static_cast<uint8_t>(SeedRestorationModeEnum::COUNT) - 1) ? selectionIndex + 1 : 0;
                break;
            case KEY_ARROW_UP:
                selectionIndex = (selectionIndex > 0) ? selectionIndex - 1 : static_cast<uint8_t>(SeedRestorationModeEnum::COUNT) - 1;
                break;
            case KEY_RETURN_CUSTOM:
                return SeedRestorationModeEnum::NONE;
        }
    }

    return static_cast<SeedRestorationModeEnum>(selectionIndex);
}

const std::string SeedRestorationSelection::getModeToString(SeedRestorationModeEnum mode) const {
    switch (mode) {
        case SeedRestorationModeEnum::RFID:
            return "RFID TAG";
        case SeedRestorationModeEnum::SD:
            return "SD CARD";
        case SeedRestorationModeEnum::WORDS_24:
            return "24 WORDS";
        default:
            return "UNKNOWN";
    }
}

const std::string SeedRestorationSelection::getModeDescription(SeedRestorationModeEnum mode) const {
    switch (mode) {
        case SeedRestorationModeEnum::RFID:
            return "       from RFID tag";
        case SeedRestorationModeEnum::SD:
            return "            from SD card";
        case SeedRestorationModeEnum::WORDS_24:
            return "   from mnemonic";
        default:
            return "UNKNOWN";
    }
}

const std::vector<std::string> SeedRestorationSelection::getModeStrings() const {
    std::vector<std::string> modeStrings;
    for (int i = 0; i < static_cast<int>(SeedRestorationModeEnum::COUNT); ++i) {
        modeStrings.push_back(getModeToString(static_cast<SeedRestorationModeEnum>(i)));
    }
    return modeStrings;
}

const std::vector<std::string> SeedRestorationSelection::getModeDescriptionStrings() const {
    std::vector<std::string> descriptionStrings;
    for (int i = 0; i < static_cast<int>(SeedRestorationModeEnum::COUNT); ++i) {
        descriptionStrings.push_back(getModeDescription(static_cast<SeedRestorationModeEnum>(i)));
    }
    return descriptionStrings;
}

} // namespace selections
