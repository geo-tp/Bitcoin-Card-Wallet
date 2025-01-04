#ifndef SEED_CONTROLLER_H
#define SEED_CONTROLLER_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Services/CryptoService.h>
#include <Services/WalletService.h>
#include <Services/SdService.h>
#include <Services/LedService.h>
#include <Services/RfidService.h>
#include <Models/Wallet.h>
#include <Selections/MnemonicSelection.h>
#include <Selections/StringPromptSelection.h>
#include <Selections/ConfirmationSelection.h>
#include <Selections/SeedRestorationSelection.h>
#include <Contexts/SelectionContext.h>
#include <Contexts/GlobalContext.h>
#include <Enums/SeedRestorationModeEnum.h>
#include <vector>
#include <string>

using namespace services;
using namespace views;
using namespace inputs;
using namespace selections;
using namespace models;
using namespace enums;

namespace controllers {

class SeedController {
private:
    CardputerView& display;
    CardputerInput& input;
    CryptoService& cryptoService;
    WalletService& walletService;
    SdService& sdService;
    RfidService& rfidService;
    LedService& ledService;
    MnemonicSelection& mnemonicSelection;
    StringPromptSelection& stringPromptSelection;
    ConfirmationSelection& confirmationSelection;
    SeedRestorationSelection& seedRestorationSelection;
    SelectionContext& selectionContext = SelectionContext::getInstance();
    GlobalContext& globalContext = GlobalContext::getInstance();
public:
    SeedController(CardputerView& display, 
                   CardputerInput& input, 
                   CryptoService& cryptoService,
                   WalletService& walletService,
                   SdService& sdService,
                   RfidService& rfidService,
                   LedService& ledService,
                   MnemonicSelection& mnemonicSelection,
                   StringPromptSelection& stringPromptSelection,
                   ConfirmationSelection& confirmationSelection,
                   SeedRestorationSelection& seedRestorationSelection);

    void handleSeedGeneration();
    void handleSeedInformations();
    void handleSeedRestoration();
    void manageMnemonic(std::vector<std::string>& mnemonic);
    void manageSdSave(Wallet wallet);
    bool manageSdConfirmation();
    void manageRfidSave(std::vector<uint8_t> privateKey);
    std::vector<uint8_t>manageRfidRead();
    std::vector<uint8_t> managePrivateKey();
    std::tuple<std::vector<uint8_t>, std::string> manageEncryption(std::vector<uint8_t> privateKey);
    std::vector<uint8_t> manageDecryption();
    std::string managePassphrase();
    std::string confirmStringsMatch(const std::string& prompt1, const std::string& prompt2, const std::string& mismatchMessage);
};

} // namespace controllers

#endif // SEED_CONTROLLER_H
