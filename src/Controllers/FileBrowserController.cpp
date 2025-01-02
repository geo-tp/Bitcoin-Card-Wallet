#include "FileBrowserController.h"

namespace controllers {

FileBrowserController::FileBrowserController(CardputerView& display, 
                                             CardputerInput& input, 
                                             SdService& sdService,
                                             WalletService& walletService,
                                             FilePathSelection& filePathSelection,
                                             ConfirmationSelection& confirmationSelection)
    : display(display), 
      input(input), 
      sdService(sdService),
      walletService(walletService),
      filePathSelection(filePathSelection),
      confirmationSelection(confirmationSelection),
      globalContext(GlobalContext::getInstance()),
      selectionContext(SelectionContext::getInstance()),
      currentPath("/") {}

void FileBrowserController::handleFileWalletSelection() {
    std::vector<std::string> elementNames;
    std::string fileContent;

    display.displaySubMessage("Loading", 83);
    sdService.begin(); // SD card start
    
    // No SD Card
    if (!sdService.getSdState()) {
        confirmationSelection.select("SD card not found");
        sdService.close(); // SD card stop
        selectionContext.setIsModeSelected(false);
        return;
    }

    // Get the file content if currentPath is a file or recursively go through folders
    do {
        // currentPath is a file
        if (sdService.isFile(currentPath)) {
            if(manageFile(fileContent, currentPath)) {break;}; // succesfuly loaded

            // Revert to parent dir, currentPath was not a valid wallet file
            currentPath = getParentDirectory(currentPath);
            currentPath = currentPath.empty() ? "/" : currentPath;
        }

        // Get currentPath folder elements from the cache if exist or get them from the sd card
        display.displaySubMessage("Loading", 83);
        elementNames = getCachedDirectoryElements(currentPath); 
        
        // Select the file or folder
        uint16_t currentIndex = selectionContext.getCurrentFileIndex();
        currentPath = filePathSelection.select(elementNames, extractFilename(currentPath), currentPath, currentIndex);

    } while (!currentPath.empty()); // user hits the return button at root path

    sdService.close(); // SD card stop
    currentPath = "/"; // reset to root path

    if (selectionContext.getCurrentSelectedMode() != SelectionModeEnum::PORTFOLIO) {
        selectionContext.setIsModeSelected(false); // go back to menu
    }
}

bool FileBrowserController::manageFile(std::string fileContent, std::string currentPath) {
    auto fileName = extractFilename(currentPath);
    auto fileExt = extractFileExtension(fileName);

    // Valid file ext
    if (fileExt == "txt") {
        fileContent = sdService.readFile(currentPath.c_str());

        // Valid wallet file
        if (verifyWalletFile(fileContent)) {
            walletService.loadAllWallets(fileContent);
            selectionContext.setCurrentSelectedMode(SelectionModeEnum::PORTFOLIO);
            globalContext.setFileWalletPath(currentPath);
            display.displaySubMessage("Wallets loaded", 50, 1000);
            return true; // wallets loaded successfully

        // Not valid wallet file
        } else {
        confirmationSelection.select("  Invalid wallets");
        }
    // Not a valid file ext
    } else {
        confirmationSelection.select("Unsupported file");
    }
    
    return false;
}

bool FileBrowserController::verifyWalletFile(std::string fileContent) {
    if (fileContent.find("Filetype: Card Wallet") == std::string::npos ||
        fileContent.find("Version:") == std::string::npos) {
        return false;
    }

    return true;
}

std::string FileBrowserController::extractFilename(const std::string& filepath) {
    size_t lastSlash = filepath.find_last_of("/\\");
    return (lastSlash == std::string::npos) ? filepath : filepath.substr(lastSlash + 1);
}

std::string FileBrowserController::extractFileExtension(const std::string& filename) {
    size_t lastDot = filename.find_last_of('.');
    return (lastDot == std::string::npos) ? "" : filename.substr(lastDot + 1);
}

std::string FileBrowserController::getParentDirectory(const std::string& filePath) {
    size_t lastSlash = filePath.find_last_of("/\\");
    return (lastSlash == std::string::npos) ? "/" : filePath.substr(0, lastSlash);
}

std::vector<std::string> FileBrowserController::getCachedDirectoryElements(const std::string& path) {
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

} // namespace controllers
