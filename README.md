# Bitcoin Card Wallet

Bitcoin Card Wallet is an ESP32 project that allows you to generate seeds, addresses, and  manage Bitcoin wallets. It offers key generation based on BIP39 standards, allowing the user to generate a mnemonic 24 words seed that can be safely backed up and restored in compatible wallets.

![](./images/bitcoin-card-wallet.jpg)


- **Manage Wallets**: create seeds, public key, mnemonic words, balance
- **QR Code**: Generate and display QR codes for easy sharing of Bitcoin and balance addresses.
- **USB Typing**: Emulate a USB keyboard to type out Bitcoin addresses.
- **SD Card**: Support for SD card storage to save and retrieve wallet data.
- **Passphrase**: Generate and use seeds with an optional passphrase for enhanced security.

## How to use

1. Navigate to the **"Create a wallet"** option.
2. Record the displayed mnemonic safely (on paper, **not digitally**), you can choose a passphrase.
3. Use the wallet's menu to derive and display your **Bitcoin address, balance and xpub key.**

**NOTE :** An SD card is require to save wallets publics informations and load them at the launch of the application. Only public informations about your wallets are stored on the SD card, **SAVE YOUR SEED** or you will lose access to your bitcoin wallet.

The following informations will be stored on SD card in the file `bitcoin-card-wallets.txt` : 

- ***Wallet name***
- ***Public Key***
- ***Bitcoin address***

You can **manually edit this file** to add others bitcoin addresses or remove existing ones

```
Filetype: Card Wallet
Version: 1

# WALLET 1
Name: Geo Wallet
PublicKey: zpub6rCZfQGdfMMFhj2kMV66dZXhVxWfgfFfJF7MzkQ4zdXdT44dC4x756Qd8b14j5aAbgL1r7JgjABEDRoWSLkA89PEek5wxgFqtjXsCPgp6BQ
BitcoinAddress: bc1qcr6zdqzqsqu9dh9fr8899p59m4cq4xjl3aepmr

# WALLET 2
...

```

## BIP39 Mnemonic Support:
  - The private key is converted into a **BIP39-compliant** mnemonic phrase.
  - The wallet generates seeds in compliance with the BIP39 standard.
  - Supports **24-word mnemonics**.
  - Mnemonics can be restored in other **BIP39-compatible wallets** (Electrum for example).

#### Example of a 24 words BIP39 Mnemonic seed generated on the cardputer
`dragon reform deer execute fee tattoo wall barely loan jealous require student pipe bamboo solve toilet latin bargain escape spray scan stay father utility`

## Native SegWit Support:
  - Derives **Native SegWit** Bitcoin addresses by default.
  - The wallet uses the **BIP84 standard to derive keys**, following the path m/84'/0'/0' for Bitcoin mainnet.
  - The **xpub key (extended public key)** is generated in a standardized format and allows the derivation of an unlimited number of public addresses

#### Example of my segwit bitcoin address generated on the cardputer
`bc1qcr6zdqzqsqu9dh9fr8899p59m4cq4xjl3aepmr`

## Seed Import in Electrum

Bitcoin Card Wallet generates a BIP39-compliant seed phrase that you can use to restore your wallet:

[Electrum Wallet Official Website](https://electrum.org/)

1. Open Electrum and select **"Standard wallet"**
- ![](./images/electrum1.png)
2. Select **"I already have a seed"** and click next.
- ![](./images/electrum2.png)
3. Click on **"Options"** and select BIP39 seed, **write the 24 words**
- ![](./images/electrum3.png)
- ![](./images/electrum4.png)
5. If you used a passphrase, **ensure to check the extended word checkbox** to access the correct wallet.
- ![](./images/electrum6.png)
6. If you have a passphrase, the **passphrase must be entered** after typing the 24 words
- ![](./images/electrum7.png)
7. Electrum will automatically derive the keys and addresses following the **BIP84 (m/84'/0'/0') standard**, which is used by this wallet for **native SegWit** (Bech32) addresses.
- ![](./images/electrum5.png)


## Passphrase

The passphrase is an optional feature that adds an extra layer of security to your wallet. When set, it is combined with your BIP39 seed to generate a unique set of keys. Without the correct passphrase, the wallet cannot access the same addresses or funds, so it is crucial to remember or securely store it.

## Random Number Generator

In the Bitcoin Card Wallet project, random number generation is a critical component for creating secure private keys and ensuring the overall security of the wallet. 

The project uses multiple sources of entropy to generate random numbers, which are then combined to form a single private key.

### Three sources of enthropy

```cpp

// mbedTLS's Deterministic Random Bit Generator seeded with hardware generated entropy and a custom string
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

// Hardware Random Number Generator (HRNG) to produce random numbers from hardware
std::vector<uint8_t> CryptoService::generateRandomEsp32(size_t size) {
    // Get entropy from esp32 HRNG
    std::vector<uint8_t> randomData(size);
    bootloader_random_enable();
    esp_fill_random(randomData.data(), randomData.size());
    bootloader_random_disable();
    
    return randomData;
}

// Software based random numbers
std::vector<uint8_t> CryptoService::generateRandomBuiltin(size_t size) {
    // Builtin esp_random
    std::vector<uint8_t> randomData(size);
    for (size_t i = 0; i < size; ++i) {
        // 8bits
        randomData[i] = static_cast<uint8_t>(esp_random() & 0xFF);
    }
    return randomData;
}
```

### Mixed and hashed to form the final private key
```cpp
std::vector<uint8_t> CryptoService::generatePrivateKey() {
    const size_t keySize = 32;

    // Get random from different sources
    std::vector<uint8_t> entropyEsp32 = generateRandomEsp32(keySize);
    std::vector<uint8_t> entropyMbedtls = generateRandomMbetls(keySize);
    std::vector<uint8_t> entropyBuiltin = generateRandomBuiltin(keySize);

    if (entropyEsp32.size() != keySize || entropyMbedtls.size() != keySize || entropyBuiltin.size() != keySize) {
        throw std::runtime_error("Failed to generate sufficient entropy");
    }

    // Mix them with XOR
    std::vector<uint8_t> mixedKey(keySize);
    for (size_t i = 0; i < keySize; ++i) {
        mixedKey[i] = entropyEsp32[i] ^ entropyMbedtls[i] ^ entropyBuiltin[i];
    }

    // Post process SHA256
    uint8_t hashedKey[keySize];
    mbedtls_sha256(mixedKey.data(), mixedKey.size(), hashedKey, 0);

    // Convert to vector
    std::vector<uint8_t> privateKey(hashedKey, hashedKey + keySize);

    // Check entropy and retry if insufficient
    double entropy = calculateShanonEntropy(privateKey);
    if (entropy < 4.9) {
        return generatePrivateKey();
    }

    return privateKey;
}
```

## License and Disclaimer
This project is provided under the MIT License, allowing free use, modification, and distribution under its terms. By using this software, you agree to the following:

- **No Warranty**: This software is provided "as is," without any warranties of any kind, whether express or implied. This includes, but is not limited to, fitness for a particular purpose, security, or absence of defects.
- **No Liability**: The authors and contributors of this software are not responsible for any loss, damage, or claims arising from the use of this software. This includes but is not limited to financial loss, theft of funds, or failure of the application to perform as intended.
- **User Responsibility**: It is your responsibility to safely back up your seed phrase, private keys, and any other sensitive information generated by this software. The seed phrase is not saved on the device, and loss of this information will result in the inability to access funds.
- **Personal Use**: This software is intended for educational and personal use only.

By using this software, you acknowledge and accept these terms.