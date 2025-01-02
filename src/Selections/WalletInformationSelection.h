#ifndef WALLET_INFORMATION_SELECTION_H
#define WALLET_INFORMATION_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Contexts/GlobalContext.h>
#include <Enums/WalletInformationEnum.h>

using namespace views;
using namespace inputs;
using namespace contexts;
using namespace enums;

namespace selections {

class WalletInformationSelection {
public:
    WalletInformationSelection(CardputerView& display, CardputerInput& input);
    WalletInformationEnum select(const std::string& walletName);

    static const std::string getWalletInformationToString(WalletInformationEnum info);
    static const std::vector<std::string> getWalletInformationStrings();

private:
    CardputerView& display;
    CardputerInput& input;
    GlobalContext& globalContext = GlobalContext::getInstance();
    uint8_t selectionIndex;
    int8_t lastIndex;
};

}

#endif // WALLET_INFORMATION_SELECTION_H
