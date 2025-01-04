#ifndef SEED_MANAGER_H
#define SEED_MANAGER_H

#include <vector>
#include <string>
#include "Services/CryptoService.h"
#include "Services/RfidService.h"
#include "Services/SdService.h"
#include "Services/WalletService.h"
#include "Selections/ConfirmationSelection.h"
#include "Selections/StringPromptSelection.h"

using namespace services;
using namespace selections;

namespace managers {

class SeedManager {
public:
    SeedManager(CryptoService& cryptoService,
                RfidService& rfidService,
                SdService& sdService,
                WalletService& walletService,
                ConfirmationSelection& confirmationSelection,
                StringPromptSelection& stringPromptSelection);

    std::tuple<std::vector<uint8_t>, std::string> manageEncryption(const std::vector<uint8_t>& privateKey);
    std::vector<uint8_t> manageDecryption();
    std::vector<uint8_t> generatePrivateKey();
    bool saveToRfid(const std::vector<uint8_t>& privateKey, const std::string& salt, const std::vector<uint8_t>& signature);
    std::vector<uint8_t> readFromRfid();

private:
    CryptoService& cryptoService;
    RfidService& rfidService;
    SdService& sdService;
    WalletService& walletService;
    ConfirmationSelection& confirmationSelection;
    StringPromptSelection& stringPromptSelection;
};

} // namespace managers

#endif // SEED_MANAGER_H
