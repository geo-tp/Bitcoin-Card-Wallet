#ifndef MNEMONIC_RESTORE_SELECTION_H
#define MNEMONIC_RESTORE_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Contexts/GlobalContext.h>
#include <string>
#include <vector>
#include <algorithm>

using namespace views;
using namespace inputs;
using namespace contexts;

namespace selections {

class MnemonicRestoreSelection {
public:
    MnemonicRestoreSelection(CardputerView& display, CardputerInput& input);
    std::string select(size_t index, size_t size=24);

private:
    CardputerView& display;
    CardputerInput& input;
    GlobalContext& globalContext = GlobalContext::getInstance();
};

} // namespace selections

#endif // MNEMONIC_RESTORE_SELECTION_H
