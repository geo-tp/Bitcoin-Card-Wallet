#include "FileBrowserManager.h"

namespace managers {

FileBrowserManager::FileBrowserManager(CardputerView& display, 
                                       CardputerInput& input, 
                                       SdService& sdService, 
                                       WalletService& walletService, 
                                       FilePathSelection& filePathSelection, 
                                       ConfirmationSelection& confirmationSelection)
    : display(display), input(input), sdService(sdService), walletService(walletService), 
      filePathSelection(filePathSelection), confirmationSelection(confirmationSelection) {}

bool FileBrowserManager::verifyWalletFile(const std::string& fileContent) {
    return fileContent.find("Filetype: Card Wallet") != std::string::npos &&
           fileContent.find("Version:") != std::string::npos;
}

bool FileBrowserManager::manageFile(const std::string& currentPath) {
    auto fileName = extractFilename(currentPath);
    auto fileExt = extractFileExtension(fileName);

    if (fileExt == "txt") {
        auto fileContent = sdService.readFile(currentPath.c_str());
        if (verifyWalletFile(fileContent)) {
            walletService.loadAllWallets(fileContent);
            selectionContext.setCurrentSelectedMode(SelectionModeEnum::PORTFOLIO);
            globalContext.setFileWalletPath(currentPath);
            display.displaySubMessage("Wallets loaded", 50, 1000);
            return true;
        } else {
            confirmationSelection.select("  Invalid wallets");
        }
    } else {
        confirmationSelection.select("Unsupported file");
    }
    return false;
}

std::string FileBrowserManager::extractFilename(const std::string& filepath) {
    size_t lastSlash = filepath.find_last_of("/\\");
    return (lastSlash == std::string::npos) ? filepath : filepath.substr(lastSlash + 1);
}

std::string FileBrowserManager::extractFileExtension(const std::string& filename) {
    size_t lastDot = filename.find_last_of('.');
    return (lastDot == std::string::npos) ? "" : filename.substr(lastDot + 1);
}

std::string FileBrowserManager::getParentDirectory(const std::string& filePath) {
    size_t lastSlash = filePath.find_last_of("/\\");
    return (lastSlash == std::string::npos) ? "/" : filePath.substr(0, lastSlash);
}

std::vector<std::string> FileBrowserManager::getCachedDirectoryElements(const std::string& path) {
    if (cachedDirectoryElements.find(path) != cachedDirectoryElements.end()) {
        return cachedDirectoryElements[path];
    }

    std::vector<std::string> elements = sdService.listElements(path);
    if (elements.size() > 4) {
        if (cachedDirectoryElements.size() >= globalContext.getFileCacheLimit()) {
            cachedDirectoryElements.erase(cachedDirectoryElements.begin());
        }
        cachedDirectoryElements[path] = elements;
    }
    return elements;
}

} // namespace managers
