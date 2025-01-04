#include "SeedManager.h"
#include <algorithm>
#include <stdexcept>

namespace managers {

SeedManager::SeedManager(CardputerView& display, 
                         CardputerInput& input, 
                         CryptoService& cryptoService,
                         WalletService& walletService,
                         SdService& sdService,
                         RfidService& rfidService,
                         LedService& ledService,
                         MnemonicSelection& mnemonicSelection,
                         StringPromptSelection& stringPromptSelection,
                         ConfirmationSelection& confirmationSelection,
                         SeedRestorationSelection& seedRestorationSelection)
    : display(display), input(input), cryptoService(cryptoService), walletService(walletService), sdService(sdService), 
      rfidService(rfidService), ledService(ledService), mnemonicSelection(mnemonicSelection), stringPromptSelection(stringPromptSelection),  
      confirmationSelection(confirmationSelection), seedRestorationSelection(seedRestorationSelection)  {}

std::vector<uint8_t> SeedManager::managePrivateKey() {
  // Generate private keys and verify randomness
  std::vector<uint8_t> privateKey;
  do {
    privateKey = cryptoService.generatePrivateKey();
  } while (cryptoService.calculateShanonEntropy(privateKey) < 4.9);

  return privateKey;
}

bool SeedManager::manageSdConfirmation() {
    // SD card start
  display.displaySubMessage("Loading", 83); // sd can take time to response
  sdService.begin(); 

  // Display 'no SD card
  if (!sdService.getSdState()) {
    auto confirmation = confirmationSelection.select("No SD card found");
    if (!confirmation) {
      selectionContext.setIsModeSelected(false); // go back to menu
      sdService.close(); // SD card stop
      return false;
    }
  }

  // SD card stop
  sdService.close();
  return true;
}

void SeedManager::manageSdSave(Wallet wallet) {
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

std::string SeedManager::confirmStringsMatch(const std::string& prompt1, const std::string& prompt2, const std::string& mismatchMessage) {
    std::string input1, input2;
    auto defaultMaxCharLimit = globalContext.getMaxInputCharCount();
    globalContext.setMaxInputCharCount(128); // 128 chars max for passphrase
    do {
        input1 = stringPromptSelection.select(prompt1, 0, false);
        input2 = stringPromptSelection.select(prompt2, 4, false);

        if (input1 != input2) {
            display.displaySubMessage(mismatchMessage, 58, 2000);
        }
    } while (input1 != input2);
    globalContext.setMaxInputCharCount(defaultMaxCharLimit);
    return input1;
}

std::string SeedManager::managePassphrase() {
  auto passConfirmation = confirmationSelection.select("Add passphrase?");
  std::string passphrase = "";


  if (passConfirmation) {
    passphrase = confirmStringsMatch("Enter passphrase", "Repeat passphrase", "Do not math");
    display.displaySubMessage("Passphrase set", 46, 2000);
  }

  return passphrase;
}

std::tuple<std::vector<uint8_t>, std::string> SeedManager::manageEncryption(std::vector<uint8_t> privateKey) {
    display.displaySubMessage("Loading", 83, 800); // Add some time to avoid double input
    auto encryptConfirmation = confirmationSelection.select("Encrypt the seed?");
    if (!encryptConfirmation) { 
        return {privateKey, ""};
    }

    // Get salt and encrypt key
    auto salt = cryptoService.getRandomString(16); // 16 bytes, not chars
    auto password = confirmStringsMatch("Enter a password", " Repeat password", "Do not match");
    display.displaySubMessage("Loading", 83);
    auto encryptedKey = cryptoService.encryptPrivateKeyWithPassphrase(privateKey, password, salt);

    return {encryptedKey, salt};
}

std::vector<uint8_t> SeedManager::manageDecryption() {
    // Get private key, salt and signature
    auto privateKey = rfidService.getPrivateKey();
    auto salt = rfidService.getSalt();
    auto sign = rfidService.getSignature();

    auto saltIsEmpty = std::all_of(salt.begin(), salt.end(), [](int value) { return value == 0; });
    bool validation = false;
    while (!validation && privateKey.size() == 32 && !saltIsEmpty) {
      auto password = stringPromptSelection.select("Enter the password", 8);
      display.displaySubMessage("Loading", 83);
      auto decryptedKey = cryptoService.decryptPrivateKeyWithPassphrase(privateKey, password, salt);
      auto generatedSign = cryptoService.generateSignature(decryptedKey, salt);

      validation = sign == generatedSign;
      if(validation) {
        privateKey = decryptedKey;
        display.displaySubMessage("Seed decrypted", 48, 2000);
      } else {
        display.displaySubMessage("Bad password", 55, 1500);
      }
    }
    return privateKey;
}

void SeedManager::manageRfidSave(std::vector<uint8_t> privateKey) {
  // Display RFID
  display.displaySeedRfid();
  input.waitPress();
  display.displayTopBar("MIFARE 1K", false, false, false);

  // Confirm RFID
  auto rfidConfirmation = confirmationSelection.select("Seed on RFID tag?");
  if (!rfidConfirmation) { return; }
  
  // Init RFID
  rfidService.initialize();

  // Get salt, key, sign
  std::vector<uint8_t> returnedKey;
  std::string salt;
  std::tie(returnedKey, salt) = manageEncryption(privateKey);
  auto signature = cryptoService.generateSignature(privateKey, salt);
  auto splittedKey = cryptoService.splitVector(returnedKey); // block are 16 bytes for a 32 bytes keys
  
  display.displaySubMessage("PLUG YOUR TAG", 43);
  const unsigned long timeout = 5000; // 5 seconds
  unsigned long startTime = millis();

  while (true) {
    if (millis() - startTime > timeout) {
        // Ask confirmation to continue each 5 sec
        auto continueProcess = confirmationSelection.select("Retry saving seed?");
        if (!continueProcess) {
            display.displaySubMessage("RFID save cancelled", 30, 2000);
            break;
        }
        startTime = millis();
        display.displaySubMessage("PLUG YOUR TAG", 43);
    }

    // No tag detected
    if (!rfidService.isCardPresent()) {
        delay(300);
        continue;
    }

    ledService.blink();

    // Save private key
    auto privateKeySaved = rfidService.savePrivateKey(splittedKey.first, splittedKey.second);
    if (!privateKeySaved) {
        display.displaySubMessage("Failed to save key", 38, 1000);
        continue;
    }

    // Save salt with zeros if no encryption
    auto saltSaved = rfidService.saveSalt(salt);
    if (!saltSaved) {
        display.displaySubMessage("Failed to save salt", 34, 1000);
        continue;
    }

    // Save sign as a checksum for data
    auto signSaved = rfidService.saveSignature(signature);
    if (!saltSaved) {
        display.displaySubMessage("Failed to save sign", 34, 1000);
        continue;
    }

    display.displaySubMessage("Seed is saved", 60, 2000);
    return;
  }
  rfidService.end();
}

std::vector<uint8_t> SeedManager::manageRfidRead() {
  std::vector<uint8_t> privateKey;
  display.displayTopBar("MIFARE 1K", false, false, false);
  display.displaySubMessage("PLUG YOUR TAG", 43);

  rfidService.initialize();

  const unsigned long timeout = 5000; // 5 seconds
  unsigned long startTime = millis();

  while (true) {
    if (millis() - startTime > timeout) {
        // Ask confirmation to continue each 5 sec
        auto continueProcess = confirmationSelection.select("Retry reading tag?");
        if (!continueProcess) {
            display.displaySubMessage("RFID read cancelled", 30, 2000);
            return privateKey;
        }
        startTime = millis();
        display.displaySubMessage("PLUG YOUR TAG", 43);
    }

    // No tag detected
    if (!rfidService.isCardPresent()) {
        delay(300);
        continue;
    }

    ledService.blink();
    return manageDecryption();
  }
}

} // namespace managers
