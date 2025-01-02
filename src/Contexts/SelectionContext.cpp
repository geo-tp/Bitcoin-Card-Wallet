#include "SelectionContext.h"

namespace contexts {

SelectionContext& SelectionContext::getInstance() {
    static SelectionContext instance;
    return instance;
}

SelectionContext::SelectionContext() = default;

bool SelectionContext::getIsModeSelected() const { 
    return isModeSelected; 
}

void SelectionContext::setIsModeSelected(bool isModeSelected) { 
    this->isModeSelected = isModeSelected; 
}

SelectionModeEnum SelectionContext::getCurrentSelectedMode() const { 
    return currentSelectedMode; 
}

void SelectionContext::setCurrentSelectedMode(SelectionModeEnum mode) { 
    this->currentSelectedMode = mode; 
}

bool SelectionContext::getIsLayoutSelected() const { 
    return isLayoutSelected; 
}

void SelectionContext::setIsLayoutSelected(bool isLayoutSelected) { 
    this->isLayoutSelected = isLayoutSelected; 
}

bool SelectionContext::getIsWalletSelected() const { 
    return isWalletSelected; 
}

void SelectionContext::setIsWalletSelected(bool isWalletSelected) { 
    this->isWalletSelected = isWalletSelected; 
}

Wallet SelectionContext::getCurrentSelectedWallet() const { 
    return currentSelectedWallet; 
}

uint16_t SelectionContext::getCurrentFileIndex() const {
    return currentFileIndex;
}

void SelectionContext::setCurrentSelectedWallet(const Wallet& wallet) { 
    this->currentSelectedWallet = wallet; 
}

} // namespace contexts
