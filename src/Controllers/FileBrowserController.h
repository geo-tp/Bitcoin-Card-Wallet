#ifndef FILE_BROWSER_CONTROLLER_H
#define FILE_BROWSER_CONTROLLER_H

#include <Managers/FileBrowserManager.h>
#include <Contexts/SelectionContext.h>
#include <Contexts/GlobalContext.h>
#include <Enums/SelectionModeEnum.h>
#include <vector>
#include <string>

using namespace managers;
using namespace contexts;
using namespace enums;

namespace controllers {

class FileBrowserController {
private:
    FileBrowserManager& manager;
    GlobalContext& globalContext = GlobalContext::getInstance();
    SelectionContext& selectionContext = SelectionContext::getInstance();
    std::string currentPath;

public:
    FileBrowserController(FileBrowserManager& manager);

    void handleFileWalletSelection();
};

} // namespace controllers

#endif // FILE_BROWSER_CONTROLLER_H
