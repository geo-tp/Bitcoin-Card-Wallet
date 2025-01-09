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

    // Select between Balance, Btc Address, PubKey, Sign
    auto selectedInfo = manager.walletInformationSelection.select(selectedWallet.getName());

    // Init usb keyboard
    switch (selectedInfo) {
        case WalletInformationEnum::BALANCE:
        case WalletInformationEnum::ADDRESS:
        case WalletInformationEnum::PUBLIC_KEY:
        case WalletInformationEnum::FINGERPRINT:
        case WalletInformationEnum::DERIVE_PATH:
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
            selectionContext.setTransactionOngoing(false);
            selectionContext.setIsWalletSelected(false); // go back to wallet selection
            break;

        case WalletInformationEnum::BALANCE:
            manager.valueSelection.select(
                "Balance", 
                globalContext.getBitcoinBalanceUrl() + selectedWallet.getZPub(), 
                manager.usbService,
                manager.ledService
            );
            break;

        case WalletInformationEnum::ADDRESS:
            manager.valueSelection.select(
                "Address", 
                selectedWallet.getAddress(), 
                manager.usbService,
                manager.ledService
            );
            break;

        case WalletInformationEnum::SIGNATURE:
            selectionContext.setTransactionOngoing(true);
            if(selectedWallet.getMnemonic().empty()) {
                selectionContext.setCurrentSelectedMode(SelectionModeEnum::LOAD_SEED);
            } else {
                selectionContext.setCurrentSelectedMode(SelectionModeEnum::LOAD_SD);
                selectionContext.setCurrentSelectedFileType(FileTypeEnum::TRANSACTION);
            }            
            break;

        case WalletInformationEnum::PUBLIC_KEY:
            manager.valueSelection.select(
                "Public Zpub", 
                selectedWallet.getZPub(), 
                manager.usbService,
                manager.ledService
            );
            break;

        case WalletInformationEnum::FINGERPRINT:
            manager.valueSelection.select(
                "Fingerprint", 
                selectedWallet.getFingerprint(), 
                manager.usbService,
                manager.ledService
            );
            break;

        case WalletInformationEnum::DERIVE_PATH:
            manager.valueSelection.select(
                "Deriv Path", 
                selectedWallet.getDerivePath(), 
                manager.usbService,
                manager.ledService
            );
            break;
    }
}

} // namespace controllers
