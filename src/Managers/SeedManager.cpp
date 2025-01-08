#include "SeedManager.h"
#include <algorithm>
#include <stdexcept>

namespace managers {

SeedManager::SeedManager(const GlobalManager& gm)
    : GlobalManager(gm) // calls GlobalManager's copy constructor
{}

std::vector<uint8_t> SeedManager::managePrivateKey() {
  // Generate private keys and verify randomness
  std::vector<uint8_t> privateKey;
  do {
    privateKey = cryptoService.generatePrivateKey();
  } while (cryptoService.calculateShanonEntropy(privateKey) < 4.9);

  return privateKey;
}

void SeedManager::manageMnemonicRead(std::vector<std::string>& mnemonic) {
  bool mnemonicIsBackedUp = false;
  bool mnemonicVerification = false;
  std::string word;
  uint8_t randomNumber;

  do {
    // Show the 24 words
    display.displayTopBar("Write Seed", false, false, true);
    mnemonicSelection.select(mnemonic);

    // Verify Backup
    mnemonicVerification = confirmationSelection.select(" Verify backup?");
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
      display.displaySubMessage("Seed backup done", 36, 2000);
      mnemonicIsBackedUp = true;
    }
  } while (!mnemonicIsBackedUp);
}

std::vector<std::string> SeedManager::manageMnemonicWrite(size_t wordCount) {
    std::vector<std::string> mnemonicWords;
    mnemonicWords.reserve(wordCount);

    display.displaySeedRestorationInfos();
    input.waitPress();
    
    // Get each word
    for (size_t i = 0; i < wordCount; ++i) {
        std::string word = mnemonicRestoreSelection.select(i, wordCount);
        mnemonicWords.push_back(word);
    }

    // Ensure all words are non empty
    bool allWordsFilled = std::all_of(mnemonicWords.begin(), mnemonicWords.end(),
                                     [](const std::string& word) { return !word.empty(); });
    if (!allWordsFilled) {
        return {};
    }

    // Check if valid mnemonic
    auto mnemonicString = cryptoService.mnemonicVectorToString(mnemonicWords);
    auto mnemonicWordList = cryptoService.mnemonicStringToWordList(mnemonicString);
    auto privateKey = cryptoService.mnemonicToPrivateKey(mnemonicString);
    if (mnemonicString.empty() || !cryptoService.verifyMnemonic(mnemonicWordList)) {
      return {};
    }

    return mnemonicWords;
}

bool SeedManager::manageMnemonicRestore(size_t wordCount) {
    std::vector<std::string> mnemonicWords;
    mnemonicWords.reserve(wordCount);
    
    // Get each word
    auto mnemonic = manageMnemonicWrite(wordCount);
    if (mnemonic.empty()) { // invalid mnemonic will return empty object
      display.displaySubMessage("Invalid mnemonic", 41, 2000);
      return false;
    }

    // Convert
    auto mnemonicString = cryptoService.mnemonicVectorToString(mnemonic);
    auto mnemonicWordList = cryptoService.mnemonicStringToWordList(mnemonicString);
    auto privateKey = cryptoService.mnemonicToPrivateKey(mnemonicString);

    // Valid mnemonic
    display.displayTopBar("Restore seed", false, false, true, 5);
    display.displaySubMessage("Valid mnemonic", 45, 2000);

    // Passphrase
    auto passphrase = managePassphrase(); // return "" in case user doesn't want passphrase

    // Save RFID
    manageRfidSave(privateKey);

    // Prompt for a wallet name
    display.displayTopBar("Wallet", false, false, true);
    auto walletName = stringPromptSelection.select("Enter wallet name");
    if (walletName.empty()) {return false;}
    auto wallet = manageBitcoinWalletCreation(mnemonicString, passphrase, walletName);

    // Save wallet to SD if any
    display.displaySubMessage("Loading", 83);
    sdService.begin(); // SD card start
    manageSdSave(wallet);

    // Display seed save infos
    display.displaySeedEnd(sdService.getSdState());
    input.waitPress();

    sdService.close(); // SD card stop
    
    // Go to portfolio
    selectionContext.setCurrentSelectedMode(SelectionModeEnum::PORTFOLIO);
    return true;
}

std::vector<std::string> SeedManager::manageMnemonicLoading(size_t wordCount) {
    // Get the words from user
    auto mnemonic = manageMnemonicWrite(wordCount);
    if (mnemonic.empty()) { // not valid mnemonic will return empty object
        display.displaySubMessage("Invalid mnemonic", 41, 2000);
        return {};
    }

    // At this point mnemonic is valid
    display.displayTopBar("Restore seed", false, false, true, 5);
    display.displaySubMessage("Valid mnemonic", 45, 2000);

    // Get Wallet
    auto wallet = selectionContext.getCurrentSelectedWallet();

    // Get seed passphrase
    auto passphrase = managePassphrase();

    // Derive PublicKey to check if seed match
    display.displaySubMessage("Loading", 83);
    auto mnemonicString = cryptoService.mnemonicVectorToString(mnemonic);
    auto publicKey = cryptoService.deriveZPub(mnemonicString, passphrase);
    if (publicKey.toString().c_str() != wallet.getZPub()) {
      display.displaySubMessage("seed/wallet mismatch", 18, 3000);
      selectionContext.setTransactionOngoing(false);
      return {};
    }

    // Update
    wallet.setPassphrase(passphrase);
    wallet.setMnemonic(mnemonicString);
    selectionContext.setCurrentSelectedWallet(wallet);

    // Go to file browser
    selectionContext.setCurrentSelectedMode(SelectionModeEnum::LOAD_SD);
    selectionContext.setCurrentSelectedFileType(FileTypeEnum::TRANSACTION);
    display.displaySubMessage("Select xpub psbt file", 28, 3000);

    sdService.close(); // SD card stop
    
    return mnemonic;
}

