#ifndef SEED_CONTROLLER_H
#define SEED_CONTROLLER_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Services/CryptoService.h>
#include <Services/WalletService.h>
#include <Services/SdService.h>
#include <Models/Wallet.h>
#include <Selections/MnemonicSelection.h>
#include <Selections/StringPromptSelection.h>
#include <Selections/ConfirmationSelection.h>
#include <Contexts/SelectionContext.h>
#include <Contexts/GlobalContext.h>
#include <vector>
#include <string>

using namespace services;
using namespace views;
using namespace inputs;
using namespace selections;
using namespace models;

namespace controllers {

class SeedController {
private:
    CardputerView& display;
    CardputerInput& input;
    CryptoService& cryptoService;
    WalletService& walletService;
    SdService& sdService;
    MnemonicSelection& mnemonicSelection;
    StringPromptSelection& stringPromptSelection;
    ConfirmationSelection& confirmationSelection;
    SelectionContext& selectionContext = SelectionContext::getInstance();
    GlobalContext& globalContext = GlobalContext::getInstance();
public:
    SeedController(CardputerView& display, 
                   CardputerInput& input, 
                   CryptoService& cryptoService,
                   WalletService& walletService,
                   SdService& sdService,
                   MnemonicSelection& mnemonicSelection,
                   StringPromptSelection& stringPromptSelection,
                   ConfirmationSelection& confirmationSelection);

    void handleSeedGeneration();
    void handleSeedInformations();
    void manageMnemonic(std::vector<std::string>& mnemonic);
    void manageSdSave(Wallet wallet);
    std::string managePassphrase();
};

} // namespace controllers

#endif // SEED_CONTROLLER_H
