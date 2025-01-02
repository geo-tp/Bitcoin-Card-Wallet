#ifndef WALLET_SELECTION_H
#define WALLET_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Models/Wallet.h>
#include <vector>
#include <string>

using namespace views;
using namespace inputs;
using namespace models;

namespace selections {

class WalletSelection {
private:
    CardputerView& display;
    CardputerInput& input;
    std::vector<Wallet> wallets;
    uint8_t selectionIndex = 0; // Index actuel de sélection
    uint8_t lastIndex = 255;    // Dernier index affiché pour éviter un re-render inutile

public:
    WalletSelection(CardputerView& display, CardputerInput& input);
    Wallet select(std::vector<Wallet> wallets);
};

} // namespace selections

#endif // WALLET_SELECTION_H
