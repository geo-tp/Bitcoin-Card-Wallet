#include "AppDispatcher.h"

namespace dispatchers {

AppDispatcher::AppDispatcher(CardputerView& display, CardputerInput& input)
    : display(display), 
      input(input), 

      // OBJECT CREATION AND INJECTION
      
      // Contexts
      globalContext(GlobalContext::getInstance()),       // App variables
      selectionContext(SelectionContext::getInstance()), // User selection variables
      entropyContext(EntropyContext::getInstance()),     // Collect user entropy during app life cycle

      // Selections with injected dependencies
      modeSelection(display, input),                 // Select the main mode
      filePathSelection(display, input),             // Select a filepath from SD card
      walletSelection(display, input),               // Select a wallet from a wallets list
      stringPromptSelection(display, input),         // Return the user typed string
      confirmationSelection(display, input),         // Return the user confirmation
      walletInformationSelection(display, input),    // Select a wallet infos (Name, Address, PubKey)
      mnemonicSelection(display, input),             // Select one word from the 24 mnemonic words
      keyboardLayoutSelection(display, input),       // Select a kb layout for USB string injection
      valueSelection(display, input),                // Select between QRCode and USB typing
      seedRestorationSelection(display, input),      // Select restoration method

      // Repositories
      walletRepository(),                            // Repository for storing and loading wallets
      
      // Services
      walletService(walletRepository),               // Wallets logic with injected repo
      ledService(),                                  // Builtin LED logic
      cryptoService(),                               // Private keys, BTC address, encoding logic
      sdService(),                                   // SD Card logic
      usbService(),                                  // USB (keyboard) logic
      rfidService(),
      
      // Controllers with injected dependencies for routes handling
      modeController(display, input, modeSelection),

      walletController(display, input, walletService, usbService, walletSelection, keyboardLayoutSelection,
                       confirmationSelection, stringPromptSelection, walletInformationSelection, valueSelection),

      seedController(display, input, cryptoService, walletService, sdService, rfidService, ledService,
                     mnemonicSelection, stringPromptSelection, confirmationSelection, seedRestorationSelection),

      fileBrowserController(display, input, sdService, walletService, filePathSelection, confirmationSelection) { }

void AppDispatcher::setup() {
    display.initialize();
}

void AppDispatcher::run() {
    if (!selectionContext.getIsModeSelected()) {
        entropyContext.tick(); // this will generate entropy
        modeController.handleModeSelection();
        return;
    }

    switch (selectionContext.getCurrentSelectedMode()) {
        case SelectionModeEnum::PORTFOLIO:
            if (selectionContext.getIsWalletSelected()) {
                walletController.handleWalletInformationSelection();
            } else {
                walletController.handleWalletSelection();
            }
            break;

        case SelectionModeEnum::CREATE_WALLET:
            seedController.handleSeedGeneration();
            break;

        case SelectionModeEnum::LOAD_WALLET:
            fileBrowserController.handleFileWalletSelection();
            break;

        case SelectionModeEnum::LOAD_SEED:
            seedController.handleSeedRestoration();
            break;

        case SelectionModeEnum::INFOS:
            seedController.handleSeedInformations();
            break;
    }
}

} // namespace dispatchers
