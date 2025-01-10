#include "RfidService.h"

namespace services {

bool RfidService::initialize() {
    auto sda = globalContext.getSdaPin();
    auto scl = globalContext.getSclPin();
    auto rfidAddress = globalContext.getRfidAddress();

    mfrc522 = MFRC522(rfidAddress);
    Wire.begin(sda, scl);
    if (!isModuleConnected(rfidAddress)) {
        return false;
    }

    mfrc522.PCD_Init();
    return true;
}

bool RfidService::isCardPresent() {
    return mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial();
}

std::string RfidService::getCardUID() {
    std::ostringstream uidStream;
    uidStream << std::hex << std::setfill('0');
    for (uint8_t i = 0; i < mfrc522.uid.size; ++i) {
        uidStream << std::setw(2) << static_cast<int>(mfrc522.uid.uidByte[i]);
    }

    return uidStream.str();
}

bool RfidService::isModuleConnected(uint8_t address) {
    Wire.beginTransmission(address);
    return (Wire.endTransmission() == 0); // mean it responds
}

bool RfidService::authenticateBlock(uint8_t blockAddr) {
    MFRC522::MIFARE_Key key;
    for (uint8_t i = 0; i < MFRC522::MF_KEY_SIZE; ++i) {
        key.keyByte[i] = 0xFF; // Default key: FFFFFFFFFFFF
    }

    auto status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &key, &(mfrc522.uid));
    return status == MFRC522::STATUS_OK;
}

std::vector<uint8_t> RfidService::readBlock(uint8_t blockAddr) {
    std::vector<uint8_t> buffer(18); // 16 bytes for data + 2 bytes for CRC
    uint8_t size = buffer.size();

    if (mfrc522.MIFARE_Read(blockAddr, buffer.data(), &size) != MFRC522::STATUS_OK) {
        return {};
    }

    buffer.resize(16); // Keep only the 16 bytes of data
    return buffer;
}

bool RfidService::writeBlock(uint8_t blockAddr, const std::vector<uint8_t>& data) {
    if (data.size() != 16) {
        return false;
    }

    auto status = mfrc522.MIFARE_Write(blockAddr, const_cast<byte*>(data.data()), 16);
    return status == MFRC522::STATUS_OK;
}

bool RfidService::verifyBlock(uint8_t blockAddr, const std::vector<uint8_t>& expectedData) {
    if (expectedData.size() != 16) {
        return false;
    }

    // Read bloc
    auto readData = readBlock(blockAddr);

    // Compare
    return readData == expectedData;
}

bool RfidService::savePrivateKey(const std::vector<uint8_t>& data1, const std::vector<uint8_t>& data2) {
    if (data1.size() != 16) {
        return false;
    }

    if (!authenticateBlock(blockPrivateKey1)) {
        return false;
    }

    if (!writeBlock(blockPrivateKey1, data1) || !verifyBlock(blockPrivateKey1, data1)) {
        return false;
    }

    if (data2.empty()) {
        return true; // seed 16 bytes
    }

    if (data2.size() != 16) {
        return false;
    }

    if (!authenticateBlock(blockPrivateKey2)) {
        return false;
    }

    if (!writeBlock(blockPrivateKey2, data2) || !verifyBlock(blockPrivateKey2, data2)) {
        return false;
    }

    return true;
}

bool RfidService::saveSalt(const std::string& salt) {
    std::vector<uint8_t> saltData;
    if (salt.empty()) {
        saltData = std::vector<uint8_t>(16, 0); // all zeros act like 'no encryption on the seed'
    } else {
        saltData = std::vector<uint8_t>(salt.begin(), salt.end());
    }

    return authenticateBlock(blockSalt) && 
           writeBlock(blockSalt, saltData) && 
           verifyBlock(blockSalt, saltData);
}

bool RfidService::saveMetadata(uint8_t seedLength) {
    if (!authenticateBlock(blockMetadata)) {
        return false;
    }

    std::vector<uint8_t> metadata(16, 0);
    metadata[0] = seedLength;

    return writeBlock(blockMetadata, metadata) && verifyBlock(blockMetadata, metadata);
}

