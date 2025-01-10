#include "WalletRepository.h"

namespace repositories {

bool WalletRepository::addWallet(const Wallet& wallet) {
    wallets.push_back(wallet);
    return true;
}

bool WalletRepository::updateWallet(const Wallet& updatedWallet) {
    for (auto& wallet : wallets) {
        if (wallet.getZPub() == updatedWallet.getZPub() && 
            wallet.getName() == updatedWallet.getName()) {
            wallet = updatedWallet;
            return true;
        }
    }
    return false; 
}

bool WalletRepository::deleteWallet(const std::string& walletName) {
    auto it = std::remove_if(wallets.begin(), wallets.end(), [&walletName](const Wallet& w) {
        return w.getName() == walletName;
    });

    if (it != wallets.end()) {
        wallets.erase(it, wallets.end()); 
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
    auto maxWallets =  globalContext.getMaxAllowedWallet();

    while (std::getline(stream, line)) {
        // Detecter le début d'un wallet
        if (line.find("# WALLET") != std::string::npos) {
            currentWalletData.clear(); // Reset for new wallet

            // The 5 following lines are: Name, zPub, BitcoinAddress, Fingerprint, DerivePath
            for (int i = 0; i < 5; ++i) {
                if (std::getline(stream, line) && !line.empty()) {
                    currentWalletData += line + "\n";
                }
            }

            if (!currentWalletData.empty()) {
                walletDataList.push_back(currentWalletData);
                // Max allowed wallet count
                if (walletDataList.size() >= maxWallets) {
                    return walletDataList;
                }
            }
        }
    }
    return walletDataList;
}

Wallet WalletRepository::parseWallet(const std::string& walletData) {
    std::string name;
    std::string zPub;
    std::string xPub;
    std::string address;
    std::string fingerprint;
    std::string derivePath;

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
        } else if (key == "zPub") {
            zPub = value;
        } else if (key == "BitcoinAddress") {
            address = value;
        } else if (key == "Fingerprint") {
            fingerprint = value;
        } else if (key == "DerivePath") {
            derivePath = value;
        }
    }

    return Wallet(name, zPub, address, "", fingerprint, derivePath);
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
    fileContent << "Version: 2\n";
    fileContent << "\n";

    // Wallets
    for (size_t i = 0; i < wallets.size(); ++i) {
        const auto& wallet = wallets[i];

        std::string zPub = wallet.getZPub();
        std::string xPub = wallet.getXPub();
        std::string fingerprint = wallet.getFingerprint();
        std::string derivePath = wallet.getDerivePath();

        fileContent << "# WALLET " << (i + 1) << "\n";
        fileContent << "Name: " << wallet.getName().c_str() << "\n";
        fileContent << "zPub: " << zPub.c_str() << "\n";
        fileContent << "BitcoinAddress: " << wallet.getAddress().c_str() << "\n";
        fileContent << "Fingerprint: " << fingerprint.c_str() << "\n";
        fileContent << "DerivePath: " << derivePath.c_str() << "\n";
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
