#ifndef VALUESELECTION_H
#define VALUESELECTION_H

#include <string>
#include <Inputs/CardputerInput.h>
#include <Views/CardputerView.h>
#include <Services/UsbService.h>

using namespace inputs;
using namespace views;
using namespace services;

namespace selections {

class ValueSelection {
public:
    ValueSelection(CardputerView& display, CardputerInput& input);

    void select(const std::string& description, const std::string& value, UsbService& usbService);

private:
    CardputerView& display;
    CardputerInput& input;
};

} // namespace selections

#endif // VALUESELECTION_H