std::vector<uint8_t> RfidService::getPrivateKey32() {
    // Auth des blocs contenant la clé
    if (!authenticateBlock(blockPrivateKey1) || !authenticateBlock(blockPrivateKey2)) {
        return {};
    }

    // Lecture 16 bytes blocs
    auto part1 = readBlock(blockPrivateKey1);
    auto part2 = readBlock(blockPrivateKey2);

    // Combine key
    std::vector<uint8_t> encryptedKey;
    encryptedKey.reserve(32);
    encryptedKey.insert(encryptedKey.end(), part1.begin(), part1.end());
    encryptedKey.insert(encryptedKey.end(), part2.begin(), part2.end());

    return encryptedKey;
}

std::vector<uint8_t> RfidService::getPrivateKey16() {
    if (!authenticateBlock(blockPrivateKey1)) {
        return {};
    }

    // Lecture du bloc 16 bytes
    auto part1 = readBlock(blockPrivateKey1);

    return part1;
}

std::vector<uint8_t> RfidService::getPrivateKey() {
    if (!authenticateBlock(blockMetadata)) {
        return {};
    }

    auto metadata = readBlock(blockMetadata);
    if (metadata.empty()) {
        return {};
    }

    // 0x10 16 bytes, 0x20 pour 32 bytes
    uint8_t keyType = metadata[0];

    if (!authenticateBlock(blockPrivateKey1)) {
        return {};
    }

    // Seed first 16 bytes
    auto part1 = readBlock(blockPrivateKey1);

    // 16 bytes seed
    if (keyType == 0x10) {
        return part1;
    }

    // 32 bytes seed
    if (keyType == 0x20) {
        if (!authenticateBlock(blockPrivateKey2)) {
            return {};
        }

        auto part2 = readBlock(blockPrivateKey2);
        if (part1.size() != 16 || part2.size() != 16) {
            return {};
        }

        std::vector<uint8_t> encryptedKey;
        encryptedKey.reserve(32);
        encryptedKey.insert(encryptedKey.end(), part1.begin(), part1.end());
        encryptedKey.insert(encryptedKey.end(), part2.begin(), part2.end());

        return encryptedKey;
    }

    return {};
}

std::string RfidService::getSalt() {
    // Auth du bloc contenant le salt
    if (!authenticateBlock(blockSalt)) {
        return "";
    }

    // Lecture du bloc 6
    auto saltData = readBlock(blockSalt);

    // Conversion du vecteur de bytes en string
    std::string salt(saltData.begin(), saltData.end());
    salt.erase(std::find(salt.begin(), salt.end(), '\0'), salt.end());

    return salt;
}

bool RfidService::saveChecksum(const std::vector<uint8_t>& signature) {
    if (signature.size() != 16) {
        return false;
    }

    return authenticateBlock(blockSign) && 
           writeBlock(blockSign, signature) &&
           verifyBlock(blockSign, signature);
}

std::vector<uint8_t> RfidService::getCheckSum() {
    if (!authenticateBlock(blockSign)) {
        return {};
    }

    return readBlock(blockSign);
}

uint8_t RfidService::getMetadata() {
    if (!authenticateBlock(blockMetadata)) {
        return 0;
    }

    auto metadata = readBlock(blockMetadata);
    return metadata[0]; // seed length
}

bool RfidService::lockSectorAsReadOnly(uint8_t sector) {
    // Calculer l'adresse du sector trailer
    uint8_t sectorTrailer = sector * 4 + 3;

    if (!authenticateBlock(sectorTrailer)) {
        return false;
    }

    // Définir les nouvelles clés
    MFRC522::MIFARE_Key keyA = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
    MFRC522::MIFARE_Key keyB = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

    // Définir les bits d'acces pour lecture seule
    byte accessBits[3] = {0b11111100, 0b00001111, 0b00000000};

    // Construire le nouveau contenu du secteur
    byte dataBlock[16];
    memcpy(dataBlock, keyA.keyByte, 6); // Clé A
    memcpy(dataBlock + 6, accessBits, 3); // acces
    dataBlock[9] = 0x00; // Byte utilisateur (optionnel)
    memcpy(dataBlock + 10, keyB.keyByte, 6); // Clé B

    // Ecrire le secteur trailer
    if (!writeBlock(sectorTrailer, std::vector<uint8_t>(dataBlock, dataBlock + 16))) {
        return false;
    }

    // Verif secteur
    if (!verifyBlock(sectorTrailer, std::vector<uint8_t>(dataBlock, dataBlock + 16))) {
        return false;
    }

    return true;
}

void RfidService::reset() {
    end();
    mfrc522.PCD_Reset();
    mfrc522.PCD_Init();
    delay(100);
}

void RfidService::end() {
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}

} // namespace services
