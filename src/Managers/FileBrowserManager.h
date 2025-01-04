#ifndef FILE_BROWSER_MANAGER_H
#define FILE_BROWSER_MANAGER_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Services/SdService.h>
#include <Services/WalletService.h>
#include <Selections/FilePathSelection.h>
#include <Selections/ConfirmationSelection.h>
#include <Contexts/SelectionContext.h>
#include <Contexts/GlobalContext.h>
#include <unordered_map>
#include <vector>
#include <string>

using namespace views;
using namespace inputs;
using namespace services;
using namespace selections;
using namespace contexts;

namespace managers {

class FileBrowserManager {
public:
    CardputerView& display;
    CardputerInput& input;
    SdService& sdService;
    WalletService& walletService;
    FilePathSelection& filePathSelection;
    ConfirmationSelection& confirmationSelection;
    GlobalContext& globalContext = GlobalContext::getInstance();
    SelectionContext& selectionContext = SelectionContext::getInstance();
    std::unordered_map<std::string, std::vector<std::string>> cachedDirectoryElements;

    FileBrowserManager(CardputerView& display, CardputerInput& input, SdService& sdService, WalletService& walletService,
                       FilePathSelection& filePathSelection, ConfirmationSelection& confirmationSelection);

    std::string extractFilename(const std::string& filepath);
    std::string extractFileExtension(const std::string& filename);
    std::string getParentDirectory(const std::string& filePath);
    bool verifyWalletFile(const std::string& fileContent);
    bool manageFile(const std::string& currentPath);
    std::vector<std::string> getCachedDirectoryElements(const std::string& path);
};

} // namespace managers

#endif // FILE_BROWSER_MANAGER_H
