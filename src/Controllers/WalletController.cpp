#include "WalletController.h"

namespace controllers {

WalletController::WalletController(CardputerView& display, 
                                   CardputerInput& input, 
                                   WalletService& walletService,
                                   UsbService& usbService,
                                   WalletSelection& walletSelection,
                                   KeyboardLayoutSelection& keyboardLayoutSelection,
                                   ConfirmationSelection& confirmationSelection,
                                   StringPromptSelection& stringPromptSelection,
                                   WalletInformationSelection& walletInformationSelection,
                                   ValueSelection& valueSelection)
    : display(display), input(input), walletService(walletService), keyboardLayoutSelection(keyboardLayoutSelection), 
    usbService(usbService), walletSelection(walletSelection), confirmationSelection(confirmationSelection), 
    stringPromptSelection(stringPromptSelection), walletInformationSelection(walletInformationSelection), valueSelection(valueSelection) {}

void WalletController::handleWalletSelection() {
    auto wallets = walletService.getAllWallets();

    // No wallets currently in the repo, ask for loading wallets file from the SD card
    if (wallets.empty()) {
        auto confirmation = confirmationSelection.select("Load wallets file?");

        if (confirmation) {
            selectionContext.setCurrentSelectedMode(SelectionModeEnum::LOAD_WALLET);
        } else {
            selectionContext.setIsModeSelected(false); // Go back to menu
        }
        return;
    }

    // Select the desired wallet
    auto selectedWallet = walletSelection.select(wallets);
    
    // User hits the return button if the returned wallet is empty
    if (selectedWallet.empty()) {
        selectionContext.setIsModeSelected(false); // Go back to menu
        return;
    }
    
    // Go to the next step
    selectionContext.setCurrentSelectedWallet(selectedWallet);
    selectionContext.setIsWalletSelected(true);
}

void WalletController::handleWalletInformationSelection() {
    // Get the selected wallet
    auto selectedWallet = selectionContext.getCurrentSelectedWallet();
    const uint8_t* selectedLayout = nullptr; // for keyboard layout

    // Select between Balance, Btc Address, PubKey
    WalletInformationSelection walletInfoSelection(display, input);
    auto selectedInfo = walletInfoSelection.select(selectedWallet.getName());

    // Init usb keyboard
    switch (selectedInfo) {
        case WalletInformationEnum::BALANCE:
        case WalletInformationEnum::ADDRESS:
        case WalletInformationEnum::PUBLIC_KEY:
            // Keyboard layout is not selected, this means usb keyboard is not init
            if (!selectionContext.getIsLayoutSelected()) {
                // Select keyboard layout and init it
                selectedLayout = keyboardLayoutSelection.select();
                usbService.setLayout(selectedLayout);
                usbService.begin();
                selectionContext.setIsLayoutSelected(true);
            }
            break;
    }
    
    // Route to the selected wallet infos
    switch (selectedInfo) {
        case WalletInformationEnum::NONE: // when key return is hits
            selectionContext.setIsWalletSelected(false); // go back to wallet selection
            break;

        case WalletInformationEnum::BALANCE:
            valueSelection.select(
                "Balance", 
                globalContext.getBitcoinBalanceUrl() + selectedWallet.getAddress(), 
                usbService
            );
            break;

        case WalletInformationEnum::ADDRESS:
            valueSelection.select(
                "Address", 
                selectedWallet.getAddress(), 
                usbService
            );
            break;

        case WalletInformationEnum::PUBLIC_KEY:
            valueSelection.select(
                "Public Key", 
                selectedWallet.getPublicKey(), 
                usbService
            );
            break;
    }
}

} // namespace controllers