bool SeedManager::manageRfidSeedSignature() {
    // Display infos about module
    display.displayPlugRfid();
    input.waitPress();

    // Get the private key from tag
    auto privateKey = manageRfidRead();
    if (privateKey.empty()) {return false;} // user hits return

    // Convert to mnemonic 24 words
    auto mnemonic = cryptoService.privateKeyToMnemonic(privateKey);

    // Bad seed if empty
    if (mnemonic.empty()) {
      return false;
    }

    // Get Wallet
    auto wallet = selectionContext.getCurrentSelectedWallet();

    // Get seed passphrase
    auto passphrase = managePassphrase();
    wallet.setPassphrase(passphrase);

    // Derive PublicKey to check if seed match
    display.displaySubMessage("Loading", 83);
    auto mnemonicString = cryptoService.mnemonicVectorToString(mnemonic);
    auto publicKey = cryptoService.deriveZPub(mnemonicString, passphrase);
    if (publicKey.toString().c_str() != wallet.getZPub()) {
      display.displaySubMessage("seed/wallet mismatch", 18, 4000);
      selectionContext.setCurrentSelectedMode(SelectionModeEnum::PORTFOLIO);
      selectionContext.setTransactionOngoing(false);
      return false;
    }

    // Valid seed
    display.displaySubMessage("Seed is valid", 63, 1000);
    display.displaySubMessage("1st word: " + mnemonic[0], 49, 3000);

    // Set mnemonic to wallet
    wallet.setMnemonic(mnemonicString);
    selectionContext.setCurrentSelectedWallet(wallet);

    // Go to file browser
    display.displaySubMessage("Loading", 83);
    selectionContext.setCurrentSelectedMode(SelectionModeEnum::LOAD_SD);
    selectionContext.setCurrentSelectedFileType(FileTypeEnum::TRANSACTION);

    display.displaySubMessage("Select xpub psbt file", 28, 3000);

    return true;
}

void SeedManager::manageRfidSeedRestoration() {
    // Display infos about module
    display.displayPlugRfid();
    input.waitPress();

    // Get the private key from tag
    auto privateKey = manageRfidRead();
    if (privateKey.empty()) {return;} // user hits return

    // Convert to mnemonic words
    auto mnemonic = cryptoService.privateKeyToMnemonic(privateKey);

    // Bad seed if empty
    if (!mnemonic.empty()) {
      display.displaySubMessage("Seed is valid", 63, 1000);
      display.displaySubMessage("1st word: " + mnemonic[0], 47, 3000);
    }

    // Passphrase
    auto mnemonicString = cryptoService.mnemonicVectorToString(mnemonic);
    auto passphrase = managePassphrase(); // return "" in case user doesn't want passphrase

    // Prompt for a wallet name
    display.displayTopBar("Wallet", false, false, true);
    auto walletName = stringPromptSelection.select("Enter wallet name");
    if (walletName.empty()) {return;}
    auto wallet = manageBitcoinWalletCreation(mnemonicString, passphrase, walletName);

    // Save wallet to SD if any
    display.displaySubMessage("Loading", 83);
    sdService.begin(); // SD card start
    manageSdSave(wallet);

    // Display seed save infos
    display.displaySeedEnd(sdService.getSdState());
    input.waitPress();

    sdService.close(); // SD card stop

    // Delete seed
    mnemonic.clear();
    privateKey.clear();
    mnemonicString.clear(); 

    // Go to Portfolio
    selectionContext.setCurrentSelectedMode(SelectionModeEnum::PORTFOLIO);
}

void SeedManager::manageNewSeedCreation() {
    // Display the top main bar with the btc icon
    display.displayTopBar("New Seed", false, false, true, 5);

    // Check if an SD is plugged
    auto confirmRunWithoutSd = manageSdConfirmation();
    if (!confirmRunWithoutSd) {
        selectionContext.setIsModeSelected(false);
        return;
    }

    // Prompt for a wallet name
    auto walletName = stringPromptSelection.select("Enter wallet name");

    // User hits return (empty name) => go back to menu
    if (walletName.empty()) {
        selectionContext.setIsModeSelected(false);
        return;
    }

    // Display seed infos
    display.displaySeedStart();
    input.waitPress();

    // Generate private key and verify randomness
    auto privateKey = managePrivateKey();

    // Create a mnemonic from private key
    auto mnemonic       = cryptoService.privateKeyToMnemonic(privateKey);
    auto mnemonicString = cryptoService.mnemonicVectorToString(mnemonic);

    // Let the user see the 24 words
    manageMnemonicRead(mnemonic);

    // Optional passphrase
    auto passphrase = managePassphrase(); // returns "" if user doesn't want passphrase

    auto wallet = manageBitcoinWalletCreation(mnemonicString, passphrase, walletName);

    // Save seed on an RFID tag
    manageRfidSave(privateKey);

    // Save wallet to SD if any
    display.displaySubMessage("Loading", 83);
    sdService.begin();
    manageSdSave(wallet);

    // Display seed save infos
    display.displaySeedEnd(sdService.getSdState());
    input.waitPress();

    sdService.close(); // stop SD

    // Delete sensitive data
    mnemonic.clear();
    privateKey.clear();
    mnemonicString.clear();

    // Go to Portfolio
    selectionContext.setCurrentSelectedMode(SelectionModeEnum::PORTFOLIO);
}

} // namespace managers
