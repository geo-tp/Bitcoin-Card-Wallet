#ifndef CARDPUTER_VIEW_H
#define CARDPUTER_VIEW_H

#include <vector>
#include <string>
#include <cstring>
#include <M5Cardputer.h>

// SIZING
#define DEFAULT_MARGIN 5
#define DEFAULT_ROUND_RECT 5
#define TOP_BAR_HEIGHT 30

// PALETTE
#define BACKGROUND_COLOR TFT_BLACK
#define PRIMARY_COLOR 0xfc20
#define RECT_COLOR_DARK 0x0841
#define RECT_COLOR_LIGHT 0xd69a
#define TEXT_COLOR 0xef7d

// TEXT SIZE
#define TEXT_BIG 1
#define TEXT_LARGE 0.95
#define TEXT_WIDE 0.7
#define TEXT_MEDIUM_LARGE 0.6
#define TEXT_MEDIUM 0.55
#define TEXT_SMALL 0.5
#define TEXT_TINY 0.45

namespace views {

class CardputerView {
public:
    static void initialize();
    static void setBrightness(uint16_t brightness);
    static void displayTopBar(const std::string& title, bool submenu=false, bool searchBar=false, bool bitcoinIcon=false, size_t correctionOffset=0);
    static void displaySelection(const std::vector<std::string>& selectionStrings,  uint16_t selectionIndex, const std::vector<std::string>& selectionDescription={}, bool upperCase=false, bool showCurrency=false);
    static void displayStringPrompt(std::string stringDescription, std::string stringInput, size_t offsetX=0, bool backButton=true);
    static void displayConfirmationPrompt(std::string stringDescription);
    static void displaySubMessage(std::string message, size_t x = 10, int delayMs=0);
    static void displayMnemonicWord(std::string word, size_t index, size_t size=24, bool esc=true, bool restore=false);
    static void displayWalletFileInfo(std::string defaultFileName);
    static void displayClearMainView(uint8_t offsetY=0);
    static void displayDebug(std::string message);
    static void displayQrCode(std::string address);
    static void displayWalletValue(std::string description, std::string value);
    static void displayTopIcon();
    static void displayKeyboardLayout(const std::string& layoutName);
    static void displayPlugUsbMention();
    static void displaySeedStart();
    static void displaySeedRfid();
    static void displaySeedEnd(bool sdCardMount);
    static void displaySeedGeneralInfos();
    static void displaySdSaveGeneralInfos();
    static void displaySeedFormatGeneralInfos();
    static void displaySeedLoadInfos();
    static void displaySeedRestorationInfos();
    static void displayRfidTagInfos();
    static void displayRfidInfos();
    static void displayPlugRfid();
    static void drawBitcoinIcon(int x, int y);

private:
    static M5GFX* Display;
    static void drawSubMenuReturn(uint8_t x, uint8_t y);
    static void drawSearchIcon(int x, int y, int size, uint16_t color);
    static void drawRect(bool selected, uint8_t margin, uint16_t startY, uint16_t sizeX, uint16_t sizeY, uint16_t stepY);
    static void clearTopBar();
    static std::string toUpperCase(const std::string& text);
    static float getTextCenterOffset(const std::string& text, int16_t width, float sizeText);
    static std::string truncateString(const std::string& input, size_t maxLength);
};

}

#endif
