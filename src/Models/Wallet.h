#ifndef WALLET_H
#define WALLET_H

#include <string>
#include <vector>

namespace models {

class Wallet {
private:
    std::string name;                // Nom du wallet
    std::string publicKey;           // Clé publique du wallet xpub sous forme zpub....
    std::string address;             // Adresse Bitcoin
    std::vector<uint8_t> privateKey; // Only stored if restored or loaded
    std::string mnemonic;            // Only store if restore from SD
    std::string passphrase;          // Only store for signing transaction

public:
    Wallet() : name(""), address(""), mnemonic(""), passphrase("") {}

    Wallet(const std::string& walletName, 
           const std::string& pubKey, 
           const std::string& addr)
        : name(walletName), publicKey(pubKey), address(addr) {}

    Wallet(const std::string& walletName, 
           const std::string& pubKey, 
           const std::string& addr,
           const std::string& mnemonic)
        : name(walletName), publicKey(pubKey), address(addr), mnemonic(mnemonic) {}

    // Getters
    std::string getName() const {
        return name;
    }

    std::string getPublicKey() const {
        return publicKey;
    }

    std::string getAddress() const {
        return address;
    }

    std::vector<uint8_t> getPrivateKey() const {
        return privateKey;
    }

    std::string getMnemonic() const {
        return mnemonic;
    }

    std::string getPassphrase() const {
        return passphrase;
    }

    // Setters
    void setName(const std::string& walletName) {
        name = walletName;
    }

    void setPublicKey(const std::string& pubKey) {
        publicKey = pubKey;
    }

    void setPrivateKey(const std::vector<uint8_t>& priKey) {
        privateKey = priKey;
    }

    void setMnemonic(const std::string& mne) {
        mnemonic = mne;
    }

    void setAddress(const std::string& addr) {
        address = addr;
    }

    void setPassphrase(const std::string& pp) {
        passphrase = pp;
    }
    
    // Utils
    bool empty() const {
        return name.empty() && publicKey.empty() && address.empty();
    }
};

} // namespace models

#endif // WALLET_H
