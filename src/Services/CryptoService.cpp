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
#include "mbedtls/pkcs5.h"
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

std::string CryptoService::getRandomString(size_t length) {
    auto randomData = generateRandomEsp32(length);
    std::string randomString(randomData.begin(), randomData.end());

    return randomString;
}

std::vector<uint8_t> CryptoService::generatePrivateKey(size_t keySize) {
    // Get entropy from hardware and software
    auto entropyEsp32 = generateRandomEsp32(keySize);
    auto entropyMbedtls = generateRandomMbetls(keySize);
    auto entropyBuiltin = generateRandomBuiltin(keySize);

    // Get entropy from user action
    auto entropyUser = entropyContext.getAccumulatedEntropy();

    // Control size
    if (entropyEsp32.size() != keySize || entropyMbedtls.size() != keySize || entropyBuiltin.size() != keySize) {
        throw std::runtime_error("Failed to generate sufficient entropy");
    }

    // Process SHA256 on the user entropy
    auto hashedEntropyUser = hashSha256(entropyUser, keySize);

    // Mix entropy with XOR
    auto mixedKey = mixEntropy(entropyMbedtls, entropyEsp32, 
                                               entropyBuiltin, hashedEntropyUser);
    // Process SHA256 on the result
    auto privateKey = hashSha256(mixedKey, keySize);

    return privateKey;
}

std::vector<std::string> CryptoService::privateKeyToMnemonic(const std::vector<uint8_t>& privateKey) {
    // Convert a 32 bytes private key into a vector of 24 words
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

BIP39::word_list CryptoService::mnemonicStringToWordList(const std::string& mnemonicStr) {
    BIP39::word_list wordList;

    if (mnemonicStr.empty()) {
        return wordList;
    }

    // Split the string by whitespace into tokens, then add each token to the word_list
    std::istringstream iss(mnemonicStr);
    for (std::string token; iss >> token; ) {
        wordList.add(token);
    }

    return wordList;
}

bool CryptoService::verifyMnemonic(BIP39::word_list mnemonic) {
    return BIP39::valid_mnemonic(mnemonic, BIP39::language::en);
}

std::vector<uint8_t> CryptoService::hashSha256(const std::vector<uint8_t>& entropy, size_t keySize) {
    uint8_t hash[keySize];
    mbedtls_sha256(entropy.data(), entropy.size(), hash, 0); // 0 = SHA-256 (not SHA-224)

    // Convert to std::vector<uint8_t> and return
    return std::vector<uint8_t>(hash, hash + keySize);
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

std::vector<uint8_t> CryptoService::mixEntropy(const std::vector<uint8_t>& data1, 
                                               const std::vector<uint8_t>& data2, 
                                               const std::vector<uint8_t>& data3,
                                               const std::vector<uint8_t>& data4) {
    // Mix entropy with XOR
    std::vector<uint8_t> mixedKey(data1.size());
    for (size_t i = 0; i < data1.size(); ++i) {
        mixedKey[i] = data1[i] ^ data2[i] ^ 
                      data3[i] ^ data4[i];
    }
    return mixedKey;
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

std::vector<uint8_t> CryptoService::deriveKeyFromPassphrase(const std::string& passphrase, const std::string& salt, size_t keySize) {
    std::vector<uint8_t> key(keySize);

    // initialize context
    mbedtls_md_context_t mdContext;
    mbedtls_md_init(&mdContext);
    const mbedtls_md_info_t* mdInfo = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);


    // Configure context
    if (mbedtls_md_setup(&mdContext, mdInfo, 1) != 0) {
        mbedtls_md_free(&mdContext);
        throw std::runtime_error("Failed to setup MD context");
    }

    // Derive key PBKDF2
    int ret = mbedtls_pkcs5_pbkdf2_hmac(
        &mdContext,                               // Contexte de hachage
        reinterpret_cast<const unsigned char*>(passphrase.data()), passphrase.size(), // Passphrase
        reinterpret_cast<const unsigned char*>(salt.data()), salt.size(),             // Salt
        10000,                                    // Nombre iterations
        keySize,                                  // Taille de la clé
        key.data()                                // Résultat
    );

    mbedtls_md_free(&mdContext);

    if (ret != 0) {
        throw std::runtime_error("Failed to derive key using PBKDF2");
    }

    return key;
}

std::vector<uint8_t> CryptoService::encryptAES(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
    if (data.size() % 16 != 0) {
        throw std::invalid_argument("Data size must be a multiple of 16.");
    }

    if (key.size() != 16) {
        throw std::invalid_argument("Key size must be 16 bytes for AES-128.");
    }

    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);

    // Set the encryption key
    mbedtls_aes_setkey_enc(&aes, key.data(), 128);

    std::vector<uint8_t> encrypted(data.size());
    for (size_t i = 0; i < data.size(); i += 16) {
        mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, data.data() + i, encrypted.data() + i);
    }

    mbedtls_aes_free(&aes);

    return encrypted;
}

