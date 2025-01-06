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

void SelectionContext::setCurrentSelectedWallet(const Wallet& wallet) { 
    this->currentSelectedWallet = wallet; 
}

uint16_t SelectionContext::getCurrentFileIndex() const {
    return static_cast<uint16_t>(currentFileIndex);
}

FileTypeEnum SelectionContext::getCurrentSelectedFileType() const {
    return currentSelectedFileType;
}

void SelectionContext::setCurrentSelectedFileType(FileTypeEnum fileType) {
    this->currentSelectedFileType = fileType;
}

bool SelectionContext::getTransactionOngoing() const {
    return transactionOngoing;
}

void SelectionContext::setTransactionOngoing(bool transactionOngoing) {
    this->transactionOngoing = transactionOngoing;
}

} // namespace contexts
