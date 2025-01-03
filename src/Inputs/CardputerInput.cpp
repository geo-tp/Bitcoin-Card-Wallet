#include "CardputerInput.h"

namespace inputs {


char CardputerInput::handler() {

    // Update keyboard state
    M5Cardputer.update();

    // Bouton GO
    if (M5Cardputer.BtnA.isPressed()) {
        delay(150); // debounce
        return KEY_RETURN_CUSTOM;
    }
    
    if (M5Cardputer.Keyboard.isChange()) {

        if (M5Cardputer.Keyboard.isPressed()) {
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
            entropyContext.tick();

            if (status.enter) { // go to next menu
                return KEY_OK;
            }
            if (status.del) { 
                return KEY_DEL;
            }
            
            if(M5Cardputer.Keyboard.isKeyPressed(KEY_ARROW_LEFT)) { // go back to previous menu
                return KEY_RETURN_CUSTOM;
            }

            if(M5Cardputer.Keyboard.isKeyPressed(KEY_ARROW_RIGHT)) { // go to next menu
                return KEY_ARROW_RIGHT;
            }

            for (auto c : status.word) {
                entropyContext.add(c); // get some entropy
                return c; // retourner le premier char saisi
            }
        }
    }
    delay(10); // debounce
    return KEY_NONE;
}

void CardputerInput::waitPress() {
  while(1){
    M5Cardputer.update();
    if (M5Cardputer.Keyboard.isChange()) {
      if (M5Cardputer.Keyboard.isPressed()) {
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
            entropyContext.tick();
            for (auto c : status.word) {
                entropyContext.add(c); // get some entropy
            }
        return;
      }
    }
    delay(5);
  }
}

}