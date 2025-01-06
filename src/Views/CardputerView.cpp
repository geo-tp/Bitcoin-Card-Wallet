#include "CardputerView.h"

namespace views {

M5GFX* CardputerView::Display = nullptr;

void CardputerView::initialize() {
    Display = &M5Cardputer.Display;
    Display->setRotation(1);
    Display->setTextColor(TEXT_COLOR);
    Display->fillScreen(BACKGROUND_COLOR);
    M5Cardputer.Display.setTextDatum(middle_center);
    M5Cardputer.Display.setFont(&fonts::Orbitron_Light_24);
}

void CardputerView::displayTopBar(const std::string& title, bool submenu, bool searchBar, bool bitcoinIcon, size_t correctionOffset) {
    uint8_t marginX = 4;
    uint8_t marginY = 14;
    float offsetX; // for text align
    size_t limiter; // char limitation
    float sizeText; // pixels offset depending on text size

    clearTopBar();

    if (submenu) {
        drawSubMenuReturn(marginX+3, marginY); // for return <
        limiter = 20; // limit string size
        sizeText = 5.1; // pixels offset for each char
    } else {
        Display->setTextSize(TEXT_LARGE);
        limiter = 16;
        sizeText = 6.95;
    }

    // To center text
    offsetX = getTextCenterOffset(title, Display->width(), sizeText) - correctionOffset;
    
    if (searchBar) {
        Display->setTextColor(TEXT_COLOR);

        // Empty search query
        const std::string searchQuery = title.empty() ? "Type to search" : title.substr(0, limiter);
        drawSearchIcon(Display->width() - 20, marginY-2, 10, PRIMARY_COLOR);

        Display->setCursor(offsetX, marginY);
        Display->printf(searchQuery.c_str());
    } else {
        Display->setTextColor(TEXT_COLOR);

        Display->setCursor(offsetX+8, marginY);
        Display->setTextSize(TEXT_BIG);
        Display->printf(title.c_str());
    }

    if (bitcoinIcon) {
        drawBitcoinIcon(offsetX-22, 3);
    }
}


void CardputerView::displaySelection(
                    const std::vector<std::string>& selectionStrings,  
                    uint16_t selectionIndex, const std::vector<std::string>& selectionDescription, 
                    bool upperCase, bool showCurrency){

    uint8_t sizeX = Display->width() - 10; // width of each block
    uint8_t sizeY = 22; // height of each block
    uint8_t startY = 30; // height start block
    uint8_t stepY = 26; // step between each block
    uint8_t margin = DEFAULT_MARGIN;
    uint8_t startText = 41; // where text for each block starts
    uint8_t marginText; // width start block
    uint8_t rowsPerScreen = 4;
    size_t currentIndex; // up to date index
    bool selected; // track selected row
    uint16_t currentStartRow = selectionIndex / rowsPerScreen * rowsPerScreen;
    std::string upperString;

    displayClearMainView();

    // for filtering with no results
    if (selectionStrings.empty()) {
        Display->drawCenterString("No results", Display->width() / 2, Display->height() / 2);
    }

    for (size_t i = 0; i < rowsPerScreen && (currentStartRow + i) < selectionStrings.size(); ++i) {
        currentIndex = currentStartRow + i;
        selected = (currentIndex == selectionIndex);
        Display->setTextSize(TEXT_WIDE);
        
        drawRect(selected, margin, startY, sizeX, sizeY, stepY * i);

        // Margin
        marginText = DEFAULT_MARGIN * 2;

        Display->setCursor(marginText + margin, startText + stepY * i);

        // Limiter 
        const char* limiter = "%.17s";

        if (upperCase) {
            upperString = toUpperCase(selectionStrings[currentIndex]);
            Display->printf(upperString.c_str());
        } else { 
            Display->printf(selectionStrings[currentIndex].c_str());
        }

        if (showCurrency) {
            Display->setTextSize(TEXT_WIDE);
            Display->fillRoundRect(Display->width() - 44, startY + stepY * i - 1, 39, sizeY, DEFAULT_ROUND_RECT, RECT_COLOR_DARK);
            Display->drawRoundRect(Display->width() - 45, startY + stepY * i, 40, sizeY, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
            Display->setCursor(Display->width() - 38, startText + stepY * i);
            Display->printf("btc");
        }

        // Description
        if (selectionDescription.size() != 0) {
            Display->setTextSize(TEXT_TINY);
            Display->setCursor(selectionStrings[currentIndex].size() * 15, startText + stepY * i);
            Display->printf(selectionDescription[currentIndex].c_str());
        }
    }
}

void CardputerView::displayWalletFileInfo(std::string defaultFileName) {
    Display->fillScreen(BACKGROUND_COLOR);

    // Box frame
    Display->drawRect(1, 1, Display->width() - 1, Display->height() - 1, PRIMARY_COLOR);

    // Main title
    Display->setTextSize(TEXT_BIG);
    Display->setCursor(50, 22);
    Display->setTextColor(PRIMARY_COLOR);
    Display->printf("About File");

    // Sub title
    Display->setTextSize(TEXT_SMALL);
    Display->setTextColor(TEXT_COLOR);
    Display->setCursor(32, 46);
    Display->printf("When you create a wallet");

    // Text
    Display->setCursor(32, 65);
    Display->printf("it is saved on the SD card");
    Display->setTextColor(PRIMARY_COLOR);
    Display->setCursor(22, 88);
    Display->setTextSize(TEXT_MEDIUM_LARGE);
    Display->printf(defaultFileName.c_str());

    // Button OK
    Display->fillRoundRect(70, 105, 100, 20, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    Display->setTextColor(TEXT_COLOR);
    Display->setTextSize(TEXT_MEDIUM);
    Display->setCursor(80, 115);
    Display->printf("OK to start");
}

void CardputerView::displayStringPrompt(std::string stringDescription, std::string stringInput, size_t offsetX, bool backButton) {
    // Clear
    displayClearMainView(5);

    // Box frame
    Display->drawRoundRect(10, 35, Display->width() - 20, 90, DEFAULT_ROUND_RECT, PRIMARY_COLOR);

    // Description
    Display->setTextSize(TEXT_MEDIUM);
    Display->setTextColor(TEXT_COLOR);
    Display->setCursor(53-offsetX, 48);
    Display->printf(stringDescription.c_str());

    // Check the length of the input and truncate if necessary
    std::string truncatedInput;
    if (stringInput.length() > 15) {
        truncatedInput = stringInput.substr(stringInput.length() - 15); // Get the last 15 characters
    } else {
        truncatedInput = stringInput;
    }

    // input
    Display->setTextSize(TEXT_MEDIUM_LARGE);
    Display->drawRoundRect(42, 62, 155, 25, DEFAULT_ROUND_RECT, RECT_COLOR_DARK);
    Display->setCursor(51, 73);
    Display->printf(truncatedInput.c_str());

    size_t xPos = 110; 
    if (backButton) {
        // < button
        Display->drawRoundRect(53, 95, 40, 20, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
        Display->setCursor(70, 103);
        Display->printf("<");
        xPos = 135;
    }

    // Button save
    if (stringInput.length() < 3) {
        Display->drawRoundRect(xPos-30, 95, 80, 20, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    } else {
        Display->fillRoundRect(xPos-30, 95, 80, 20, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    }

    Display->setTextSize(TEXT_MEDIUM);
    Display->setCursor(xPos, 105);
    Display->printf("OK");
}

void CardputerView::displayConfirmationPrompt(std::string stringDescription) {
    // Clear
    displayClearMainView(5);

    // Box frame
    Display->drawRoundRect(10, 35, Display->width() - 20, 90, DEFAULT_ROUND_RECT, PRIMARY_COLOR);

    // Description
    Display->setTextSize(TEXT_MEDIUM);
    Display->setTextColor(TEXT_COLOR);
    Display->setCursor(57, 62);
    Display->printf(stringDescription.c_str());
    Display->setTextSize(TEXT_MEDIUM);

    // < button
    Display->drawRoundRect(65, 85, 40, 20, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    Display->setCursor(82, 94);
    Display->printf("<");

    // ok button
    Display->fillRoundRect(128, 85, 40, 20, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    Display->setCursor(138, 95);
    Display->printf("OK");
}

void CardputerView::displaySubMessage(std::string message, size_t x, int delayMs) {
    // Clear
    displayClearMainView(5);

    // Box frame
    Display->drawRoundRect(10, 35, Display->width() - 20, 90, DEFAULT_ROUND_RECT, PRIMARY_COLOR);

    // Description
    Display->setTextSize(TEXT_WIDE);
    Display->setTextColor(TEXT_COLOR);
    Display->setCursor(x, 80);
    Display->printf(message.c_str());
    Display->setTextSize(TEXT_MEDIUM);

    if (delayMs) {
        delay(delayMs);
    }
}

void CardputerView::displayMnemonicWord(std::string word, size_t index, size_t size, bool esc, bool restore) {
    // Clear
    displayClearMainView(5);

    // Box frame
    Display->drawRoundRect(10, 35, Display->width() - 20, 90, DEFAULT_ROUND_RECT, PRIMARY_COLOR);

    if (esc) {
        // Esc
        Display->setTextSize(TEXT_TINY);
        Display->setTextColor(PRIMARY_COLOR);
        Display->setCursor(32, 47);
        Display->printf("Press ESC when you are done");
    }

    if (restore) {
        Display->setTextSize(TEXT_SMALL);
        Display->setTextColor(PRIMARY_COLOR);
        Display->setCursor(24, 49);
        Display->printf("Write the word and press OK"); 
    }


    // Word box
    Display->drawRoundRect(40, 65, Display->width() - 77, 35, DEFAULT_ROUND_RECT, RECT_COLOR_LIGHT);
    
    // Word
    Display->setTextSize(TEXT_BIG);
    Display->setTextColor(TEXT_COLOR);
    auto offsetX= getTextCenterOffset(word, Display->width(), 8);
    Display->setCursor(offsetX, 80);
    Display->printf(word.c_str());

    if (!restore) {
        // < >
        Display->setCursor(20, 80);
        Display->printf("<");
        Display->setCursor(Display->width() - 30, 80);
        Display->printf(">");
    }

    // Counter
    Display->setTextColor(PRIMARY_COLOR);
    Display->setTextSize(TEXT_MEDIUM);
    Display->setCursor(Display->width() / 2 - 15, Display->height() - 23);
    Display->printf("%d/%d", index+1, size);
    Display->setTextColor(TEXT_COLOR);
}

void CardputerView::drawSearchIcon(int x, int y, int size, uint16_t color) {
    int radius = size / 2;
    int handleLength = size / 2;

    // Dessiner cercle
    Display->drawCircle(x, y, radius, color);
    // Dessiner poignée
    Display->drawLine(x + radius, y + radius, x + radius + handleLength, y + radius + handleLength, color);
}

void CardputerView::drawRect(bool selected, uint8_t margin, uint16_t startY, uint16_t sizeX, uint16_t sizeY, uint16_t stepY) {
        // Draw rect
        if (selected) {
            Display->fillRoundRect(margin, startY + stepY, sizeX, sizeY, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
            Display->setTextColor(TEXT_COLOR);
        } else {
            Display->fillRoundRect(margin, startY + stepY , sizeX, sizeY, DEFAULT_ROUND_RECT, RECT_COLOR_DARK);
            Display->drawRoundRect(margin, startY + stepY, sizeX, sizeY, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
            Display->setTextColor(TEXT_COLOR);
        }
}

void CardputerView::drawSubMenuReturn(uint8_t x, uint8_t y) {
    Display->setTextSize(TEXT_WIDE);
    Display->setTextColor(PRIMARY_COLOR);
    Display->setCursor(x, y);
    Display->printf("<");
}

void CardputerView::displayClearMainView(uint8_t offsetY) {
    Display->fillRect(0, TOP_BAR_HEIGHT-offsetY, Display->width(), Display->height(), BACKGROUND_COLOR);
}

void CardputerView::clearTopBar() {
    Display->fillRect(0, 0, Display->width(), TOP_BAR_HEIGHT, BACKGROUND_COLOR);
}

void CardputerView::drawBitcoinIcon(int x, int y) {
    int radius = 22 / 2;

    // Dessiner le cercle
    Display->fillCircle(x + radius, y + radius, radius, PRIMARY_COLOR);
    Display->drawCircle(x + radius, y + radius, radius, BACKGROUND_COLOR);

    // Dessiner le "B"
    int innerRadius = radius / 2;
    Display->setTextColor(TEXT_COLOR);
    Display->setTextSize(TEXT_TINY);
    Display->setTextDatum(middle_center);
    M5Cardputer.Display.setFont(&fonts::FreeSerifBold24pt7b);
    Display->drawString("B", x + radius, y + radius + 1);
    M5Cardputer.Display.setFont(&fonts::Orbitron_Light_24);
}

void CardputerView::displayDebug(std::string message) {
    Display->setTextSize(TEXT_MEDIUM);
    Display->fillScreen(TFT_BLACK);
    Display->setCursor(100, 10);
    Display->printf("DEBUG");
    Display->setCursor(10, 50);
    Display->printf(message.c_str());
    delay(3000);
}

float CardputerView::getTextCenterOffset(const std::string& text, int16_t width, float sizeText) {
    float correction = 0.0f;

    for (char c : text) {

        // Correction largeur lettre
        char lowerC = std::tolower(c);
        if (lowerC == 'w' || lowerC == 'm') {
            correction -= sizeText / 2;
        } else if (lowerC == 'i') {
            correction += sizeText / 2;
        } else if (c == 'l' || c == 'f' || c == 't' || c == 'j') {
            correction += sizeText / 4;
        }

        // Correction pour majuscules
        if (std::isupper(c)) {
            correction -= sizeText / 3;
        }
    }

    // offset pour centrer avec correction 
    float baseOffset = width / 2 - sizeText * text.length();
    return baseOffset + correction;
}

void CardputerView::displayKeyboardLayout(const std::string& layoutName) {
    // Clear
    displayClearMainView(5);

    // Box frame
    Display->drawRoundRect(10, 35, Display->width() - 20, 90, DEFAULT_ROUND_RECT, PRIMARY_COLOR);

    // Infos
    Display->setTextSize(TEXT_TINY);
    Display->setTextColor(PRIMARY_COLOR);
    Display->setCursor(17, 45);
    Display->printf("Press OK to select Keyboard Layout");
    Display->setCursor(17, 113);
    Display->printf("to send the btc address through USB");

    // Layout name box
    Display->drawRoundRect(40, 63, Display->width() - 77, 35, DEFAULT_ROUND_RECT, RECT_COLOR_LIGHT);
    
    // Layout name
    Display->setTextSize(TEXT_WIDE);
    Display->setTextColor(TEXT_COLOR);
    auto offsetX = getTextCenterOffset(layoutName, Display->width(), 4.5);
    Display->setCursor(offsetX, 80);
    Display->printf(layoutName.c_str());

    // Arrows for navigation
    Display->setCursor(20, 78);
    Display->printf("<");   
    Display->setCursor(Display->width() - 26, 78);
    Display->printf(">");

    // Reset text color to default
    Display->setTextColor(PRIMARY_COLOR);
    Display->setTextSize(TEXT_MEDIUM);
}

void CardputerView::displayWalletValue(std::string description, std::string value) {
    // Clear the main view area
    displayClearMainView(5);
    
    auto limit = 110;
    if (description == "Balance") {
        Display->setCursor(0, 39);
        Display->setTextSize(0.97);
        limit = 85;
    } else if (description == "Address") {
        Display->setCursor(0, 48);
        Display->setTextSize(1.2);
    } else { // Pubkey
        Display->setCursor(0, 43);
        Display->setTextSize(0.9);
        limit = 80;
    }

    // Limit value characters with "..."
    std::string truncatedValue = value;
    if (value.length() > limit) {
        truncatedValue = value.substr(0, limit) + "...";
    }

    // Display value
    Display->setTextColor(TEXT_COLOR);
    Display->setFont(&fonts::FreeSans9pt7b);
    Display->printf(truncatedValue.c_str());
    Display->setFont(&fonts::Orbitron_Light_24);

    // Display "Q" button
    Display->setTextSize(TEXT_MEDIUM);
    Display->fillRoundRect(40, 95, 20, 15, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    Display->setTextColor(TEXT_COLOR);
    Display->setCursor(45, 101);
    Display->printf("q");

    Display->setCursor(68, 102);
    Display->setTextColor(PRIMARY_COLOR);
    Display->printf("to show QR Code");

    // Display "OK" button
    Display->fillRoundRect(40, 117, 30, 15, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    Display->setTextColor(TEXT_COLOR);
    Display->setCursor(47, 124);
    Display->printf("ok");

    // Display->setTextSize(TEXT_TINY);
    Display->setCursor(80, 123);
    Display->setTextColor(PRIMARY_COLOR);
    Display->printf("to send via USB");

    Display->setTextColor(TEXT_COLOR);
}

void CardputerView::displayPlugUsbMention() {
    Display->setTextSize(TEXT_MEDIUM);
    Display->setTextColor(PRIMARY_COLOR);
    Display->setCursor(30, 120);
    Display->printf("Plug in USB as keyboard");
}


void CardputerView::displayQrCode(std::string address) {
    Display->fillScreen(BACKGROUND_COLOR);
    M5Cardputer.Display.qrcode(address.c_str(), -1, -1, 125);
}

void CardputerView::setBrightness(uint16_t brightness) {
    Display->setBrightness(brightness);
}

std::string CardputerView::toUpperCase(const std::string& text) {
    std::string uppercaseText = text;
    std::transform(uppercaseText.begin(), uppercaseText.end(), uppercaseText.begin(), ::toupper);
    return uppercaseText;
}

void CardputerView::displayTopIcon() {
    drawBitcoinIcon(207, 7);
    drawSubMenuReturn(10, 15);
}

void CardputerView::displaySeedStart(){
    Display->fillScreen(BACKGROUND_COLOR);

    // Box frame
    Display->drawRect(1, 1, Display->width() - 1, Display->height() - 1, PRIMARY_COLOR);

    // Main title
    Display->setTextSize(TEXT_BIG);
    Display->setCursor(40, 22);
    Display->setTextColor(PRIMARY_COLOR);
    Display->printf("About Seed");

    // Sub title
    Display->setTextSize(TEXT_SMALL);
    Display->setTextColor(TEXT_COLOR);
    Display->setCursor(30, 46);
    Display->printf("We will create a new seed");

    // Text
    Display->setCursor(20, 65);
    Display->printf("Note the 24 words on a paper");
    Display->setTextColor(PRIMARY_COLOR);
    Display->setCursor(12, 88);
    Display->setTextSize(TEXT_WIDE);
    Display->printf("Lost Seed = Lost Wallet");

    // Button OK
    Display->fillRoundRect(70, 105, 100, 20, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    Display->setTextColor(TEXT_COLOR);
    Display->setTextSize(TEXT_MEDIUM);
    Display->setCursor(80, 115);
    Display->printf("OK to start");
}

void CardputerView::displaySeedRfid(){
    Display->fillScreen(BACKGROUND_COLOR);

    // Box frame
    Display->drawRect(1, 1, Display->width() - 1, Display->height() - 1, PRIMARY_COLOR);

    // Main title
    Display->setTextSize(TEXT_BIG);
    Display->setCursor(45, 22);
    Display->setTextColor(PRIMARY_COLOR);
    Display->printf("About RFID");

    // Sub title
    Display->setTextSize(TEXT_SMALL);
    Display->setTextColor(TEXT_COLOR);
    Display->setCursor(12, 46);
    Display->printf("You can store your seed on a tag");

    // Text
    Display->setCursor(20, 65);
    Display->printf("Plug your RFID2 if you want it");
    Display->setTextColor(PRIMARY_COLOR);
    Display->setCursor(8, 88);
    Display->setTextSize(TEXT_SMALL);
    Display->printf("You can encrypt it with password");

    // Button Next
    Display->fillRoundRect(80, 105, 80, 20, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    Display->setTextColor(TEXT_COLOR);
    Display->setTextSize(TEXT_MEDIUM);
    Display->setCursor(90, 115);
    Display->printf("Next  ->");
}

void CardputerView::displaySeedEnd(bool sdCardMount) {
    Display->fillScreen(BACKGROUND_COLOR);

    // Box frame
    Display->drawRect(1, 1, Display->width() - 1, Display->height() - 1, PRIMARY_COLOR);

    // Main title
    Display->setTextSize(TEXT_BIG);
    Display->setCursor(30, 22);
    Display->setTextColor(PRIMARY_COLOR);
    Display->printf("About Wallet");

    // Sub title
    Display->setTextSize(TEXT_SMALL);
    Display->setTextColor(TEXT_COLOR);
    Display->setCursor(38, 46);
    Display->printf("Wallet has been created");

    // Text
    Display->setCursor(12, 65);
    Display->printf(sdCardMount ? "It has been saved to the SD card" : "  It could not be saved to the SD");
    Display->setTextColor(PRIMARY_COLOR);
    Display->setCursor(28, 88);
    Display->setTextSize(TEXT_MEDIUM_LARGE);
    Display->printf(sdCardMount ? "bitcoin-card-wallets.txt" : " It will be lost on reboot");

    // Button OK
    Display->fillRoundRect(70, 105, 100, 20, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    Display->setTextColor(TEXT_COLOR);
    Display->setTextSize(TEXT_MEDIUM);
    Display->setCursor(80, 115);
    Display->printf("OK to start");
}

void CardputerView::displayPlugRfid(){
    Display->fillScreen(BACKGROUND_COLOR);

    // Box frame
    Display->drawRect(1, 1, Display->width() - 1, Display->height() - 1, PRIMARY_COLOR);

    // Main title
    Display->setTextSize(TEXT_BIG);
    Display->setCursor(54, 22);
    Display->setTextColor(PRIMARY_COLOR);
    Display->printf("Plug RFID");

    // Sub title
    Display->setTextSize(TEXT_SMALL);
    Display->setTextColor(TEXT_COLOR);
    Display->setCursor(42, 46);
    Display->printf("Plug you RFID2 module");

    // Text
    Display->setCursor(45, 65);
    Display->printf("into the I2C grove port");
    Display->setTextColor(PRIMARY_COLOR);
    Display->setCursor(20, 88);
    Display->setTextSize(TEXT_SMALL);
    Display->printf("Press OK when you are ready");

    // Button Next
    Display->fillRoundRect(80, 105, 80, 20, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    Display->setTextColor(TEXT_COLOR);
    Display->setTextSize(TEXT_MEDIUM);
    Display->setCursor(90, 115);
    Display->printf("Next  ->");
}

void CardputerView::displaySeedGeneralInfos() {
    Display->fillScreen(BACKGROUND_COLOR);

    // Box frame
    Display->drawRect(1, 1, Display->width() - 1, Display->height() - 1, PRIMARY_COLOR);

    // Main title
    Display->setTextSize(TEXT_BIG);
    Display->setCursor(40, 22);
    Display->setTextColor(PRIMARY_COLOR);
    Display->printf("About Seed");

    // Sub title
    Display->setTextSize(TEXT_SMALL);
    Display->setTextColor(TEXT_COLOR);
    Display->setCursor(25, 46);
    Display->printf("Seed is not saved anywhere");

    // Text
    Display->setCursor(30, 65);
    Display->printf("If you lost it or don't save it");
    Display->setTextColor(PRIMARY_COLOR);
    Display->setCursor(13, 88);
    Display->setTextSize(TEXT_MEDIUM);
    Display->printf("You cannot access the funds");

    // Button Next
    Display->fillRoundRect(80, 105, 80, 20, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    Display->setTextColor(TEXT_COLOR);
    Display->setTextSize(TEXT_MEDIUM);
    Display->setCursor(90, 115);
    Display->printf("Next  ->");
}

void CardputerView::displaySeedLoadInfos() {
    Display->fillScreen(BACKGROUND_COLOR);

    // Box frame
    Display->drawRect(1, 1, Display->width() - 1, Display->height() - 1, PRIMARY_COLOR);

    // Main title
    Display->setTextSize(TEXT_BIG);
    Display->setCursor(46, 22);
    Display->setTextColor(PRIMARY_COLOR);
    Display->printf("Load Seed");

    // Sub title
    Display->setTextSize(TEXT_SMALL);
    Display->setTextColor(TEXT_COLOR);
    Display->setCursor(15, 46);
    Display->printf("Seed is not saved on the device");

    // Text
    Display->setCursor(18, 65);
    Display->printf("To be able to sign transactions");
    Display->setTextColor(PRIMARY_COLOR);
    Display->setCursor(24, 88);
    Display->setTextSize(TEXT_MEDIUM);
    Display->printf("Load it with RFID SD Words");

    // Button Next
    Display->fillRoundRect(80, 105, 80, 20, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    Display->setTextColor(TEXT_COLOR);
    Display->setTextSize(TEXT_MEDIUM);
    Display->setCursor(90, 115);
    Display->printf("Next  ->");
}

void CardputerView::displaySeedFormatGeneralInfos() {
    Display->fillScreen(BACKGROUND_COLOR);

    // Box frame
    Display->drawRect(1, 1, Display->width() - 1, Display->height() - 1, PRIMARY_COLOR);

    // Main title
    Display->setTextSize(TEXT_BIG);
    Display->setCursor(25, 22);
    Display->setTextColor(PRIMARY_COLOR);
    Display->printf("About Format");

    // Sub title
    Display->setTextSize(TEXT_SMALL);
    Display->setTextColor(TEXT_COLOR);
    Display->setCursor(30, 46);
    Display->printf("Seed uses BIP39 standard");

    // Text
    Display->setCursor(25, 65);
    Display->printf("You can restore it anywhere");
    Display->setTextColor(PRIMARY_COLOR);
    Display->setCursor(16, 88);
    Display->setTextSize(TEXT_MEDIUM);
    Display->printf("BTC address is segwit BIP84");

    // Button Next
    Display->fillRoundRect(80, 105, 80, 20, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    Display->setTextColor(TEXT_COLOR);
    Display->setTextSize(TEXT_MEDIUM);
    Display->setCursor(90, 115);
    Display->printf("Next  ->");
}

void CardputerView::displaySdSaveGeneralInfos() {
    Display->fillScreen(BACKGROUND_COLOR);

    // Box frame
    Display->drawRect(1, 1, Display->width() - 1, Display->height() - 1, PRIMARY_COLOR);

    // Main title
    Display->setTextSize(TEXT_BIG);
    Display->setCursor(20, 22);
    Display->setTextColor(PRIMARY_COLOR);
    Display->printf("About SD card");

    // Sub title
    Display->setTextSize(TEXT_SMALL);
    Display->setTextColor(TEXT_COLOR);
    Display->setCursor(28, 46);
    Display->printf("Only public infos are stored");

    // Text
    Display->setCursor(22, 65);
    Display->printf("You can manually edit the file");
    Display->setTextColor(PRIMARY_COLOR);
    Display->setCursor(26, 88);
    Display->setTextSize(TEXT_MEDIUM_LARGE);
    Display->printf("SEEDS ARE NOT SAVED");

    // Button Next
    Display->fillRoundRect(80, 105, 80, 20, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    Display->setTextColor(TEXT_COLOR);
    Display->setTextSize(TEXT_MEDIUM);
    Display->setCursor(90, 115);
    Display->printf("Next  ->");
}

void CardputerView::displaySeedRestorationInfos() {
    Display->fillScreen(BACKGROUND_COLOR);

    // Box frame
    Display->drawRect(1, 1, Display->width() - 1, Display->height() - 1, PRIMARY_COLOR);

    // Main title
    Display->setTextSize(TEXT_BIG);
    Display->setCursor(40, 22);
    Display->setTextColor(PRIMARY_COLOR);
    Display->printf("About Seed");

    // Sub title
    Display->setTextSize(TEXT_SMALL);
    Display->setTextColor(TEXT_COLOR);
    Display->setCursor(22, 46);
    Display->printf("Write each word of your seed");

    // Text
    Display->setCursor(27, 65);
    Display->printf("Input your words accurately");
    Display->setTextColor(PRIMARY_COLOR);
    Display->setCursor(28, 88);
    Display->setTextSize(TEXT_MEDIUM);
    Display->printf("It will recover your wallet");

    // Button Next
    Display->fillRoundRect(80, 105, 80, 20, DEFAULT_ROUND_RECT, PRIMARY_COLOR);
    Display->setTextColor(TEXT_COLOR);
    Display->setTextSize(TEXT_MEDIUM);
    Display->setCursor(90, 115);
    Display->printf("Next  ->");
}

}
