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
#include "wally_core.h"
#include "wally_bip32.h"
#include "wally_bip39.h"
#include "wally_address.h"
#include "wally_script.h"
#include "wally_psbt.h"
#include <fstream>
#include <M5Cardputer.h>

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
    std::vector<uint8_t> generatePrivateKey(size_t keySize = 32);
    std::vector<std::string> privateKeyToMnemonic(const std::vector<uint8_t>& privateKey);
    HDPublicKey deriveZPub(std::string mnemonic, std::string passphrase="");
    HDPublicKey deriveXPub(std::string mnemonic, std::string passphrase);
    std::string getFingerprint(std::string mnemonic, std::string passphrase);
    std::string getLegacyDerivePath();
    std::string getSegwitDerivePath();
    std::string generateBitcoinLegacyAddress(HDPublicKey xpub);
    std::string generateBitcoinSegwitAddress(HDPublicKey xpub);
    std::string mnemonicVectorToString(std::vector<std::string> mnemonic);
    BIP39::word_list mnemonicStringToWordList(const std::string& mnemonicStr);
    std::vector<uint8_t> mnemonicToPrivateKey(const std::string& mnemonic);
    bool verifyMnemonic(BIP39::word_list mnemonic);
    double calculateShanonEntropy(const std::vector<uint8_t>& data);
    double calculateMinEntropy(const std::vector<uint8_t>& data);
    double calculateMaurerRandomness(const std::vector<uint8_t>& data);
    std::vector<uint8_t> mixEntropy(const std::vector<uint8_t>& data1, const std::vector<uint8_t>& data2, const std::vector<uint8_t>& data3, const std::vector<uint8_t>& data4);
    std::vector<uint8_t> hashSha256(const std::vector<uint8_t>& entropy, size_t keySize);
    std::vector<uint8_t> deriveKeyFromPassphrase(const std::string& passphrase, const std::string& salt, size_t keySize);
    std::vector<uint8_t> encryptAES(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key);
    std::vector<uint8_t> decryptAES(const std::vector<uint8_t>& encrypted, const std::vector<uint8_t>& key);
    std::vector<uint8_t> encryptPrivateKeyWithPassphrase(const std::vector<uint8_t>& privateKey, const std::string& passphrase, const std::string& salt);
    std::vector<uint8_t> decryptPrivateKeyWithPassphrase(const std::vector<uint8_t>& encryptedPrivateKey, const std::string& passphrase, const std::string& salt);
    std::pair<std::vector<uint8_t>, std::vector<uint8_t>> splitVector(const std::vector<uint8_t>& input);
    std::vector<uint8_t> generateChecksum(const std::vector<uint8_t>& data, const std::string& salt);
    std::string signBitcoinTransactions(const std::string& psbtBase64, const std::string& mnemonic, const std::string& passphrase);
    std::vector<uint8_t> convertPSBTBase64ToBinary(const std::string& psbtBase64);
    std::string convertPSBTBinaryToBase64(const std::vector<uint8_t>& psbtBinary);
    std::vector<uint8_t> OLDderivePublicKey(const std::vector<uint8_t>& privateKey);
    std::string OLDgenerateBitcoinSegwitAddress(const std::vector<uint8_t>& publicKey);
private:
    void OLDhashPublicKey(const std::vector<uint8_t>& publicKey, std::vector<uint8_t>& hashedKey);
    std::string encodeBase58(const uint8_t* input, size_t len);
    EntropyContext& entropyContext = EntropyContext::getInstance();
};

}

#endif // CRYPTO_SERVICE_H
