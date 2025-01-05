#ifndef SEED_RESTORATION_MODE_H
#define SEED_RESTORATION_MODE_H

namespace enums {

enum class SeedRestorationModeEnum {
    NONE = -1, // No choice
    RFID,      // Restoration via RFID tag
    SD,        // Restoration via SD card
    WORDS_12,  // Restoration via 12 words mnemonic
    WORDS_24,  
    COUNT
};

} // namespace enums

#endif // SEED_RESTORATION_MODE_H
