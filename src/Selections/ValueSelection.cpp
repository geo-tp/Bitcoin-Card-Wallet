#include "ValueSelection.h"

namespace selections {

ValueSelection::ValueSelection(CardputerView& display, CardputerInput& input)
    : display(display), input(input) {}

void ValueSelection::select(const std::string& description, const std::string& value, UsbService& usbService, LedService& ledService) {
    char key = KEY_NONE;
    bool firstOkPress = true;
    unsigned long loopStartTime = millis();  // used to wait for keyboard init

    display.displayTopBar(description, true, false, false, 20);
    display.displayWalletValue(description, value);

    while (key != KEY_RETURN_CUSTOM) {
        key = input.handler();

        switch (key) {
            case KEY_OK: // Send USB
                ledService.showLed();
                if (firstOkPress) { // hid needs approx 1.5sec to init
                    unsigned long elapsed = millis() - loopStartTime;
                    if (elapsed < 1500) {delay(1500 - elapsed);}
                    firstOkPress = false;
                }
                usbService.sendString(value);
                ledService.clearLed();
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
