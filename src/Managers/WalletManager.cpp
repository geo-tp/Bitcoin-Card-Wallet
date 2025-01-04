#include "WalletManager.h"

namespace managers {

WalletManager::WalletManager(CardputerView& display, 
                             CardputerInput& input, 
                             WalletService& walletService, 
                             UsbService& usbService, 
                             WalletSelection& walletSelection, 
                             KeyboardLayoutSelection& keyboardLayoutSelection, 
                             ConfirmationSelection& confirmationSelection, 
                             StringPromptSelection& stringPromptSelection, 
                             WalletInformationSelection& walletInformationSelection, 
                             ValueSelection& valueSelection)
    : display(display), input(input), walletService(walletService), usbService(usbService), 
      walletSelection(walletSelection), keyboardLayoutSelection(keyboardLayoutSelection), 
      confirmationSelection(confirmationSelection), stringPromptSelection(stringPromptSelection), 
      walletInformationSelection(walletInformationSelection), valueSelection(valueSelection) {}

} // namespace controllers