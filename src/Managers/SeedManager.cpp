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

void SeedManager::manageMnemonic(std::vector<std::string>& mnemonic) {
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

bool SeedManager::manageMnemonicRestore(size_t wordCount) {
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
        display.displaySubMessage("Invalid mnemonic", 41, 2000);
        return false;
    }

    // Check if valid mnemonic
    auto mnemonicString = cryptoService.mnemonicVectorToString(mnemonicWords);
    auto mnemonicWordList = cryptoService.mnemonicStringToWordList(mnemonicString);
    auto privateKey = cryptoService.mnemonicToPrivateKey(mnemonicString);
    if (mnemonicString.empty() || !cryptoService.verifyMnemonic(mnemonicWordList)) {
      display.displaySubMessage("Invalid mnemonic", 41, 2000);
      return false;
    }

    display.displayTopBar("Restore seed", false, false, true, 5);
    display.displaySubMessage("Valid mnemonic", 45, 2000);

    // Passphrase
    auto passphrase = managePassphrase(); // return "" in case user doesn't want passphrase

    // Save RFID
    manageRfidSave(privateKey);

    // Prompt for a wallet name
    display.displayTopBar("Wallet", false, false, true);
    auto walletName = stringPromptSelection.select("Enter wallet name");
    if (walletName.empty()) {
      return false;
    }

    // Derive PublicKey and create segwit BTC address
    display.displaySubMessage("Loading", 83);
    auto publicKey = cryptoService.derivePublicKey(mnemonicString, passphrase);
    auto address = cryptoService.generateBitcoinAddress(publicKey);

    // Create Wallet
    Wallet wallet(walletName, publicKey.toString().c_str(), address, mnemonicString);

    // Save wallet to SD if any
    display.displaySubMessage("Loading", 83);
    sdService.begin(); // SD card start
    manageSdSave(wallet);

    // Display seed save infos
    display.displaySeedEnd(sdService.getSdState());
    input.waitPress();

    sdService.close(); // SD card stop
    

    return true;
}

void SeedManager::manageRfidSeedRestoration() {
    // Display infos about module
    display.displayPlugRfid();
    input.waitPress();

    // Get the private key from tag
    auto privateKey = manageRfidRead();
    if (privateKey.size() != 32) {return;} // user hits return

    // Convert to mnemonic 24 words
    auto mnemonic = cryptoService.privateKeyToMnemonic(privateKey);

    // Bad seed if empty
    if (!mnemonic.empty()) {
      display.displaySubMessage("Seed is valid", 63, 1000);
      display.displaySubMessage("First word: " + mnemonic[0], 38, 3000);
    }

    // Passphrase
    auto passphrase = managePassphrase(); // return "" in case user doesn't want passphrase

    // Prompt for a wallet name
    display.displayTopBar("Wallet", false, false, true);
    auto walletName = stringPromptSelection.select("Enter wallet name");
    if (walletName.empty()) {
      return;
    }

    // Derive PublicKey and create segwit BTC address
    display.displaySubMessage("Loading", 83);
    auto mnemonicString = cryptoService.mnemonicVectorToString(mnemonic);
    auto publicKey = cryptoService.derivePublicKey(mnemonicString, passphrase);
    auto address = cryptoService.generateBitcoinAddress(publicKey);

    // Create Wallet
    Wallet wallet(walletName, publicKey.toString().c_str(), address);

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
    manageMnemonic(mnemonic);

    // Optional passphrase
    auto passphrase = managePassphrase(); // returns "" if user doesn't want passphrase

    // Derive PublicKey and create segwit BTC address
    display.displaySubMessage("Loading", 83);
    auto publicKey = cryptoService.derivePublicKey(mnemonicString, passphrase);
    auto address   = cryptoService.generateBitcoinAddress(publicKey);

    // Create a Wallet object
    Wallet wallet(walletName, publicKey.toString().c_str(), address);

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
