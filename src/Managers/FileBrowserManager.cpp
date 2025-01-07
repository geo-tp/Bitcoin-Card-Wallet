#include "FileBrowserManager.h"

namespace managers {

FileBrowserManager::FileBrowserManager(const GlobalManager& gm)
    : GlobalManager(gm)  // Call the base class (GlobalManager) copy constructor
{} 

bool FileBrowserManager::loadFile(std::string currentPath, FileTypeEnum selectedFileType) {
    switch (selectedFileType) {
        case FileTypeEnum::WALLET:
            return manageWalletFile(currentPath);
        case FileTypeEnum::SEED:
            if (selectionContext.getTransactionOngoing()) {
                return manageSeedLoadingFile(currentPath);
            } else {
                return manageSeedRestorationFile(currentPath);
            }
        case FileTypeEnum::TRANSACTION:
            return manageTransactionFile(currentPath);
    }
    return false;
}

bool FileBrowserManager::verifyWalletFile(const std::string& fileContent) {
    return fileContent.find("Filetype: Card Wallet") != std::string::npos &&
           fileContent.find("Version:") != std::string::npos;
}

bool FileBrowserManager::verifySeedFile(const std::string& fileContent) {
    // Split fileContent into tokens by whitespace
    std::istringstream iss(fileContent);
    std::vector<std::string> words {
        std::istream_iterator<std::string>(iss),
        std::istream_iterator<std::string>()
    };

    // Verify the total number of words is either 12 or 24
    return (words.size() == 12 || words.size() == 24);
}

bool FileBrowserManager::manageWalletFile(const std::string& currentPath) {
    auto fileName = extractFilename(currentPath);
    auto fileExt = extractFileExtension(fileName);

    if (fileExt == "txt") {
        auto fileContent = sdService.readFile(currentPath.c_str());
        if (verifyWalletFile(fileContent)) {
            walletService.loadAllWallets(fileContent);
            selectionContext.setCurrentSelectedMode(SelectionModeEnum::PORTFOLIO);
            selectionContext.setIsWalletSelected(false);
            globalContext.setFileWalletPath(currentPath);
            display.displaySubMessage("Wallets loaded", 50, 1000);
            return true;
        } else {
            confirmationSelection.select("  Invalid wallets");
        }
    } else {
        confirmationSelection.select("Unsupported file");
    }
    return false;
}

bool FileBrowserManager::manageTransactionFile(const std::string& currentPath) {
    auto fileName = extractFilename(currentPath);
    auto fileExt = extractFileExtension(fileName);

    if (fileExt == "psbt") {
        display.displayTopBar("SIGNING", false, false, true);
        display.displaySubMessage("Loading", 83);

        // Read file
        auto fileContent = sdService.readBinaryFile(currentPath.c_str());

        // Convert and get signature
        auto psbt = cryptoService.convertPSBTBinaryToBase64(fileContent);
        auto mnemonic = selectionContext.getCurrentSelectedWallet().getMnemonic();
        auto signedTransactionBytes = manageBitcoinSignature(psbt, mnemonic);
        
        // Bad sign
        if (signedTransactionBytes.empty()) {
            display.displaySubMessage("Failed to sign", 60, 2000);
            return false;
        }

        // Sign success, means it's the correct seed for the correct transaction
        display.displaySubMessage("Successefully signed", 25, 2000);
        
        // SD Save
        auto parent = getParentDirectory(currentPath);
        std::string baseFileName = fileName.substr(0, fileName.find_last_of('.')); // remove ext .psbt
        sdService.writeBinaryFile((parent + "/" + baseFileName + "-signed.psbt").c_str(), signedTransactionBytes);
        removeCachedDirectoryElement(parent); // new sign.psbt in it, remove to refetch
        display.displaySubMessage("Sign saved on SD", 40, 3000);
        
        // Check if user want to sign another tx
        auto signConfirmation = confirmationSelection.select("Sign another tx ?");
        if (!signConfirmation) {
            // Go back to portfolio
            selectionContext.setCurrentSelectedMode(SelectionModeEnum::PORTFOLIO);
            selectionContext.setCurrentSelectedFileType(FileTypeEnum::WALLET);
            selectionContext.setTransactionOngoing(false);
            return true;
        }

        return false;

    } 

    confirmationSelection.select("Unsupported file");
    return false;
}

bool FileBrowserManager::manageSeedLoadingFile(const std::string& currentPath) {
    auto fileName = extractFilename(currentPath);
    auto fileExt = extractFileExtension(fileName);
    std::string passphrase;

    if (fileExt == "txt") {
        auto fileContent = sdService.readFile(currentPath.c_str());
        if (verifySeedFile(fileContent)) {
            auto mnemonicString = fileContent;
            auto mnemonicWordList = cryptoService.mnemonicStringToWordList(fileContent);
            auto validation = cryptoService.verifyMnemonic(mnemonicWordList);

            if (!validation) {
                display.displaySubMessage("Invalid mnemonic", 41, 2000);
                return false;
            }

            display.displayTopBar("Restore Seed", false, false, true, 5);
            display.displaySubMessage("Valid mnemonic", 45, 2000);

            // Derive PublicKey and create segwit BTC address
            display.displaySubMessage("Loading", 83);
            auto publicKey = cryptoService.derivePublicKey(mnemonicString, passphrase);
            auto address = cryptoService.generateBitcoinAddress(publicKey);
            auto privateKey = cryptoService.mnemonicToPrivateKey(mnemonicString);
            display.displaySubMessage("Seed loaded", 65, 2000);

            // Get Wallet
            auto wallet = selectionContext.getCurrentSelectedWallet();
            wallet.setMnemonic(mnemonicString);
            selectionContext.setCurrentSelectedWallet(wallet);

            // Go get pbst file
            selectionContext.setCurrentSelectedMode(SelectionModeEnum::LOAD_SD);
            selectionContext.setCurrentSelectedFileType(FileTypeEnum::TRANSACTION);
            return true;
        } else {
            confirmationSelection.select("    Invalid seed");
        }
    } else {
        confirmationSelection.select("Unsupported file");
    }
    return false;
}

bool FileBrowserManager::manageSeedRestorationFile(const std::string& currentPath) {
    auto fileName = extractFilename(currentPath);
    auto fileExt = extractFileExtension(fileName);
    std::string passphrase;

    if (fileExt == "txt") {
        auto fileContent = sdService.readFile(currentPath.c_str());
        if (verifySeedFile(fileContent)) {
            auto mnemonicString = fileContent;
            auto mnemonicWordList = cryptoService.mnemonicStringToWordList(fileContent);
            auto validation = cryptoService.verifyMnemonic(mnemonicWordList);

            if (!validation) {
                display.displaySubMessage("Invalid mnemonic", 41, 2000);
                return false;
            }

            display.displayTopBar("Restore Seed", false, false, true, 5);
            display.displaySubMessage("Valid mnemonic", 45, 2000);

            auto passphrase = managePassphrase(); // return "" in case user doesn't want passphrase

            // Derive PublicKey and create segwit BTC address
            display.displaySubMessage("Loading", 83);
            auto publicKey = cryptoService.derivePublicKey(mnemonicString, passphrase);
            auto address = cryptoService.generateBitcoinAddress(publicKey);
            auto privateKey = cryptoService.mnemonicToPrivateKey(mnemonicString);
            display.displaySubMessage("Seed loaded", 65, 2000);

            // Save seed on a RFID tag
            manageRfidSave(privateKey);

            // Prompt for a wallet name
            auto walletName = stringPromptSelection.select("Enter wallet name");

            // Create Wallet
            Wallet wallet(walletName, publicKey.toString().c_str(), address, mnemonicString);

            // Save wallet to SD if any
            display.displaySubMessage("Loading", 83);
            sdService.begin(); // SD card start
            manageSdSave(wallet);
            display.displaySeedEnd(sdService.getSdState());
            input.waitPress();
            sdService.close(); // SD card stop

            // Go to portfolio
            selectionContext.setCurrentSelectedMode(SelectionModeEnum::PORTFOLIO);
            return true;
        } else {
            confirmationSelection.select("    Invalid seed");
        }
    } else {
        confirmationSelection.select("Unsupported file");
    }
    return false;
}

std::string FileBrowserManager::extractFilename(const std::string& filepath) {
    size_t lastSlash = filepath.find_last_of("/\\");
    return (lastSlash == std::string::npos) ? filepath : filepath.substr(lastSlash + 1);
}

std::string FileBrowserManager::extractFileExtension(const std::string& filename) {
    size_t lastDot = filename.find_last_of('.');
    return (lastDot == std::string::npos) ? "" : filename.substr(lastDot + 1);
}

std::string FileBrowserManager::getParentDirectory(const std::string& filePath) {
    size_t lastSlash = filePath.find_last_of("/\\");
    return (lastSlash == std::string::npos) ? "/" : filePath.substr(0, lastSlash);
}

std::vector<std::string> FileBrowserManager::getCachedDirectoryElements(const std::string& path) {
    if (cachedDirectoryElements.find(path) != cachedDirectoryElements.end()) {
        return cachedDirectoryElements[path];
    }

    std::vector<std::string> elements = sdService.listElements(path);
    if (elements.size() > 4) {
        if (cachedDirectoryElements.size() >= globalContext.getFileCacheLimit()) {
            cachedDirectoryElements.erase(cachedDirectoryElements.begin());
        }
        cachedDirectoryElements[path] = elements;
    }
    return elements;
}

void FileBrowserManager::removeCachedDirectoryElement(const std::string& path) {
    auto it = cachedDirectoryElements.find(path);
    if (it != cachedDirectoryElements.end()) {
        // Erase
        cachedDirectoryElements.erase(it);
    }
}


} // namespace managers
