#include "WalletService.h"

namespace services {

WalletService::WalletService(WalletRepository& repository)
: repository(repository) {}

bool WalletService::addWallet(const Wallet& wallet) {
    if (repository.addWallet(wallet)) {
        return true;
    }
    return false;
}

bool WalletService::updateWallet(const Wallet& updatedWallet) {
    return repository.updateWallet(updatedWallet);
}

bool WalletService::deleteWallet(const std::string& walletName) {
    return repository.deleteWallet(walletName);
}

Wallet WalletService::getWallet(const std::string& walletName) {
    for (const auto& wallet : repository.getWallets()) {
        if (wallet.getName() == walletName) {
            return wallet;
        }
    }
    return Wallet(); // Retourne un wallet vide si non trouvé
}

std::vector<Wallet> WalletService::getAllWallets() {
    return repository.getWallets();
}

void WalletService::loadAllWallets(const std::string& fileContent) {
    repository.loadAllWallets(fileContent);
}

std::string WalletService::getWalletsFileContent() {
    return repository.getWalletsFileContent();
}

std::string WalletService::publicKeyToHexString(const std::vector<uint8_t>& vec) {
    return repository.vectorToHexString(vec);
}

} // namespace services
