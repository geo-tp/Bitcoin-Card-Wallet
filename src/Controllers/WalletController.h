#ifndef WALLET_CONTROLLER_H
#define WALLET_CONTROLLER_H

#include <Contexts/GlobalContext.h>
#include <Contexts/SelectionContext.h>
#include <Enums/WalletInformationEnum.h>
#include <Managers/WalletManager.h>

using namespace views;
using namespace inputs;
using namespace services;
using namespace selections;
using namespace contexts;
using namespace managers;

namespace controllers {

class WalletController {
private:
    WalletManager& manager;
    GlobalContext& globalContext = GlobalContext::getInstance();
    SelectionContext& selectionContext = SelectionContext::getInstance();

public:
    WalletController(WalletManager& walletManager);

    void handleWalletSelection();
    void handleWalletInformationSelection();
};

} // namespace controllers

#endif // WALLET_CONTROLLER_H
