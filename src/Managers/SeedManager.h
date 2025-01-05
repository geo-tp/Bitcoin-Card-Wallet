#ifndef SEED_MANAGER_H
#define SEED_MANAGER_H

#include "GlobalManager.h"
#include <Models/Wallet.h>
#include <vector>
#include <string>
#include <tuple>

namespace managers {

class SeedManager : public GlobalManager {
public:
    // Constructor taking a reference to an existing GlobalManager
    SeedManager(const GlobalManager& gm);

    // Additional seed-specific methods
    void manageMnemonic(std::vector<std::string>& mnemonic);
    bool manageMnemonicRestore(size_t wordCount);
    std::vector<uint8_t> managePrivateKey();
    void manageRfidSeedRestoration();
    void manageNewSeedCreation();
};

} // namespace managers

#endif // SEED_MANAGER_H
