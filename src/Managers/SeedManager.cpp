#include "SeedManager.h"
#include <algorithm>
#include <stdexcept>

namespace managers {

SeedManager::SeedManager(CryptoService& cryptoService,
                         RfidService& rfidService,
                         SdService& sdService,
                         WalletService& walletService,
                         ConfirmationSelection& confirmationSelection,
                         StringPromptSelection& stringPromptSelection)
    : cryptoService(cryptoService),
      rfidService(rfidService),
      sdService(sdService),
      walletService(walletService),
      confirmationSelection(confirmationSelection),
      stringPromptSelection(stringPromptSelection) {}

std::tuple<std::vector<uint8_t>, std::string> SeedManager::manageEncryption(const std::vector<uint8_t>& privateKey) {
    auto encryptConfirmation = confirmationSelection.select("Encrypt the seed?");
    if (!encryptConfirmation) {
        return {privateKey, ""};
    }

    auto salt = cryptoService.getRandomString(16);
    auto password = stringPromptSelection.select("Enter a password");
    auto encryptedKey = cryptoService.encryptPrivateKeyWithPassphrase(privateKey, password, salt);
    return {encryptedKey, salt};
}

std::vector<uint8_t> SeedManager::manageDecryption() {
    auto privateKey = rfidService.getPrivateKey();
    auto salt = rfidService.getSalt();
    auto sign = rfidService.getSignature();

    bool validation = false;
    while (!validation) {
        auto password = stringPromptSelection.select("Enter the password");
        auto decryptedKey = cryptoService.decryptPrivateKeyWithPassphrase(privateKey, password, salt);
        auto generatedSign = cryptoService.generateSignature(decryptedKey, salt);

        if (generatedSign == sign) {
            validation = true;
            return decryptedKey;
        }
    }
    throw std::runtime_error("Decryption failed.");
}

std::vector<uint8_t> SeedManager::generatePrivateKey() {
    std::vector<uint8_t> privateKey;
    do {
        privateKey = cryptoService.generatePrivateKey();
    } while (cryptoService.calculateShanonEntropy(privateKey) < 4.9);
    return privateKey;
}

bool SeedManager::saveToRfid(const std::vector<uint8_t>& privateKey, const std::string& salt, const std::vector<uint8_t>& signature) {
    rfidService.initialize();

    auto splittedKey = cryptoService.splitVector(privateKey);

    bool privateKeySaved = rfidService.savePrivateKey(splittedKey.first, splittedKey.second);
    bool saltSaved = rfidService.saveSalt(salt);
    bool signatureSaved = rfidService.saveSignature(signature);

    return privateKeySaved && saltSaved && signatureSaved;
}

std::vector<uint8_t> SeedManager::readFromRfid() {
    rfidService.initialize();
    return manageDecryption();
}

} // namespace managers
