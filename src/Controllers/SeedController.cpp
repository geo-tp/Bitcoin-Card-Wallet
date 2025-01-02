#include "SeedController.h"

namespace controllers {

SeedController::SeedController(CardputerView& display, 
                               CardputerInput& input, 
                               CryptoService& cryptoService,
                               WalletService& walletService,
                               SdService& sdService,
                               MnemonicSelection& mnemonicSelection,
                               StringPromptSelection& stringPromptSelection,
                               ConfirmationSelection& confirmationSelection)
    : display(display), input(input), cryptoService(cryptoService), walletService(walletService), sdService(sdService),
      mnemonicSelection(mnemonicSelection), stringPromptSelection(stringPromptSelection),  confirmationSelection(confirmationSelection)  {}

void SeedController::handleSeedInformations() {
  display.displaySeedGeneralInfos();
  input.waitPress();
  display.displaySdSaveGeneralInfos();
  input.waitPress();
  display.displaySeedFormatGeneralInfos();
  input.waitPress();
  selectionContext.setIsModeSelected(false); // go back to menu
}

void SeedController::handleSeedGeneration() {
  // Display the top main bar with the btc icon
  display.displayTopBar("New Seed", false, false, true, 5);

  // SD card start
  display.displaySubMessage("Loading", 83); // sd can take time to response
  sdService.begin(); 

  // Display no SD card
  if (!sdService.getSdState()) {
    auto confirmation = confirmationSelection.select("No SD card found");
    if (!confirmation) {
      selectionContext.setIsModeSelected(false); // go back to menu
      sdService.close(); // SD card stop
      return;
    }
  }

  // SD card stop
  sdService.close();

  // Prompt for a wallet name
  auto walletName = stringPromptSelection.select("Enter wallet name");

  // User hits return during wallet selection
  if (walletName.empty()) {
    selectionContext.setIsModeSelected(false); // go back to menu
    return;
  }

  // Display seed infos
  display.displaySeedStart();
  input.waitPress();
  
  // Generate private keys and verify randomness
  std::vector<uint8_t> privateKey;
  do {
    // Generate keys
    privateKey = cryptoService.generatePrivateKey();
    // (4.9 for 32bits is equal to 90% randomness)
  } while (cryptoService.calculateShanonEntropy(privateKey) < 4.9);

  auto mnemonic = cryptoService.privateKeyToMnemonic(privateKey);
  auto mnemonicString = cryptoService.mnemonicVectorToString(mnemonic);
  // auto mnemonicString = "dragon reform deer execute fee tattoo wall barely loan jealous require student pipe bamboo solve toilet latin bargain escape spray scan stay father utility";

  // Mnemonic
  manageMnemonic(mnemonic);

  // Passphrase
  auto passphrase = managePassphrase(); // return "" in case user doesn't want passphrase

  // Derive PublicKey and create segwit BTC address
  display.displaySubMessage("Loading", 83);
  auto publicKey = cryptoService.derivePublicKey(mnemonicString, passphrase);
  auto address = cryptoService.generateBitcoinAddress(publicKey);

  // Delete seed
  mnemonic.clear();
  privateKey.clear();
  mnemonicString.clear(); 

  // Create Wallet
  Wallet wallet(walletName, publicKey.toString().c_str(), address);

  // Save wallet to SD if any
  sdService.begin(); // SD card start
  manageSdSave(wallet);

  // Display seed save infos
  display.displaySeedEnd(sdService.getSdState());
  input.waitPress();

  sdService.close(); // SD card stop

  // Go to Portfolio
  selectionContext.setCurrentSelectedMode(SelectionModeEnum::PORTFOLIO);
}

void SeedController::manageSdSave(Wallet wallet) {
  // Wallets filepath
  auto filePath = globalContext.getFileWalletPath();
  auto defaultPath = globalContext.getfileWalletDefaultPath();
  auto finalPath = filePath.empty() ? defaultPath : filePath;

  std::string fileContent;
  if (sdService.getSdState()) {
    fileContent = sdService.readFile(finalPath.c_str()); // old content
    walletService.loadAllWallets(fileContent); // in case it was not already loaded
    walletService.addWallet(wallet);
    fileContent = walletService.getWalletsFileContent(); // new content with the new wallet
    sdService.writeFile(finalPath.c_str(), fileContent); // save the new content to SD card
  } else {
      walletService.addWallet(wallet); // added in memory
  }
}

void SeedController::manageMnemonic(std::vector<std::string>& mnemonic) {
  bool mnemonicIsBackedUp = false;
  bool mnemonicVerification = false;
  std::string word;
  uint8_t randomNumber;

  do {
    // Show the 24 words
    display.displayTopBar("Write Seed", false, false, true);
    mnemonicSelection.select(mnemonic);

    // Verify Backup
    mnemonicVerification = confirmationSelection.select(" Verify backup ?");
    if (mnemonicVerification) {
      // Random num for a word index
      randomNumber = rand() % mnemonic.size();
      // Ask user the correct word for the given index
      display.displayTopBar("Verify Seed", false, false, true);
      auto question = "What's the " + std::to_string(randomNumber + 1) + " word ?";
      word = stringPromptSelection.select(question, 8);
    }

    // User want to verify the seed but words are different
    if (mnemonicVerification && word != mnemonic[randomNumber]) {
      if (!word.empty()) {display.displaySubMessage("Wrong answer", 50, 2000);}
    } else {
      display.displaySubMessage("Seed backup done", 38, 2000);
      mnemonicIsBackedUp = true;
    }
  } while (!mnemonicIsBackedUp);
}

std::string SeedController::managePassphrase() {
  auto passConfirmation = confirmationSelection.select("Add passphrase?");
  std::string passphrase1 = "";
  auto defaultMaxCharLimit = globalContext.getMaxInputCharCount();
  globalContext.setMaxInputCharCount(128); // 128 chars max for passphrase

  if (passConfirmation) {
    std::string passphrase2 = "";
    do {
      passphrase1 = stringPromptSelection.select("Enter passphrase", 0, false);
      passphrase2 = stringPromptSelection.select("Repeat passphrase", 4, false);

      if (passphrase1 != passphrase2) {
        display.displaySubMessage("Do not match", 58, 2000);
      }

    } while (passphrase1 != passphrase2);
    display.displaySubMessage("Passphrase set", 46, 2000);
  }

  globalContext.setMaxInputCharCount(defaultMaxCharLimit);
  return passphrase1;
}

} // namespace controllers
