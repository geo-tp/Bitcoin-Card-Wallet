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
    do {
        input1 = stringPromptSelection.select(prompt1, 0, false, true);
        input2 = stringPromptSelection.select(prompt2, 4, false, true);
        if (input1 != input2) {
            display.displaySubMessage(mismatchMessage, 58, 2000);
        }
    } while (input1 != input2);
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
    if (privateKey.empty()) {
      display.displaySubMessage("Failed to read key", 38, 1000);
      return {};
    }
    auto salt = rfidService.getSalt();
    auto sign = rfidService.getCheckSum(); 
    ledService.blink(); // to signal RFID reading

    // If salt is empty, the seed is not emcrypted
    auto saltIsEmpty = std::all_of(salt.begin(), salt.end(), [](int value) { return value == 0; });

    bool validation = false;
    while (!validation && privateKey.size() % 16 == 0 && !saltIsEmpty) {
      // Ask password
      auto password = stringPromptSelection.select("Enter the password", 8, true, true);
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
  auto initialised = rfidService.initialize();
  if(!initialised) {
     display.displaySubMessage("No RFID module", 48, 2500);
     return;
  }

  // Get salt, key, sign
  std::vector<uint8_t> returnedKey;
  std::string salt;
  std::tie(returnedKey, salt) = manageRfidEncryption(privateKey);
  auto signature = cryptoService.generateChecksum(privateKey, salt);
  auto splittedKey = cryptoService.splitVector(returnedKey); // return {key, {}} for 16 bytes seed
  
  display.displaySubMessage("PLUG YOUR TAG", 43);
  const unsigned long timeout = 5000; // 5 seconds
  unsigned long startTime = millis();
  bool eraseConfirmation = false;

  while (true) {
    if (millis() - startTime > timeout) {
        // Ask confirmation to continue each 5 sec
        auto continueProcess = confirmationSelection.select("Retry saving seed?");
        if (!continueProcess) {
            display.displaySubMessage("RFID save cancelled", 27, 2000);
            break;
        }
        rfidService.reset();
        startTime = millis();
        display.displaySubMessage("PLUG YOUR TAG", 43);
    }

    // No tag detected
    if (!rfidService.isCardPresent()) {
        delay(300);
        continue;
    }

    // Tag already contains a seed
    if (!eraseConfirmation) {
      auto metadataByte = rfidService.getMetadata();
      if (metadataByte == 32 || metadataByte == 16) {
        display.displaySubMessage("Tag contains a seed", 28, 1500);
        eraseConfirmation = confirmationSelection.select(" Overwrite tag ?");
        display.displaySubMessage("PLUG YOUR TAG", 43);
        rfidService.reset();
        startTime = millis();
        continue;
      }
    }

    // Save private key
    auto privateKeySaved = rfidService.savePrivateKey(splittedKey.first, splittedKey.second);
    if (!privateKeySaved) {
        display.displaySubMessage("Failed to save key", 36, 1000);
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
        display.displaySubMessage("Failed to save length", 28, 1000);
        continue;
    }

    ledService.blink();
    display.displaySubMessage("Seed is saved", 54, 2500);
    return;
  }
  rfidService.end();
}

std::vector<uint8_t> GlobalManager::manageRfidRead() {
  auto initialised = rfidService.initialize();
  if(!initialised) {
     display.displayTopBar("RFID Error");
     display.displaySubMessage("No RFID module", 48, 2500);
     return {};
  }

  display.displayTopBar("MIFARE 1K");
  display.displaySubMessage("PLUG YOUR TAG", 43);

  std::vector<uint8_t> privateKey;
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
        rfidService.reset();
        startTime = millis();
        display.displaySubMessage("PLUG YOUR TAG", 43);
    }

    // No tag detected
    if (!rfidService.isCardPresent()) {
        delay(300);
        continue;
    }

    // Return key if not empty
    privateKey = manageRfidDecryption();
    if (!privateKey.empty()) {
      return privateKey;
    }
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

Wallet GlobalManager::manageBitcoinWalletCreation(std::string mnemonic, std::string passphrase, 
                                                  std::string walletName, bool loadedConfirmation) {
    // Derive public keys and create segwit BTC address
    display.displaySubMessage("Loading", 83);
    auto zpub = cryptoService.deriveZPub(mnemonic, passphrase);
    auto fingerprint = cryptoService.getFingerprint(mnemonic, passphrase);
    auto derivePath = cryptoService.getSegwitDerivePath();
    auto addressSegwit = cryptoService.generateBitcoinSegwitAddress(zpub);

    if (loadedConfirmation) {
      display.displaySubMessage("Seed loaded", 65, 2000);
    }

    // Create Wallet
    return Wallet(walletName, zpub.toString().c_str(), addressSegwit, 
                  fingerprint, derivePath);
}

} // namespace managers
