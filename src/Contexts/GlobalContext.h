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
private:
    // Private constructor to prevent instantiation
    GlobalContext();

    std::string appName = "Card Wallet";
    std::string fileWalletPath;
    std::string fileWalletDefaultPath = "/bitcoin-card-wallets.txt";
    std::string bitcoinBalanceUrl = "https://www.blockonomics.co/#/search?q=";
    int maxInputCharCount = 14;
    int ledPin = 21;
    int sdCardCSPin = 12;
    int sdCardMISOPin = 39;
    int sdCardMOSIPin = 14;
    int sdCardCLKPin = 40;
    int fileCacheLimit = 24;
    int fileCountLimit = 512;
};

} // namespace contexts

#endif // GLOBAL_CONTEXT_H
