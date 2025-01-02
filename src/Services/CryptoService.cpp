#include "CryptoService.h"
#include "mbedtls/sha256.h"
#undef PSTR // conflict
#undef F
#include <cryptopp/ripemd.h>
#include <cstring>
#include <algorithm>
#include "bootloader_random.h"
#include "esp_random.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "cryptopp/rng.h"

namespace services {

CryptoService::CryptoService() {}

std::vector<uint8_t> CryptoService::generateRandomMbetls(size_t size) {
    // Init context
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);

    // Seed the DRBG
    const char *pers = "cardputer_card_wallet_random_generator";
    mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                          reinterpret_cast<const unsigned char*>(pers),
                          strlen(pers));

    // Get random
    std::vector<uint8_t> randomData(size);
    mbedtls_ctr_drbg_random(&ctr_drbg, randomData.data(), size);

    // Release context
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);

    return randomData;
}

std::vector<uint8_t> CryptoService::generateRandomEsp32(size_t size) {
    // Get entropy from esp32 HRNG
    std::vector<uint8_t> randomData(size);
    bootloader_random_enable();
    esp_fill_random(randomData.data(), randomData.size());
    bootloader_random_disable();
    
    return randomData;
}

std::vector<uint8_t> CryptoService::generateRandomBuiltin(size_t size) {
    // Builtin esp_random
    std::vector<uint8_t> randomData(size);
    size_t i = 0;

    while (i < size) {
        // 32 bits integer
        uint32_t randVal = esp_random();

        // Split randVal into 4 parts
        size_t bytesToCopy = std::min(size - i, static_cast<size_t>(4));
        memcpy(randomData.data() + i, &randVal, bytesToCopy);

        i += bytesToCopy;
    }

    return randomData;
}

std::vector<uint8_t> CryptoService::generatePrivateKey() {
    // 32 bits for a 24 words mnemonic
    const size_t keySize = 32;

    // Get entropy from hardware and software
    std::vector<uint8_t> entropyEsp32 = generateRandomEsp32(keySize);
    std::vector<uint8_t> entropyMbedtls = generateRandomMbetls(keySize);
    std::vector<uint8_t> entropyBuiltin = generateRandomBuiltin(keySize);

    // Get entropy from user action
    std::vector<uint8_t> entropyUser = entropyContext.getAccumulatedEntropy();

    // Hash the user entropy
    uint8_t hash[keySize];
    mbedtls_sha256(entropyUser.data(), entropyUser.size(), hash, 0); // 0 = SHA-256 (pas SHA-224)
    std::vector<uint8_t> hashedUserKey(hash, hash + keySize);

    // Control size
    if (entropyEsp32.size() != keySize || entropyMbedtls.size() != keySize || entropyBuiltin.size() != keySize) {
        throw std::runtime_error("Failed to generate sufficient entropy");
    }

    // Mix entropy with XOR
    std::vector<uint8_t> mixedKey(keySize);
    for (size_t i = 0; i < keySize; ++i) {
        mixedKey[i] = entropyEsp32[i] ^ entropyMbedtls[i] ^ 
                      entropyBuiltin[i] ^ hashedUserKey[i];
    }

    // Process SHA256 on the result
    uint8_t hashedKey[keySize];
    mbedtls_sha256(mixedKey.data(), mixedKey.size(), hashedKey, 0);


    // Convert to vector
    std::vector<uint8_t> privateKey(hashedKey, hashedKey + keySize);

    return privateKey; // Shanon score 7.2 on 256 bits sample
}

std::vector<std::string> CryptoService::privateKeyToMnemonic(const std::vector<uint8_t>& privateKey) {
    auto entropy = std::vector<uint8_t>(privateKey.begin(), privateKey.end());
    auto mnemonic = BIP39::create_mnemonic(entropy, BIP39::language::en);
    auto validation = BIP39::valid_mnemonic(mnemonic, BIP39::language::en);
    if (!validation) {return {};}

    return {mnemonic.begin(), mnemonic.end()};;
}

std::string CryptoService::mnemonicVectorToString(std::vector<std::string> mnemonic) {
    std::ostringstream oss;
    for (auto it = mnemonic.begin(); it != mnemonic.end(); ++it) {
        if (it != mnemonic.begin()) {
            oss << " ";
        }
        oss << *it;
    }

    return oss.str();
}

HDPublicKey CryptoService::derivePublicKey(std::string mnemonic, std::string passphrase) {
    // Mnemonic 24 words with passphrase
    HDPrivateKey hd(mnemonic.c_str(), passphrase.c_str());

    // derive native segwit account BIP84
    HDPrivateKey account = hd.derive("m/84'/0'/0'/");
    return account.xpub();
}

std::string CryptoService::generateBitcoinAddress(HDPublicKey xpub) {
    // Set segwit addresses by default
    xpub.type = P2WPKH;
    return xpub.derive("m/0/0").address().c_str();
}

