#ifndef KEYBOARDLAYOUTSELECTION_H
#define KEYBOARDLAYOUTSELECTION_H

#include <vector>
#include <string>
#include <Arduino.h>
#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <USBHIDKeyboard.h>

using namespace views;
using namespace inputs;

namespace selections {

class KeyboardLayoutSelection {
public:
    KeyboardLayoutSelection(CardputerView& display, CardputerInput& input);

    // Affiche les layouts et renvoie le pointeur du layout selectionné
    const uint8_t* select();

private:
    CardputerView& display;
    CardputerInput& input;
    size_t currentIndex = 0;

    std::vector<std::pair<std::string, const uint8_t*>> layouts = {
        {"English (US)", KeyboardLayout_en_US},
        {"French (FR)", KeyboardLayout_fr_FR},
        {"German (DE)", KeyboardLayout_de_DE},
        {"Spanish (ES)", KeyboardLayout_es_ES},
        {"            Italian (IT)", KeyboardLayout_it_IT}, // hack to align well on screen
        {"Portuguese (PT)", KeyboardLayout_pt_PT},
        {"Portuguese (BR)", KeyboardLayout_pt_BR},
        {"Swedish (SE)", KeyboardLayout_sv_SE},
        {"Danish (DK)", KeyboardLayout_da_DK},
        {"Hungarian (HU)", KeyboardLayout_hu_HU}
    };
};

} // namespace selections

#endif // KEYBOARDLAYOUTSELECTION_H
