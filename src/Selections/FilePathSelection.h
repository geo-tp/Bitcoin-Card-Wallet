#ifndef FILEPATH_SELECTION_H
#define FILEPATH_SELECTION_H

#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Contexts/GlobalContext.h>

using namespace views;
using namespace inputs;
using namespace contexts;

namespace selections {

class FilePathSelection {
public:
    FilePathSelection(CardputerView& display, CardputerInput& input);
    std::string select(const std::vector<std::string>& elementNames, std::string folderName, std::string folderpath, uint16_t& selectionIndex);
private:
    CardputerView& display;
    CardputerInput& input;
    int16_t lastIndex;
    GlobalContext& globalContext = GlobalContext::getInstance();
    std::string toLowerCase(const std::string& input);
    void handleFirstRun();
    bool firstRun = true; // show infos about file only once
};

}

#endif // MODE_SELECTION_H
