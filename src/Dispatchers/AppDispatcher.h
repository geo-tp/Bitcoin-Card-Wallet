#ifndef APP_DISPATCHER_H
#define APP_DISPATCHER_H

#include <Contexts/GlobalContext.h>
#include <Contexts/SelectionContext.h>
#include <Contexts/EntropyContext.h>
#include <Enums/SelectionModeEnum.h>
#include <Services/LedService.h>
#include <Services/WalletService.h>
#include <Services/CryptoService.h>
#include <Services/SdService.h>
#include <Services/UsbService.h>
#include <Repositories/WalletRepository.h>
#include <Selections/ModeSelection.h>
#include <Selections/FilePathSelection.h>
#include <Selections/MnemonicSelection.h>
#include <Selections/FilePathSelection.h>
#include <Selections/KeyboardLayoutSelection.h>
#include <Selections/ValueSelection.h>
#include <Controllers/ModeController.h>
#include <Controllers/WalletController.h>
#include <Controllers/SeedController.h>
#include <Controllers/FileBrowserController.h>

using namespace controllers;
using namespace contexts;
using namespace enums;
using namespace repositories;
using namespace selections;
using namespace services;

namespace dispatchers {

class AppDispatcher {
public:
    AppDispatcher(CardputerView& display, CardputerInput& input);
    
    void setup();
    void run();

private:
    CardputerView& display;
    CardputerInput& input;

    // Contexts
    GlobalContext& globalContext;
    SelectionContext& selectionContext;
    EntropyContext& entropyContext;

    // Selections
    ModeSelection modeSelection;
    WalletSelection walletSelection;
    StringPromptSelection stringPromptSelection;
    ConfirmationSelection confirmationSelection;
    WalletInformationSelection walletInformationSelection;
    MnemonicSelection mnemonicSelection;
    FilePathSelection filePathSelection;
    KeyboardLayoutSelection keyboardLayoutSelection;
    ValueSelection valueSelection;

    // Repositories
    WalletRepository walletRepository;

    // Services
    WalletService walletService;
    CryptoService cryptoService;
    LedService ledService;
    SdService sdService;
    UsbService usbService;

    // Controllers
    ModeController modeController;
    WalletController walletController;
    SeedController seedController;
    FileBrowserController fileBrowserController;
};

} // namespace dispatchers

#endif // APP_DISPATCHER_H
