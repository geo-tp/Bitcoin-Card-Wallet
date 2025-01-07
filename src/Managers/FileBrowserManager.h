#ifndef FILE_BROWSER_MANAGER_H
#define FILE_BROWSER_MANAGER_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Services/SdService.h>
#include <Services/WalletService.h>
#include <Services/CryptoService.h>
#include <Selections/FilePathSelection.h>
#include <Selections/ConfirmationSelection.h>
#include <Selections/StringPromptSelection.h>
#include <Contexts/SelectionContext.h>
#include <Contexts/GlobalContext.h>
#include <Managers/GlobalManager.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <iterator>

using namespace views;
using namespace inputs;
using namespace services;
using namespace selections;
using namespace contexts;

namespace managers {

class FileBrowserManager : public GlobalManager {
public:
    FileBrowserManager(const GlobalManager& gm);

    std::unordered_map<std::string, std::vector<std::string>> cachedDirectoryElements;

    std::string extractFilename(const std::string& filepath);
    std::string extractFileExtension(const std::string& filename);
    std::string getParentDirectory(const std::string& filePath);
    bool loadFile(std::string currentPath, FileTypeEnum selectedFileType);    
    bool verifyWalletFile(const std::string& fileContent);
    bool manageWalletFile(const std::string& currentPath);
    bool manageTransactionFile(const std::string& currentPath);
    bool verifySeedFile(const std::string& fileContent);
    bool manageSeedRestorationFile(const std::string& currentPath);
    bool manageSeedLoadingFile(const std::string& currentPath);
    std::vector<std::string> getCachedDirectoryElements(const std::string& path);
    void removeCachedDirectoryElement(const std::string& path);
};

} // namespace managers

#endif // FILE_BROWSER_MANAGER_H
