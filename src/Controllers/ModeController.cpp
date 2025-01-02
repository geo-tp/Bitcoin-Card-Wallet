#include "ModeController.h"

namespace controllers {

ModeController::ModeController(CardputerView& display, CardputerInput& input, ModeSelection& modeSelection)
    : display(display), input(input), modeSelection(modeSelection) {}

void ModeController::handleModeSelection() {
    SelectionModeEnum selectedMode = modeSelection.select();
    selectionContext.setCurrentSelectedMode(selectedMode);
    selectionContext.setIsModeSelected(true);
}

} // namespace controllers
