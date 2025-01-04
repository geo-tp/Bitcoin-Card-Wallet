#ifndef SEED_RESTORATION_SELECTION_H
#define SEED_RESTORATION_SELECTION_H

#include <vector>
#include <string>
#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Enums/SeedRestorationModeEnum.h>
#include <Contexts/GlobalContext.h>

using namespace views;
using namespace inputs;
using namespace enums;
using namespace contexts;

namespace selections {

class SeedRestorationSelection {
private:
    CardputerView& display;
    CardputerInput& input;
    GlobalContext& globalContext = GlobalContext::getInstance();
    uint8_t selectionIndex = 0;
    uint8_t lastIndex = -1;

    const std::string getModeToString(SeedRestorationModeEnum mode) const;
    const std::string getModeDescription(SeedRestorationModeEnum mode) const;
    const std::vector<std::string> getModeStrings() const;
    const std::vector<std::string> getModeDescriptionStrings() const;

public:
    SeedRestorationSelection(CardputerView& display, CardputerInput& input);
    SeedRestorationModeEnum select();
};

} // namespace selections

#endif // SEED_RESTORATION_SELECTION_H
