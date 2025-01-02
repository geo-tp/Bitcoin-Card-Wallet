#include <M5Cardputer.h>
#include <Views/CardputerView.h>
#include <Inputs/CardputerInput.h>
#include <Dispatchers/AppDispatcher.h>
#include <WiFi.h>

using namespace dispatchers;

CardputerView display;
CardputerInput input;
AppDispatcher* dispatcher;

void setup() {
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);

    WiFi.disconnect(true, true); // to be sure

    dispatcher = new AppDispatcher(display, input);
    dispatcher->setup();
}

void loop() {
    dispatcher->run();
}
