#ifndef WALLET_MANAGER_H
#define WALLET_MANAGER_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Services/WalletService.h>
#include <Services/UsbService.h>
#include <Selections/WalletSelection.h>
#include <Selections/StringPromptSelection.h>
#include <Selections/ConfirmationSelection.h>
#include <Selections/KeyboardLayoutSelection.h>
#include <Selections/WalletInformationSelection.h>
#include <Selections/ValueSelection.h>
#include <Contexts/GlobalContext.h>
#include <Contexts/SelectionContext.h>
#include <Enums/WalletInformationEnum.h>

using namespace selections;
using namespace services;
using namespace contexts;

namespace managers {

class WalletManager {
public:
    WalletManager(CardputerView& display, 
                     CardputerInput& input, 
                     WalletService& walletService,
                     UsbService& usbService,
                     WalletSelection& walletSelection,
                     KeyboardLayoutSelection& keyboardLayoutSelection,
                     ConfirmationSelection& confirmationSelection,
                     StringPromptSelection& stringPromptSelection,
                     WalletInformationSelection& walletInformationSelection,
                     ValueSelection& valueSelection);

    CardputerView& display;
    CardputerInput& input;
    WalletService& walletService;
    UsbService& usbService;
    WalletSelection& walletSelection;
    ConfirmationSelection& confirmationSelection;
    StringPromptSelection& stringPromptSelection;
    KeyboardLayoutSelection& keyboardLayoutSelection;
    WalletInformationSelection& walletInformationSelection;
    ValueSelection& valueSelection;
    GlobalContext& globalContext = GlobalContext::getInstance();
    SelectionContext& selectionContext = SelectionContext::getInstance();
};

} // namespace managers

#endif // WALLET_CONTROLLER_H
