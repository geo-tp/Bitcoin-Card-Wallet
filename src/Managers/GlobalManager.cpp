#include "GlobalManager.h"
#include <algorithm>
#include <stdexcept>
#include <tuple>

namespace managers {

GlobalManager::GlobalManager(CardputerView& display,
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
                             ValueSelection& valueSelection
)
    : display(display),
      input(input),
      cryptoService(cryptoService),
      walletService(walletService),
      sdService(sdService),
      rfidService(rfidService),
      ledService(ledService),
      usbService(usbService),
      mnemonicSelection(mnemonicSelection),
      mnemonicRestoreSelection(mnemonicRestoreSelection),
      stringPromptSelection(stringPromptSelection),
      confirmationSelection(confirmationSelection),
      seedRestorationSelection(seedRestorationSelection),
      filePathSelection(filePathSelection),
      keyboardLayoutSelection(keyboardLayoutSelection),
      walletSelection(walletSelection),
      walletInformationSelection(walletInformationSelection),
      valueSelection(valueSelection)
{}

GlobalManager::GlobalManager(const GlobalManager& other)
    : display(other.display),
      input(other.input),
      cryptoService(other.cryptoService),
      walletService(other.walletService),
      sdService(other.sdService),
      rfidService(other.rfidService),
      ledService(other.ledService),
      usbService(other.usbService),
      mnemonicSelection(other.mnemonicSelection),
      mnemonicRestoreSelection(other.mnemonicRestoreSelection),
      stringPromptSelection(other.stringPromptSelection),
      confirmationSelection(other.confirmationSelection),
      seedRestorationSelection(other.seedRestorationSelection),
      filePathSelection(other.filePathSelection),
      keyboardLayoutSelection(other.keyboardLayoutSelection),
      walletSelection(other.walletSelection),
      walletInformationSelection(other.walletInformationSelection),
      valueSelection(other.valueSelection)
{}

bool GlobalManager::manageSdConfirmation() {
    // SD card start
  display.displaySubMessage("Loading", 83); // sd can take time to response
  sdService.begin(); 

  // Display 'no SD card'
  if (!sdService.getSdState()) {
    auto confirmation = confirmationSelection.select("No SD card found");
    if (!confirmation) {
      sdService.close(); // SD card stop
      return false;
    }
  }

  // SD card stop
  sdService.close();
  return true;
}

void GlobalManager::manageSdSave(Wallet wallet) {
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

std::string GlobalManager::managePassphrase() {
    auto passConfirmation = confirmationSelection.select("Add passphrase?");
    display.displaySubMessage("Loading", 83);

    std::string passphrase;
    if (passConfirmation) {
        passphrase = confirmStringsMatch("Enter passphrase", "Repeat passphrase", "Do not match");
        display.displaySubMessage("Passphrase set", 48, 2000);
    }
    return passphrase;
}

std::string GlobalManager::confirmStringsMatch(const std::string& prompt1, 
                                               const std::string& prompt2, 
                                               const std::string& mismatchMessage) 
{
    std::string input1, input2;
    auto defaultMaxCharLimit = globalContext.getMaxInputCharCount();
    globalContext.setMaxInputCharCount(128);
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

std::tuple<std::vector<uint8_t>, std::string> GlobalManager::manageRfidEncryption(std::vector<uint8_t> privateKey) {
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

std::vector<uint8_t> GlobalManager::manageRfidDecryption() {
    // Get private key, salt and signature
    auto privateKey = rfidService.getPrivateKey();
    ledService.blink(); // to signal RFID reading
    if (privateKey.empty()) {return {};}
    auto salt = rfidService.getSalt();
    auto sign = rfidService.getCheckSum(); 

    // Allow more chars
    auto defaultMaxCharLimit = globalContext.getMaxInputCharCount();
    globalContext.setMaxInputCharCount(128);

    // If salt is empty, the seed is not emcrypted
    auto saltIsEmpty = std::all_of(salt.begin(), salt.end(), [](int value) { return value == 0; });

    bool validation = false;
    while (!validation && privateKey.size() % 16 == 0 && !saltIsEmpty) {
      // Ask password
      auto password = stringPromptSelection.select("Enter the password", 8);
      if (password.empty()) {return {};} // return button

      // Decrypt
      display.displaySubMessage("Loading", 83);
      auto decryptedKey = cryptoService.decryptPrivateKeyWithPassphrase(privateKey, password, salt);
      auto generatedSign = cryptoService.generateChecksum(decryptedKey, salt);

      // Verify
      validation = sign == generatedSign;
      if(validation) {
        privateKey = decryptedKey;
        display.displaySubMessage("Seed decrypted", 48, 2000);
      } else {
        display.displaySubMessage("Bad password", 55, 1500);
      }
    }
    globalContext.setMaxInputCharCount(defaultMaxCharLimit);
    return privateKey;
}

void GlobalManager::manageRfidSave(std::vector<uint8_t> privateKey) {
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
  std::tie(returnedKey, salt) = manageRfidEncryption(privateKey);
  auto signature = cryptoService.generateChecksum(privateKey, salt);
  auto splittedKey = cryptoService.splitVector(returnedKey); // return {key, {}} for 16 bytes seed
  
  display.displaySubMessage("PLUG YOUR TAG", 43);
  const unsigned long timeout = 5000; // 5 seconds
  unsigned long startTime = millis();

  while (true) {
    if (millis() - startTime > timeout) {
        // Ask confirmation to continue each 5 sec
        auto continueProcess = confirmationSelection.select("Retry saving seed?");
        if (!continueProcess) {
            display.displaySubMessage("RFID save cancelled", 27, 2000);
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
    auto signSaved = rfidService.saveChecksum(signature);
    if (!signSaved) {
        display.displaySubMessage("Failed to save sign", 34, 1000);
        continue;
    }

    // Save seed length
    auto lengthSaved = rfidService.saveMetadata(privateKey.size());
    if (!lengthSaved) {
        display.displaySubMessage("Failed to save length", 31, 1000);
        continue;
    }

    display.displaySubMessage("Seed is saved", 60, 2000);
    return;
  }
  rfidService.end();
}

std::vector<uint8_t> GlobalManager::manageRfidRead() {
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

    return manageRfidDecryption();
  }
}

std::vector<uint8_t> GlobalManager::manageBitcoinSignature(std::string& psbt, std::string& mnemonic) {
    // Get passphrase or ask for it
    auto passphrase = selectionContext.getCurrentSelectedWallet().getPassphrase();
    if(passphrase.empty()) {
      passphrase = managePassphrase();
    }
    
    // Sign
    auto signedTransactionB64 = cryptoService.signBitcoinTransactions(psbt, mnemonic, passphrase);
    if(signedTransactionB64.empty()) {return {};}

    // Convert
    auto signedTransactionBytes = cryptoService.convertPSBTBase64ToBinary(signedTransactionB64);
    if(signedTransactionBytes.empty()) {return {};}

    return signedTransactionBytes;
}

} // namespace managers
