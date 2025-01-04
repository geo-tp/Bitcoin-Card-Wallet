#ifndef CRYPTO_SERVICE_H
#define CRYPTO_SERVICE_H

#include <string>
#include <vector>
#include <cstdint>
#include <sstream>
#include <iomanip>
#include "secp256k1.h" // bitcoin core official lib
#include "bip39/bip39.h"
#include "Bitcoin.h"
#include "PSBT.h"       // if using PSBT functionality
#include "Conversion.h" // to get access to functions like toHex() or fromBase64()
#include "Hash.h"       // if using hashes
#include <Contexts/EntropyContext.h>

using namespace contexts;

namespace services {

class CryptoService {
public:
    CryptoService();
    
    std::vector<uint8_t> generateRandomMbetls(size_t size);
    std::vector<uint8_t> generateRandomEsp32(size_t size);
    std::vector<uint8_t> generateRandomBuiltin(size_t size);
    std::string getRandomString(size_t length);
    std::vector<uint8_t> generatePrivateKey();
    std::vector<std::string> privateKeyToMnemonic(const std::vector<uint8_t>& privateKey);
    HDPublicKey derivePublicKey(std::string mnemonic, std::string passphrase="");
    std::string generateBitcoinAddress(HDPublicKey xpub);
    std::string mnemonicVectorToString(std::vector<std::string> mnemonic);
    double calculateShanonEntropy(const std::vector<uint8_t>& data);
    double calculateMinEntropy(const std::vector<uint8_t>& data);
    double calculateMaurerRandomness(const std::vector<uint8_t>& data);
    std::vector<uint8_t> deriveKeyFromPassphrase(const std::string& passphrase, const std::string& salt, size_t keySize);
    std::vector<uint8_t> encryptAES(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key);
    std::vector<uint8_t> decryptAES(const std::vector<uint8_t>& encrypted, const std::vector<uint8_t>& key);
    std::vector<uint8_t> encryptPrivateKeyWithPassphrase(const std::vector<uint8_t>& privateKey, const std::string& passphrase, const std::string& salt);
    std::vector<uint8_t> decryptPrivateKeyWithPassphrase(const std::vector<uint8_t>& encryptedPrivateKey, const std::string& passphrase, const std::string& salt);
    std::pair<std::vector<uint8_t>, std::vector<uint8_t>> splitVector(const std::vector<uint8_t>& input);
    std::vector<uint8_t> generateSignature(const std::vector<uint8_t>& data, const std::string& salt);
    std::vector<uint8_t> OLDderivePublicKey(const std::vector<uint8_t>& privateKey);
    std::string OLDgenerateBitcoinAddress(const std::vector<uint8_t>& publicKey);
private:
    void OLDhashPublicKey(const std::vector<uint8_t>& publicKey, std::vector<uint8_t>& hashedKey);
    std::string encodeBase58(const uint8_t* input, size_t len);
    EntropyContext& entropyContext = EntropyContext::getInstance();
};

}

#endif // CRYPTO_SERVICE_H