std::vector<uint8_t> CryptoService::decryptAES(const std::vector<uint8_t>& encrypted, const std::vector<uint8_t>& key) {
    if (encrypted.size() % 16 != 0) {
        throw std::invalid_argument("Encrypted data size must be a multiple of 16.");
    }

    if (key.size() != 16) {
        throw std::invalid_argument("Key size must be 16 bytes for AES-128.");
    }

    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);

    // Set the decryption key
    mbedtls_aes_setkey_dec(&aes, key.data(), 128);

    std::vector<uint8_t> decrypted(encrypted.size());
    for (size_t i = 0; i < encrypted.size(); i += 16) {
        mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, encrypted.data() + i, decrypted.data() + i);
    }

    mbedtls_aes_free(&aes);

    return decrypted;
}

std::vector<uint8_t> CryptoService::encryptPrivateKeyWithPassphrase(const std::vector<uint8_t>& privateKey, const std::string& passphrase, const std::string& salt) {
    if (privateKey.size() != 32) {
        throw std::invalid_argument("Private key size must be 32 bytes.");
    }

    // Derive key with passphrase and salt
    auto derivedKey = deriveKeyFromPassphrase(passphrase, salt, 16);

    // Encrypt
    auto encryptedPrivateKey = encryptAES(privateKey, derivedKey);

    return encryptedPrivateKey;
}

std::vector<uint8_t> CryptoService::decryptPrivateKeyWithPassphrase(const std::vector<uint8_t>& encryptedPrivateKey, const std::string& passphrase, const std::string& salt) {
    if (encryptedPrivateKey.size() % 16 != 0) {
        throw std::invalid_argument("Encrypted private key size must be a multiple of 16.");
    }

    // Derive key with passphrase and salt
    auto derivedKey = deriveKeyFromPassphrase(passphrase, salt, 16);

    // Decrypt
    auto decryptedPrivateKey = decryptAES(encryptedPrivateKey, derivedKey);

    return decryptedPrivateKey;
}

std::pair<std::vector<uint8_t>, std::vector<uint8_t>> CryptoService::splitVector(const std::vector<uint8_t>& input) {
    if (input.size() != 32) {
        throw std::invalid_argument("Input vector must be 32 bytes.");
    }

    std::vector<uint8_t> part1(input.begin(), input.begin() + 16);
    std::vector<uint8_t> part2;
    part2.insert(part2.end(), input.begin() + 16, input.end());

    return {part1, part2};
}

std::vector<uint8_t> CryptoService::generateSignature(const std::vector<uint8_t>& data, const std::string& salt) {
    // Combine data and salt
    std::vector<uint8_t> combined(data.begin(), data.end());
    combined.insert(combined.end(), salt.begin(), salt.end());

    // Hash the combined data
    uint8_t hash[32]; // SHA256 produces 32 bytes
    mbedtls_sha256(combined.data(), combined.size(), hash, 0); // 0 for SHA256, not SHA224

    // Return the first 16 bytes
    return std::vector<uint8_t>(hash, hash + 16);
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