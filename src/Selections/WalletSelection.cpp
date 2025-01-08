#include "WalletSelection.h"

namespace selections {

WalletSelection::WalletSelection(CardputerView& display, CardputerInput& input)
    : display(display), input(input) {}

Wallet WalletSelection::select(std::vector<Wallet> wallets) {
    if (wallets.empty()) {
        return Wallet(); // default empty wallet
    }

    this->wallets = wallets; // Stocker la liste locale
    lastIndex = -1;

    display.displayTopBar("Portfolio", true, false, true, 10);
    
    char key = KEY_NONE;
    while (key != KEY_OK && key != KEY_ARROW_RIGHT) {
        if (lastIndex != selectionIndex) {
            std::vector<std::string> walletNames;
            for (const auto& wallet : wallets) {
                walletNames.push_back(wallet.getName());
            }

            display.displaySelection(walletNames, selectionIndex, {}, true, true);
            lastIndex = selectionIndex;
        }

        key = input.handler();

        switch (key) {
            case KEY_ARROW_DOWN:
                selectionIndex = (selectionIndex < wallets.size() - 1) ? selectionIndex + 1 : 0;
                break;
            case KEY_ARROW_UP:
                selectionIndex = (selectionIndex > 0) ? selectionIndex - 1 : wallets.size() - 1;
                break;
            case KEY_RETURN_CUSTOM:
                selectionIndex = 0; // default
                return Wallet(); // empty wallet
        }
    }

    return wallets[selectionIndex];
}

} // namespace selections