double CryptoService::calculateShanonEntropy(const std::vector<uint8_t>& data) {
    if (data.empty()) {
        throw std::invalid_argument("Data vector is empty.");
    }

    std::map<int, int> frequency; // Frequency map to count occurrences of each byte
    for (uint8_t num : data) {
        frequency[num]++;
    }

    double entropy = 0.0;
    for (const auto& pair : frequency) {
        double p = static_cast<double>(pair.second) / data.size(); // Calculate probability
        entropy -= p * std::log2(p); // Shannon entropy formula
    }

    return entropy;
}

double CryptoService::calculateMaurerRandomness(const std::vector<uint8_t>& data) {
    if (data.empty()) {
        throw std::invalid_argument("Data vector is empty.");
    }

    CryptoPP::MaurerRandomnessTest test;
    test.Put2(data.data(), data.size(), 0, true);

    if (test.BytesNeeded() == 0) {
        return test.GetTestValue();
    }

    throw std::runtime_error("Insufficient data for Maurer randomness test.");
}

double CryptoService::calculateMinEntropy(const std::vector<uint8_t>& data) {
    if (data.empty()) {
        throw std::invalid_argument("Data vector is empty.");
    }

    std::map<int, int> frequency; // Frequency map
    for (uint8_t num : data) {
        frequency[num]++;
    }

    // Find the maximum probability
    size_t maxFrequency = 0;
    for (const auto& pair : frequency) {
        maxFrequency = std::max(maxFrequency, static_cast<size_t>(pair.second));
    }

    double maxProbability = static_cast<double>(maxFrequency) / data.size();
    double minEntropy = -std::log2(maxProbability); // Calculate min-entropy

    return minEntropy;
}

std::string CryptoService::encodeBase58(const uint8_t* input, size_t len) {
    if (!input || len == 0) {
        return "";
    }

    const char* base58Chars = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
    size_t zeroCount = 0;

    // Count leading zero bytes
    while (zeroCount < len && input[zeroCount] == 0) {
        ++zeroCount;
    }

    // Initialize buffer for b58 conversion
    std::vector<uint8_t> b58Buffer(len * 2);
    size_t bufferSize = 0;

    // Encode to b58
    for (size_t i = zeroCount; i < len; ++i) {
        int carry = input[i];
        for (size_t j = 0; j < bufferSize; ++j) {
            carry += b58Buffer[j] * 256;
            b58Buffer[j] = carry % 58;
            carry /= 58;
        }
        while (carry > 0) {
            b58Buffer[bufferSize++] = carry % 58;
            carry /= 58;
        }
    }

    // String result
    std::string result(zeroCount, '1');
    for (auto it = b58Buffer.rbegin(); it != b58Buffer.rend(); ++it) {
        if (*it != 0 || result.size() > zeroCount) { // Skip leading zeroes in the buffer
            result += base58Chars[*it];
        }
    }

    return result;
}

std::vector<uint8_t> CryptoService::OLDderivePublicKey(const std::vector<uint8_t>& privateKey) {
    // Create context
    secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_NONE);

    // Verify Private key
    if (!secp256k1_ec_seckey_verify(ctx, privateKey.data())) {
        secp256k1_context_destroy(ctx);
        throw std::runtime_error("Invalid private key");
    }

    // Get PubKey
    secp256k1_pubkey pubkey;
    if (!secp256k1_ec_pubkey_create(ctx, &pubkey, privateKey.data())) {
        secp256k1_context_destroy(ctx);
        throw std::runtime_error("Failed to create public key");
    }
    
    // Serialize uncompressed PubKey
    unsigned char serializedPubkey[65];
    size_t serializedPubkeyLen = sizeof(serializedPubkey);
    secp256k1_ec_pubkey_serialize(ctx, serializedPubkey, &serializedPubkeyLen, &pubkey, SECP256K1_EC_UNCOMPRESSED);
    
    // Delete context
    secp256k1_context_destroy(ctx);

    return std::vector<uint8_t>(serializedPubkey, serializedPubkey + serializedPubkeyLen);
}

void CryptoService::OLDhashPublicKey(const std::vector<uint8_t>& publicKey, std::vector<uint8_t>& hashedKey) {
    uint8_t sha256Hash[32];
    mbedtls_sha256(publicKey.data(), publicKey.size(), sha256Hash, 0);

    CryptoPP::RIPEMD160 ripemd;
    hashedKey.resize(20);
    ripemd.CalculateDigest(hashedKey.data(), sha256Hash, sizeof(sha256Hash));
}

std::string CryptoService::OLDgenerateBitcoinAddress(const std::vector<uint8_t>& publicKey) {
    // Legacy address starting with 1.....
    std::vector<uint8_t> hashedKey(20);
    OLDhashPublicKey(publicKey, hashedKey);

    uint8_t extendedKey[25];
    uint8_t checksum[32];
    extendedKey[0] = 0x00;
    std::memcpy(extendedKey + 1, hashedKey.data(), 20);

    mbedtls_sha256(extendedKey, 21, checksum, 0);
    mbedtls_sha256(checksum, 32, checksum, 0);

    std::memcpy(extendedKey + 21, checksum, 4);
    return encodeBase58(extendedKey, 25);
}

}