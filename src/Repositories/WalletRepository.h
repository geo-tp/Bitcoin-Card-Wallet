#ifndef WALLET_REPOSITORY_H
#define WALLET_REPOSITORY_H

#include <string>
#include <vector>
#include <mbedtls/base64.h>
#include <sstream>
#include <Models/Wallet.h>
#include <algorithm>
#include <M5Cardputer.h>
#include <iomanip>
#include <Contexts/GlobalContext.h>

using namespace models;
using namespace contexts;

namespace repositories {

class WalletRepository {
public:
    void loadAllWallets(const std::string& fileContent);
    std::string getWalletsFileContent();

    bool addWallet(const Wallet& wallet);
    bool updateWallet(const Wallet& updatedWallet);
    bool deleteWallet(const std::string& walletName);
    const std::vector<Wallet>& getWallets() const;
    std::string vectorToHexString(const std::vector<uint8_t>& vec);

private:
    std::vector<Wallet> wallets;
    std::vector<std::string> splitWallets(const std::string& walletData);
    models::Wallet parseWallet(const std::string& walletData);
    std::vector<uint8_t> hexStringToVector(const std::string& str);
    GlobalContext& globalContext = GlobalContext::getInstance();
};

} // namespace repositories

#endif // WALLET_REPOSITORY_H
