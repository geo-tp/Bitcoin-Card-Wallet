#ifndef WALLET_H
#define WALLET_H

#include <string>
#include <vector>

namespace models {

class Wallet {
private:
    std::string name;                // Nom du wallet
    std::string zPub;                // Clé publique étendue SegWit (zpub)
    std::string xPub;                // Clé publique étendue Legacy (xpub)
    std::string address;             // Adresse Bitcoin
    std::vector<uint8_t> privateKey; // Only stored if restored or loaded
    std::string mnemonic;            // Only store if restore from SD
    std::string passphrase;          // Only store for signing transaction
    std::string fingerprint;         // Master public fingerprint
    std::string derivePath;          // Derivation

public:
    Wallet() : name(""), address(""), mnemonic(""), passphrase(""), 
               fingerprint(""), derivePath(""), zPub(""), xPub("") {}

    Wallet(const std::string& walletName, 
           const std::string& zpubKey, 
           const std::string& addr)
        : name(walletName), zPub(zpubKey), address(addr) {}

    Wallet(const std::string& walletName, 
           const std::string& zpubKey, 
           const std::string& addr,
           const std::string& mnemonic)
        : name(walletName), zPub(zpubKey), address(addr), mnemonic(mnemonic) {}

    Wallet(const std::string& walletName, 
           const std::string& zpubKey, 
           const std::string& addr,
           const std::string& fingerprint,
           const std::string& derivePath)
        : name(walletName), zPub(zpubKey), address(addr),
          fingerprint(fingerprint), derivePath(derivePath) {}

    Wallet(const std::string& walletName,
           const std::string& zpubKey,
           const std::string& addr,
           const std::string& mnemonic,
           const std::string& fingerprint,
           const std::string& derivePath)
        : name(walletName), zPub(zpubKey), address(addr), mnemonic(mnemonic),
          fingerprint(fingerprint), derivePath(derivePath) {}

    // Getters
    std::string getName() const {
        return name;
    }

    std::string getZPub() const {
        return zPub;
    }

    std::string getXPub() const {
        return xPub;
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

    std::string getFingerprint() const {
        return fingerprint;
    }

    std::string getDerivePath() const {
        return derivePath;
    }

    // Setters
    void setName(const std::string& walletName) {
        name = walletName;
    }

    void setZPub(const std::string& zpubKey) {
        zPub = zpubKey;
    }

    void setXPub(const std::string& xpubKey) {
        xPub = xpubKey;
    }

    void setAddress(const std::string& addr) {
        address = addr;
    }

    void setPrivateKey(const std::vector<uint8_t>& priKey) {
        privateKey = priKey;
    }

    void setMnemonic(const std::string& mne) {
        mnemonic = mne;
    }

    void setPassphrase(const std::string& pp) {
        passphrase = pp;
    }

    void setFingerprint(const std::string& fp) {
        fingerprint = fp;
    }

    void setDerivePath(const std::string& path) {
        derivePath = path;
    }

    // Utils
    bool empty() const {
        return name.empty() && zPub.empty() && address.empty();
    }
};

} // namespace models

#endif // WALLET_H
