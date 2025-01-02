#ifndef FILE_BROWSER_CONTROLLER_H
#define FILE_BROWSER_CONTROLLER_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Services/SdService.h>
#include <Services/WalletService.h>
#include <Selections/FilePathSelection.h>
#include <Selections/ConfirmationSelection.h>
#include <Contexts/SelectionContext.h>
#include <Contexts/GlobalContext.h>
#include <Enums/SelectionModeEnum.h>
#include <unordered_map>
#include <vector>
#include <string>
// #include <M5Cardputer.h>

using namespace views;
using namespace inputs;
using namespace services;
using namespace selections;
using namespace contexts;
using namespace enums;

namespace controllers {

class FileBrowserController {
private:
    CardputerView& display;
    CardputerInput& input;
    SdService& sdService;
    WalletService& walletService;
    FilePathSelection& filePathSelection;
    ConfirmationSelection& confirmationSelection;
    GlobalContext& globalContext = GlobalContext::getInstance();
    SelectionContext& selectionContext = SelectionContext::getInstance();
    std::string currentPath;
    std::unordered_map<std::string, std::vector<std::string>> cachedDirectoryElements;

    std::string extractFilename(const std::string& filepath);
    std::string extractFileExtension(const std::string& filename);
    std::string getParentDirectory(const std::string& filePath);
    bool manageFile(std::string fileContent, std::string currentPath);

public:
    FileBrowserController(CardputerView& display, CardputerInput& input, SdService& sdService, WalletService& walletService,
                          FilePathSelection& filePathSelection, ConfirmationSelection& confirmationSelection);

    void handleFileWalletSelection();
    std::vector<std::string> getCachedDirectoryElements(const std::string& path);
    bool verifyWalletFile(std::string fileContent);
};

} // namespace controllers

#endif // FILE_BROWSER_CONTROLLER_H
