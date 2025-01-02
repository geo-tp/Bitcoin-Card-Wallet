#include "WalletRepository.h"

namespace repositories {

bool WalletRepository::addWallet(const Wallet& wallet) {
    wallets.push_back(wallet);
    return true;
}

bool WalletRepository::deleteWallet(const std::string& walletName) {
    auto it = std::remove_if(wallets.begin(), wallets.end(), [&walletName](const Wallet& w) {
        return w.getName() == walletName;
    });

    if (it != wallets.end()) {
        wallets.erase(it, wallets.end()); // Supprime le wallet
        return true;
    }

    return false; // Wallet non trouvé
}

const std::vector<Wallet>& WalletRepository::getWallets() const {
    return wallets;
}

std::vector<std::string> WalletRepository::splitWallets(const std::string& fileContent) {
    std::vector<std::string> walletDataList;
    std::istringstream stream(fileContent);
    std::string line;
    std::string currentWalletData;

    while (std::getline(stream, line)) {
        // Détecter le début d'un wallet
        if (line.find("# WALLET") != std::string::npos) {
            currentWalletData.clear(); // Reset for new wallet

            // The 3 following lines are : Name, PublicKey, BitcoinAddress
            for (int i = 0; i < 3; ++i) {
                if (std::getline(stream, line) && !line.empty()) {
                    currentWalletData += line + "\n";
                }
            }

            if (!currentWalletData.empty()) {
                walletDataList.push_back(currentWalletData);
            }
        }
    }
    return walletDataList;
}

Wallet WalletRepository::parseWallet(const std::string& walletData) {
    std::string name;
    std::string publicKey;
    std::string address;

    std::istringstream stream(walletData);
    std::string line;

    while (std::getline(stream, line)) {
        auto delimiterPos = line.find(": ");
        if (delimiterPos == std::string::npos) continue;

        std::string key = line.substr(0, delimiterPos);
        std::string value = line.substr(delimiterPos + 2);

        // Clean bad chars
        key.erase(0, key.find_first_not_of(" \t\r\n"));
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);

        if (key == "Name") {
            name = value;
        } else if (key == "PublicKey") {
            publicKey = value;
        } else if (key == "BitcoinAddress") {
            address = value;
        }
    }

    return Wallet(name, publicKey, address);
}

void WalletRepository::loadAllWallets(const std::string& fileContent) {
    wallets.clear(); // delete existing wallets
    auto walletSegments = splitWallets(fileContent);

    for (const auto& walletData : walletSegments) {
        wallets.push_back(parseWallet(walletData));
    }
}

std::string WalletRepository::getWalletsFileContent() {
    std::ostringstream fileContent;

    // Header
    fileContent << "Filetype: Card Wallet\n";
    fileContent << "Version: 1\n";
    fileContent << "\n";

    // Wallets
    for (size_t i = 0; i < wallets.size(); ++i) {
        const auto& wallet = wallets[i];

        std::string publicKey = wallet.getPublicKey();
        
        fileContent << "# WALLET " << (i + 1) << "\n";
        fileContent << "Name: " << wallet.getName().c_str() << "\n";
        fileContent << "PublicKey: " << publicKey.c_str() << "\n";
        fileContent << "BitcoinAddress: " << wallet.getAddress().c_str() << "\n";
        fileContent << "\n";
    }

    return fileContent.str();
}

std::string WalletRepository::vectorToHexString(const std::vector<uint8_t>& vec) {
    std::ostringstream oss;
    for (uint8_t byte : vec) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return oss.str();
}

std::vector<uint8_t> WalletRepository::hexStringToVector(const std::string& hex) {
    std::vector<uint8_t> vec;
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoi(byteString, nullptr, 16));
        vec.push_back(byte);
    }
    return vec;
}

} // namespace repositories
