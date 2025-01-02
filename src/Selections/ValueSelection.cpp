#include "ValueSelection.h"

namespace selections {

ValueSelection::ValueSelection(CardputerView& display, CardputerInput& input)
    : display(display), input(input) {}

void ValueSelection::select(const std::string& description, const std::string& value, UsbService& usbService) {
    char key = KEY_NONE;

    display.displayTopBar(description, true, false, false, 20);
    display.displayWalletValue(description, value);

    while (key != KEY_RETURN_CUSTOM) {
        key = input.handler();

        switch (key) {
            case KEY_OK: // Send USB
                usbService.sendString(value);
                break;
            case 'q': // QR Code
                display.setBrightness(50);
                display.displayQrCode(value);
                display.displayTopIcon();
                input.waitPress();
                display.setBrightness(120);
                display.displayTopBar(description, true, false, false, 20);
                display.displayWalletValue(description, value); 
                break;
        }
    }
}

} // namespace selections
