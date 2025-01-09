#ifndef WALLET_SERVICE_H
#define WALLET_SERVICE_H

#include <string>
#include <vector>
#include <Repositories/WalletRepository.h>
#include <Models/Wallet.h>

using namespace models;
using namespace repositories;

namespace services {

class WalletService {
private:
    WalletRepository& repository;  // Référence au repository
    std::vector<Wallet> wallets;   // Copie locale des wallets pour gestion en mémoire

public:
    WalletService(WalletRepository& repository);

    bool addWallet(const Wallet& wallet);
    bool updateWallet(const Wallet& updatedWallet);
    bool deleteWallet(const std::string& walletName); 
    Wallet getWallet(const std::string& walletName);
    std::vector<Wallet> getAllWallets();
    void loadAllWallets(const std::string& fileContent);
    std::string getWalletsFileContent();
    std::string publicKeyToHexString(const std::vector<uint8_t>& vec);
};

} // namespace services

#endif // WALLET_SERVICE_H
