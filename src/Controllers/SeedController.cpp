#include "SeedController.h"

namespace controllers {

SeedController::SeedController(SeedManager& seedManager) : manager(seedManager)  {}

void SeedController::handleSeedInformations() {
  manager.display.displaySeedGeneralInfos();
  manager.input.waitPress();
  manager.display.displaySdSaveGeneralInfos();
  manager.input.waitPress();
  manager.display.displaySeedFormatGeneralInfos();
  manager.input.waitPress();
  selectionContext.setIsModeSelected(false); // go back to menu
}

void SeedController::handleSeedGeneration() {
  // Display the top main bar with the btc icon
  manager.display.displayTopBar("New Seed", false, false, true, 5);

  // Check if an sd is plugged
  auto confirmRunWithoutSd = manager.manageSdConfirmation();
  if(!confirmRunWithoutSd) {return;}

  // Prompt for a wallet name
  auto walletName = manager.stringPromptSelection.select("Enter wallet name");

  // User hits return during wallet selection
  if (walletName.empty()) {
    selectionContext.setIsModeSelected(false); // go back to menu
    return;
  }

  // Display seed infos
  manager.display.displaySeedStart();
  manager.input.waitPress();
  
  // Generate private keys and verify randomness
  auto privateKey = manager.managePrivateKey();

  // Mnemonic
  auto mnemonic = manager.cryptoService.privateKeyToMnemonic(privateKey);
  auto mnemonicString = manager.cryptoService.mnemonicVectorToString(mnemonic);
  // auto mnemonicString = "dragon reform deer execute fee tattoo wall barely loan jealous require student pipe bamboo solve toilet latin bargain escape spray scan stay father utility";
  manager.manageMnemonic(mnemonic);

  // Passphrase
  auto passphrase = manager.managePassphrase(); // return "" in case user doesn't want passphrase

  // Derive PublicKey and create segwit BTC address
  manager.display.displaySubMessage("Loading", 83);
  auto publicKey = manager.cryptoService.derivePublicKey(mnemonicString, passphrase);
  auto address = manager.cryptoService.generateBitcoinAddress(publicKey);

  // Create Wallet
  Wallet wallet(walletName, publicKey.toString().c_str(), address);

  // Save seed on a RFID tag
  manager.manageRfidSave(privateKey);

  // Save wallet to SD if any
  manager.display.displaySubMessage("Loading", 83);
  manager.sdService.begin(); // SD card start
  manager.manageSdSave(wallet);

  // Display seed save infos
  manager.display.displaySeedEnd(manager.sdService.getSdState());
  manager.input.waitPress();

  manager.sdService.close(); // SD card stop

  // Delete seed
  mnemonic.clear();
  privateKey.clear();
  mnemonicString.clear(); 

  // Go to Portfolio
  selectionContext.setCurrentSelectedMode(SelectionModeEnum::PORTFOLIO);
}

void SeedController::handleSeedRestoration() {
  manager.display.displayTopBar("Load Seed", true, false, true, 15);
  auto restorationMethod = manager.seedRestorationSelection.select();

  if (restorationMethod == SeedRestorationModeEnum::NONE) {
      selectionContext.setIsModeSelected(false); // go to menu
      return;
  }

  std::vector<uint8_t> privateKey;
  std::vector<std::string> mnemonic;
  std::string walletName;
  std::string mnemonicString;
  std::string passphrase;
  std::string address;
  HDPublicKey publicKey;
  Wallet wallet;

  switch (restorationMethod) {
      case SeedRestorationModeEnum::NONE:
          selectionContext.setIsModeSelected(false); // go to menu

      case SeedRestorationModeEnum::RFID:
          // Display infos about module
          manager.display.displayPlugRfid();
          manager.input.waitPress();

          // Get the private key from tag
          privateKey = manager.manageRfidRead();
          if (privateKey.size() != 32) {return;} // user hits return

          // Convert to mnemonic 24 words
          mnemonic = manager.cryptoService.privateKeyToMnemonic(privateKey);

          // Bad seed if empty
          if (!mnemonic.empty()) {
            manager.display.displaySubMessage("Seed is valid", 63, 1000);
            manager.display.displaySubMessage("First word: " + mnemonic[0], 38, 3000);
          }

          // Passphrase
          passphrase = manager.managePassphrase(); // return "" in case user doesn't want passphrase

          // Prompt for a wallet name
          manager.display.displayTopBar("Wallet", false, false, true);
          walletName = manager.stringPromptSelection.select("Enter wallet name");
          if (walletName.empty()) {
            return;
          }

          // Derive PublicKey and create segwit BTC address
          manager.display.displaySubMessage("Loading", 83);
          mnemonicString = manager.cryptoService.mnemonicVectorToString(mnemonic);
          publicKey = manager.cryptoService.derivePublicKey(mnemonicString, passphrase);
          address = manager.cryptoService.generateBitcoinAddress(publicKey);

          // Create Wallet
          wallet.setName(walletName);
          wallet.setPublicKey(publicKey.toString().c_str());
          wallet.setAddress(address);

          // Save wallet to SD if any
          manager.display.displaySubMessage("Loading", 83);
          manager.sdService.begin(); // SD card start
          manager.manageSdSave(wallet);

          // Display seed save infos
          manager.display.displaySeedEnd(manager.sdService.getSdState());
          manager.input.waitPress();

          manager.sdService.close(); // SD card stop

          // Delete seed
          mnemonic.clear();
          privateKey.clear();
          mnemonicString.clear(); 

          // Go to Portfolio
          selectionContext.setCurrentSelectedMode(SelectionModeEnum::PORTFOLIO);
          break;

      case SeedRestorationModeEnum::SD:
          selectionContext.setCurrentSelectedFileType(FileTypeEnum::SEED);
          
           // Go to sd file browser
          selectionContext.setCurrentSelectedMode(SelectionModeEnum::LOAD_SD);
          break;

      case SeedRestorationModeEnum::WORDS_24:
          // handleRestorationFromWords();
          break;

      default:
          manager.display.displayDebug("Invalid resto mode");
          break;
  }

}

} // namespace controllers
