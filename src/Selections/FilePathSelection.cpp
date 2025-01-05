#include "FilePathSelection.h"

namespace selections {

FilePathSelection::FilePathSelection(CardputerView& display, CardputerInput& input)
    : display(display), input(input), lastIndex(-1) {}

std::string FilePathSelection::select(const std::vector<std::string>& elementNames, std::string folderName, 
                                      std::string folderPath, uint16_t& selectionIndex, FileTypeEnum fileType) {
    lastIndex = -1;
    char key = KEY_NONE;
    std::vector<std::string> filteredNames = elementNames;
    std::string searchQuery = "";
    std::string lowerSearchQuery;
    std::string lowerName;
    size_t lastSlashPos;
    folderName = folderName == "" ? "root" : folderName; // if foldeName is "" then folderName is "root"

    // Info about file on first run
    handleFirstRun(fileType);
    display.displayTopBar(searchQuery != "" ? searchQuery : folderName, true, true);

    while (key != KEY_OK && key != KEY_ARROW_RIGHT) {
        // Update display is index has changed
        if (lastIndex != selectionIndex) {
            display.displaySelection(filteredNames, selectionIndex);
            lastIndex = selectionIndex;
        }

        key = input.handler();
        switch (key) {
            case KEY_NONE:
                break; // to avoid check other cases

            case KEY_OK:
                break; // to avoid check other cases

            case KEY_ARROW_DOWN:
                if (selectionIndex < filteredNames.size() - 1) {
                    selectionIndex++;
                } else {
                    selectionIndex = 0;
                }
                break;

            case KEY_ARROW_UP:
                if (selectionIndex > 0) {
                    selectionIndex--;
                } else {
                    selectionIndex = filteredNames.size() - 1;
                }
                break;

            case KEY_RETURN_CUSTOM:

                // Trouver la position du dernier slash dans le chemin
                lastSlashPos = folderPath.find_last_of("/\\");

                // Fin de la selection, sortie du mode
                if (lastSlashPos == 0 && folderPath == "/") {
                    return "";
                }

                // si le chemin est à 1 niveau de sous dossier, ex "/folder", retourner "/"
                if (lastSlashPos == 0) {
                    return "/";
                }

                return folderPath.substr(0, lastSlashPos);

            default:
                if (key == KEY_DEL) {
                    if (searchQuery.length() > 0) {
                        searchQuery.pop_back();
                    }
                } else {
                    searchQuery += key;
                    selectionIndex = 0; // Reset the selection after each char entry
                }

                // Search with user typed string
                if (searchQuery.empty() == true) {
                    filteredNames = elementNames; // revert to full list
                } else {
                    filteredNames = {};
                    selectionIndex = 0;
                    lowerSearchQuery = toLowerCase(searchQuery);

                    // Filter the list based on the search bar, compare uppercase
                    for (const auto& name : elementNames) {
                        lowerName = toLowerCase(name);
                        if (lowerName.find(lowerSearchQuery) != std::string::npos) {
                            filteredNames.push_back(name);
                        }
                    }
                }
                break;

        }

        if (key != KEY_NONE) {
            // Update screen
            display.displayTopBar(searchQuery != "" ? searchQuery : folderName, true, true);
            display.displaySelection(filteredNames, selectionIndex);
        }
    }

    if (folderPath != "/") {
        folderPath += "/";
    }
    
    return folderPath + filteredNames[selectionIndex];
}

void FilePathSelection::handleFirstRun(FileTypeEnum fileType) {
    if (!firstRun) {return;}

    switch (fileType) {
        case FileTypeEnum::WALLET:
            display.displayWalletFileInfo(globalContext.getfileWalletDefaultPath());
            input.waitPress();
            break;
        case FileTypeEnum::SEED:
            display.displaySubMessage("12 or 24 words txt file", 19, 2000);
            break;
    }
    firstRun = false;
}

std::string FilePathSelection::toLowerCase(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

}