#ifndef WALLET_MANAGER_H
#define WALLET_MANAGER_H

#include <Managers/GlobalManager.h>
#include <Contexts/GlobalContext.h>
#include <Contexts/SelectionContext.h>

using namespace selections;
using namespace services;
using namespace contexts;

namespace managers {

class WalletManager : public GlobalManager {
public:
    WalletManager(const GlobalManager& gm);

    GlobalContext& globalContext = GlobalContext::getInstance();
    SelectionContext& selectionContext = SelectionContext::getInstance();
};

} // namespace managers

#endif // WALLET_CONTROLLER_H
