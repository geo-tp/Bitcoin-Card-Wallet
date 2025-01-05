#ifndef GLOBAL_MANAGER_H
#define GLOBAL_MANAGER_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Services/CryptoService.h>
#include <Services/WalletService.h>
#include <Services/SdService.h>
#include <Services/LedService.h>
#include <Services/RfidService.h>
#include <Services/UsbService.h>
#include <Models/Wallet.h>
#include <Selections/MnemonicSelection.h>
#include <Selections/MnemonicRestoreSelection.h>
#include <Selections/StringPromptSelection.h>
#include <Selections/ConfirmationSelection.h>
#include <Selections/SeedRestorationSelection.h>
#include <Selections/KeyboardLayoutSelection.h>
#include <Selections/WalletSelection.h>
#include <Selections/WalletInformationSelection.h>
#include <Selections/FilePathSelection.h>
#include <Selections/ValueSelection.h>
#include <Contexts/SelectionContext.h>
#include <Contexts/GlobalContext.h>
#include <vector>
#include <string>
#include <tuple>

using namespace services;
using namespace contexts;
using namespace models;
using namespace inputs;
using namespace views;
using namespace selections;

namespace managers {

class GlobalManager {
public:
    CardputerView& display;
    CardputerInput& input;
    CryptoService& cryptoService;
    WalletService& walletService;
    SdService& sdService;
    UsbService& usbService;
    RfidService& rfidService;
    LedService& ledService;
    MnemonicSelection& mnemonicSelection;
    MnemonicRestoreSelection& mnemonicRestoreSelection;
    StringPromptSelection& stringPromptSelection;
    ConfirmationSelection& confirmationSelection;
    SeedRestorationSelection& seedRestorationSelection;
    FilePathSelection& filePathSelection;
    KeyboardLayoutSelection& keyboardLayoutSelection;
    WalletSelection& walletSelection;
    WalletInformationSelection& walletInformationSelection;
    ValueSelection& valueSelection;
    contexts::SelectionContext& selectionContext = SelectionContext::getInstance();
    contexts::GlobalContext& globalContext       = GlobalContext::getInstance();

    GlobalManager(CardputerView& display,
                  CardputerInput& input,
                  CryptoService& cryptoService,
                  WalletService& walletService,
                  SdService& sdService,
                  RfidService& rfidService,
                  LedService& ledService,
                  UsbService& usbService,
                  MnemonicSelection& mnemonicSelection,
                  MnemonicRestoreSelection& mnemonicRestoreSelection,
                  StringPromptSelection& stringPromptSelection,
                  ConfirmationSelection& confirmationSelection,
                  SeedRestorationSelection& seedRestorationSelection,
                  FilePathSelection& filePathSelection,
                  KeyboardLayoutSelection& keyboardLayoutSelection,
                  WalletSelection& walletSelection,
                  WalletInformationSelection& walletInformationSelection,
                  ValueSelection& valueSelection);

    GlobalManager(const GlobalManager& other);

    bool manageSdConfirmation();
    void manageSdSave(Wallet wallet);
    std::string managePassphrase();
    std::string confirmStringsMatch(const std::string& prompt1, 
                                    const std::string& prompt2, 
                                    const std::string& mismatchMessage);

    std::tuple<std::vector<uint8_t>, std::string> manageEncryption(std::vector<uint8_t> privateKey);
    std::vector<uint8_t> manageDecryption();

    void manageRfidSave(std::vector<uint8_t> privateKey);
    std::vector<uint8_t> manageRfidRead();
};

} // namespace managers

#endif // GLOBAL_MANAGER_H
