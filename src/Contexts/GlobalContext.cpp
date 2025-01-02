#include "GlobalContext.h"

namespace contexts {

// Singleton instance accessor
GlobalContext& GlobalContext::getInstance() {
    static GlobalContext instance;
    return instance;
}

// Define the private default constructor
GlobalContext::GlobalContext() = default;

std::string GlobalContext::getAppName() const {
    return appName;
}

void GlobalContext::setAppName(const std::string& appName) {
    this->appName = appName;
}

std::string GlobalContext::getFileWalletPath() const {
    return this->fileWalletPath;
}

void GlobalContext::setFileWalletPath(const std::string& fileWalletPath) {
    this->fileWalletPath= fileWalletPath;
}

std::string GlobalContext::getBitcoinBalanceUrl() const {
    return this->bitcoinBalanceUrl;
}

void GlobalContext::setBitcoinBalanceUrl(const std::string& bitcoinBalanceUrl) {
    this->bitcoinBalanceUrl= bitcoinBalanceUrl;
}

std::string GlobalContext::getfileWalletDefaultPath() const {
    return fileWalletDefaultPath;
}

void GlobalContext::setfileWalletDefaultPath(const std::string& fileWalletDefaultPath) {
    this->fileWalletDefaultPath= fileWalletDefaultPath;
}

int GlobalContext::getMaxInputCharCount() const {
    return maxInputCharCount;
}

void GlobalContext::setMaxInputCharCount(int maxInputCharCount) {
    this->maxInputCharCount = maxInputCharCount;
}

int GlobalContext::getLedPin() const {
    return ledPin;
}

void GlobalContext::setLedPin(int ledPin) {
    this->ledPin = ledPin;
}

int GlobalContext::getSdCardCSPin() const {
    return sdCardCSPin;
}

void GlobalContext::setSdCardCSPin(int sdCardCSPin) {
    this->sdCardCSPin = sdCardCSPin;
}

int GlobalContext::getSdCardMISOPin() const {
    return sdCardMISOPin;
}

void GlobalContext::setSdCardMISOPin(int sdCardMISOPin) {
    this->sdCardMISOPin = sdCardMISOPin;
}

int GlobalContext::getSdCardMOSIPin() const {
    return sdCardMOSIPin;
}

void GlobalContext::setSdCardMOSIPin(int sdCardMOSIPin) {
    this->sdCardMOSIPin = sdCardMOSIPin;
}

int GlobalContext::getSdCardCLKPin() const {
    return sdCardCLKPin;
}

void GlobalContext::setSdCardCLKPin(int sdCardCLKPin) {
    this->sdCardCLKPin = sdCardCLKPin;
}

int GlobalContext::getFileCacheLimit() const {
    return fileCacheLimit;
}

void GlobalContext::setFileCacheLimit(int fileCacheLimit) {
    this->fileCacheLimit = fileCacheLimit;
}

int GlobalContext::getFileCountLimit() const {
    return fileCountLimit;
}

void GlobalContext::setFileCountLimit(int fileCountLimit) {
    this->fileCountLimit = fileCountLimit;
}

} // namespace contexts
