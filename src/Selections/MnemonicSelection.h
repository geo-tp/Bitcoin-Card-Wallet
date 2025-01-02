#ifndef MNEMONIC_SELECTION_H
#define MNEMONIC_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <vector>
#include <string>

using namespace views;
using namespace inputs;

namespace selections {

class MnemonicSelection {
private:
    CardputerView& display;
    CardputerInput& input;
    size_t currentIndex = 0;

public:
    MnemonicSelection(CardputerView& display, CardputerInput& input);

    /**
     * Affiche chaque mot de la phrase mnémonique un par un
     * @param mnemonic La phrase mnémonique (vecteur de mots)
     */
    void select(const std::vector<std::string>& mnemonic);
};

} // namespace selections

#endif // MNEMONIC_SELECTION_H
