#include "UsbService.h"

namespace services {

UsbService::UsbService() 
    : keyboard(), layout(KeyboardLayout_en_US) {}

void UsbService::setLayout(const uint8_t* newLayout) {
    layout = newLayout;
}

void UsbService::begin() {
    if (!initialized) {
        USB.begin();
        keyboard.begin(layout);
        initialized = true;
    }
}

// USB Keyboard send string
void UsbService::sendString(const std::string& text) {
    keyboard.releaseAll();

    for (const char& c : text) {
        keyboard.write(c);
    }
}

bool UsbService::isReady() const {
    return initialized;
}

} // namespace services
