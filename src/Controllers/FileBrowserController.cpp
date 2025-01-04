#include "FileBrowserController.h"

namespace controllers {

FileBrowserController::FileBrowserController(FileBrowserManager& manager)
    : manager(manager),
      currentPath("/") {}

void FileBrowserController::handleFileWalletSelection() {
    std::vector<std::string> elementNames;
    std::string fileContent;

    manager.display.displaySubMessage("Loading", 83);
    manager.sdService.begin(); // SD card start
    
    // No SD Card
    if (!manager.sdService.getSdState()) {
        manager.display.displaySubMessage("SD card not found", 38, 2000);
        manager.sdService.close(); // SD card stop
        manager.selectionContext.setIsModeSelected(false);
        return;
    }

    // Get the file content if currentPath is a file or recursively go through folders
    do {
        // currentPath is a file
        if (manager.sdService.isFile(currentPath)) {
            if(manager.manageFile(currentPath)) {break;}; // succesfuly loaded

            // Revert to parent dir, currentPath was not a valid wallet file
            currentPath = manager.getParentDirectory(currentPath);
            currentPath = currentPath.empty() ? "/" : currentPath;
        }

        // Get currentPath folder elements from the cache if exist or get them from the sd card
        manager.display.displaySubMessage("Loading", 83);
        elementNames = manager.getCachedDirectoryElements(currentPath); 
        
        // Select the file or folder
        uint16_t currentIndex = selectionContext.getCurrentFileIndex();
        currentPath = manager.filePathSelection.select(elementNames, manager.extractFilename(currentPath), currentPath, currentIndex);

    } while (!currentPath.empty()); // user hits the return button at root path

    manager.sdService.close(); // SD card stop
    currentPath = "/"; // reset to root path

    if (selectionContext.getCurrentSelectedMode() != SelectionModeEnum::PORTFOLIO) {
        selectionContext.setIsModeSelected(false); // go back to menu
    }
}

} // namespace controllers
