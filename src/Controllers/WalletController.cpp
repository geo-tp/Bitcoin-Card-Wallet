#include "WalletController.h"

namespace controllers {

WalletController::WalletController(WalletManager& manager) : manager(manager) {}

void WalletController::handleWalletSelection() {

    auto wallets = manager.walletService.getAllWallets();

    // No wallets currently in the repo, ask for loading wallets file from the SD card
    if (wallets.empty()) {
        auto confirmation = manager.confirmationSelection.select("Load wallets file?");

        if (confirmation) {
            selectionContext.setCurrentSelectedMode(SelectionModeEnum::LOAD_SD);
        } else {
            selectionContext.setIsModeSelected(false); // Go back to menu
        }
        return;
    }

    // Select the desired wallet
    auto selectedWallet = manager.walletSelection.select(wallets);
    
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
    auto selectedInfo = manager.walletInformationSelection.select(selectedWallet.getName());

    // Init usb keyboard
    switch (selectedInfo) {
        case WalletInformationEnum::BALANCE:
        case WalletInformationEnum::ADDRESS:
        case WalletInformationEnum::PUBLIC_KEY:
            // Keyboard layout is not selected, this means usb keyboard is not init
            if (!selectionContext.getIsLayoutSelected()) {
                // Select keyboard layout and init it
                selectedLayout = manager.keyboardLayoutSelection.select();
                manager.usbService.setLayout(selectedLayout);
                manager.usbService.begin();
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
            manager.valueSelection.select(
                "Balance", 
                globalContext.getBitcoinBalanceUrl() + selectedWallet.getPublicKey(), 
                manager.usbService
            );
            break;

        case WalletInformationEnum::ADDRESS:
            manager.valueSelection.select(
                "Address", 
                selectedWallet.getAddress(), 
                manager.usbService
            );
            break;

        case WalletInformationEnum::PUBLIC_KEY:
            manager.valueSelection.select(
                "Public Key", 
                selectedWallet.getPublicKey(), 
                manager.usbService
            );
            break;

        case WalletInformationEnum::SIGNATURE:
            auto selectedWallet = selectionContext.getCurrentSelectedWallet();
            selectionContext.setTransactionOngoing(true);
            if(selectedWallet.getMnemonic().empty()) {
                selectionContext.setCurrentSelectedMode(SelectionModeEnum::LOAD_SEED);
            } else {
                selectionContext.setCurrentSelectedMode(SelectionModeEnum::LOAD_SD);
                selectionContext.setCurrentSelectedFileType(FileTypeEnum::TRANSACTION);
            }
            
            selectionContext.setTransactionOngoing(true);
            break;
    }
}

} // namespace controllers
