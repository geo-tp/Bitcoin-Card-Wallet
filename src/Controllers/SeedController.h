#ifndef SEED_CONTROLLER_H
#define SEED_CONTROLLER_H

#include <Managers/SeedManager.h>
#include <Models/Wallet.h>
#include <Contexts/SelectionContext.h>
#include <Contexts/GlobalContext.h>
#include <Enums/SeedRestorationModeEnum.h>
#include <vector>
#include <string>

using namespace contexts;
using namespace managers;

namespace controllers {

class SeedController {
private:
    SeedManager& manager;
    SelectionContext& selectionContext = SelectionContext::getInstance();
    GlobalContext& globalContext = GlobalContext::getInstance();
public:
    SeedController(SeedManager& seedManager);

    void handleSeedGeneration();
    void handleSeedInformations();
    void handleSeedRestoration();
};

} // namespace controllers

#endif // SEED_CONTROLLER_H
