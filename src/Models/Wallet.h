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

public:
    Wallet() : name(""), address("") {}

    Wallet(const std::string& walletName, 
           const std::string& pubKey, 
           const std::string& addr)
        : name(walletName), publicKey(pubKey), address(addr) {}

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

    // Setters
    void setName(const std::string& walletName) {
        name = walletName;
    }

    void setPublicKey(const std::string& pubKey) {
        publicKey = pubKey;
    }

    void setAddress(const std::string& addr) {
        address = addr;
    }
    
    // Utils
    bool empty() const {
        return name.empty() && publicKey.empty() && address.empty();
    }
};

} // namespace models

#endif // WALLET_H
