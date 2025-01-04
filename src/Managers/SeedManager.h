#ifndef SEED_MANAGER_H
#define SEED_MANAGER_H

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
using namespace selections;

namespace managers {

class SeedManager {
public:
    SeedManager(CardputerView& display, 
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
    
    void manageMnemonic(std::vector<std::string>& mnemonic);
    std::tuple<std::vector<uint8_t>, std::string> manageEncryption(std::vector<uint8_t> privateKey);    
    std::vector<uint8_t> manageDecryption();
    std::vector<uint8_t> managePrivateKey();
    void manageRfidSave(std::vector<uint8_t> privateKey);
    std::vector<uint8_t>manageRfidRead();
    bool manageSdConfirmation();
    void manageSdSave(Wallet wallet);
    std::string managePassphrase();
    std::string confirmStringsMatch(const std::string& prompt1, const std::string& prompt2, const std::string& mismatchMessage);

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
};

} // namespace managers

#endif // SEED_MANAGER_H
