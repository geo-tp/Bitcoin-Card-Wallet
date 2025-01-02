#ifndef SELECTION_CONTEXT_H
#define SELECTION_CONTEXT_H

#include <string>
#include <Models/Wallet.h>
#include <Enums/SelectionModeEnum.h>

using namespace enums;
using namespace models;

namespace contexts {

class SelectionContext {
public:
    // Singleton instance
    static SelectionContext& getInstance();

    // Getters and setters
    bool getIsModeSelected() const;
    void setIsModeSelected(bool isModeSelected);

    SelectionModeEnum getCurrentSelectedMode() const;
    void setCurrentSelectedMode(SelectionModeEnum mode);

    bool getIsWalletSelected() const;
    void setIsWalletSelected(bool isWalletSelected);
    
    bool getIsLayoutSelected() const;
    void setIsLayoutSelected(bool isLayoutSelected);

    Wallet getCurrentSelectedWallet() const;
    void setCurrentSelectedWallet(const Wallet& wallet);

    uint16_t getCurrentFileIndex() const;
private:
    // Private constructor for singleton
    SelectionContext();

    // Member variables
    bool isModeSelected = false;
    bool isLayoutSelected = false;
    bool isWalletSelected = false;
    SelectionModeEnum currentSelectedMode;

    Wallet currentSelectedWallet;

    size_t currentFileIndex;
    std::string currentFilePath;
};

} // namespace contexts

#endif // SELECTION_CONTEXT_H
