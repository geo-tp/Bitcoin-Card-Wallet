#ifndef GLOBAL_CONTEXT_H
#define GLOBAL_CONTEXT_H

#include <string>

namespace contexts {

class GlobalContext {
public:
    // Singleton instance accessor
    static GlobalContext& getInstance();

    // Getters and setters
    std::string getAppName() const;
    void setAppName(const std::string& appName);

    std::string getfileWalletDefaultPath() const;
    void setfileWalletDefaultPath(const std::string& fileWalletDefaultPath);

    std::string getFileWalletPath() const;
    void setFileWalletPath(const std::string& fileWalletPath);

    std::string getBitcoinBalanceUrl() const;
    void setBitcoinBalanceUrl(const std::string& BitcoinBalanceUrl);

    int getMaxInputCharCount() const;
    void setMaxInputCharCount(int maxInputCharCount);

    int getLedPin() const;
    void setLedPin(int ledPin);

    int getSdCardCSPin() const;
    void setSdCardCSPin(int sdCardCSPin);

    int getSdCardMISOPin() const;
    void setSdCardMISOPin(int sdCardMISOPin);

    int getSdCardMOSIPin() const;
    void setSdCardMOSIPin(int sdCardMOSIPin);

    int getSdCardCLKPin() const;
    void setSdCardCLKPin(int sdCardCLKPin);

    int getFileCacheLimit() const;
    void setFileCacheLimit(int fileCacheLimit);

    int getFileCountLimit() const;
    void setFileCountLimit(int fileCountLimit);

    int getSdaPin() const;
    void setSdaPin(int sdaPin);

    int getSclPin() const;
    void setSclPin(int sclPin);

    int getRfidAddress() const;
    void setRfidAddress(int rfidAddress);

    int getBlockSalt() const;
    void setBlockSalt(int blockSalt);

    int getBlockPrivateKey1() const;
    void setBlockPrivateKey1(int blockPrivateKey1);

    int getBlockPrivateKey2() const;
    void setBlockPrivateKey2(int blockPrivateKey2);

    int getBlockSign() const;
    void setBlockSign(int blockSign);

    int getBlockMetadata() const;
    void setBlockMetadata(int blockSign);

private:
    // Private constructor to prevent instantiation
    GlobalContext();

    // GENERAL
    std::string appName = "Card Wallet";
    std::string fileWalletPath;
    std::string fileWalletDefaultPath = "/bitcoin-card-wallets.txt";
    std::string bitcoinBalanceUrl = "https://www.blockonomics.co/#/search?q=";
    int maxInputCharCount = 14;

    // LED
    int ledPin = 21;

    // SD
    int sdCardCSPin = 12;
    int sdCardMISOPin = 39;
    int sdCardMOSIPin = 14;
    int sdCardCLKPin = 40;
    int fileCacheLimit = 24;
    int fileCountLimit = 512;

    // I2C
    int sdaPin = 2;
    int sclPin = 1;

    // RFID
    int rfidAddress = 0x28;
    int blockSalt = 6;
    int blockPrivateKey1 = 4;
    int blockPrivateKey2 = 5;
    int blockSign = 8;
    int blockMetadata = 9;
};

} // namespace contexts

#endif // GLOBAL_CONTEXT_H
