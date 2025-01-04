#include "RfidService.h"

namespace services {

RfidService::RfidService()
    : mfrc522(0x28) {}

void RfidService::initialize() {
    Wire.begin(2, 1);
    mfrc522.PCD_Init();
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
        throw std::runtime_error("Failed to read block.");
    }

    buffer.resize(16); // Keep only the 16 bytes of data
    return buffer;
}

bool RfidService::writeBlock(uint8_t blockAddr, const std::vector<uint8_t>& data) {
    if (data.size() != 16) {
        throw std::invalid_argument("Data block must be exactly 16 bytes.");
    }

    auto status = mfrc522.MIFARE_Write(blockAddr, const_cast<byte*>(data.data()), 16);
    return status == MFRC522::STATUS_OK;
}

bool RfidService::verifyBlock(uint8_t blockAddr, const std::vector<uint8_t>& expectedData) {
    if (expectedData.size() != 16) {
        throw std::invalid_argument("Expected data must be exactly 16 bytes.");
    }

    // Read bloc
    auto readData = readBlock(blockAddr);

    // Compare
    return readData == expectedData;
}

bool RfidService::savePrivateKey(const std::vector<uint8_t>& data1, const std::vector<uint8_t>& data2) {
    if (data1.size() != 16 || data2.size() != 16) {
        throw std::invalid_argument("Both data blocks must be exactly 16 bytes.");
    }

    if (!authenticateBlock(4) || !authenticateBlock(5)) {
        return false;
    }

    if (!writeBlock(4, data1) || !writeBlock(5, data2)) {
        return false;
    }

    // if (!verifyBlock(4, data1) || !verifyBlock(5, data2)) {
    //     return false;
    // }

    return true; // success
}

void RfidService::end() {
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}

bool RfidService::saveSalt(const std::string& salt) {
    std::vector<uint8_t> saltData;
    if (salt.empty()) {
        saltData = std::vector<uint8_t>(16, 0); // Act like 'no encryption on the seed'
    } else {
        saltData = std::vector<uint8_t>(salt.begin(), salt.end());
    }

    return authenticateBlock(6) && 
           writeBlock(6, saltData) && 
           verifyBlock(6, saltData);
}

std::vector<uint8_t> RfidService::getPrivateKey() {
    // Authentification des blocs contenant la clé
    if (!authenticateBlock(4) || !authenticateBlock(5)) {
        throw std::runtime_error("Failed to authenticate blocks for encrypted key.");
    }

    // Lecture des blocs 4 et 5
    auto part1 = readBlock(4);
    auto part2 = readBlock(5);

    // Combinaison des deux parties de la clé
    std::vector<uint8_t> encryptedKey;
    encryptedKey.reserve(32);
    encryptedKey.insert(encryptedKey.end(), part1.begin(), part1.end());
    encryptedKey.insert(encryptedKey.end(), part2.begin(), part2.end());

    return encryptedKey;
}

std::string RfidService::getSalt() {
    // Authentification du bloc contenant le salt
    if (!authenticateBlock(6)) {
        throw std::runtime_error("Failed to authenticate block for salt.");
    }

    // Lecture du bloc 6
    auto saltData = readBlock(6);

    // Conversion du vecteur de bytes en string
    std::string salt(saltData.begin(), saltData.end());
    salt.erase(std::find(salt.begin(), salt.end(), '\0'), salt.end());

    return salt;
}

bool RfidService::saveSignature(const std::vector<uint8_t>& signature) {
    if (signature.size() != 16) {
        throw std::invalid_argument("Signature block must be 16 bytes.");
    }

    return authenticateBlock(8) && 
           writeBlock(8, signature) &&
           verifyBlock(8, signature);
}

std::vector<uint8_t> RfidService::getSignature() {
    if (!authenticateBlock(8)) {
        throw std::runtime_error("Failed to authenticate block for signature.");
    }

    return readBlock(8);
}

bool RfidService::lockSectorAsReadOnly(uint8_t sector) {
    // Calculer l'adresse du sector trailer
    uint8_t sectorTrailer = sector * 4 + 3;

    if (!authenticateBlock(sectorTrailer)) {
        return false;
    }

    // Définir les nouvelles clés ici
    MFRC522::MIFARE_Key keyA = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
    MFRC522::MIFARE_Key keyB = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

    // Définir les bits d'acces pour lecture seule
    byte accessBits[3] = {0b11111100, 0b00001111, 0b00000000};

    // Construire le nouveau contenu du secteur trailer
    byte dataBlock[16];
    memcpy(dataBlock, keyA.keyByte, 6); // Clé A
    memcpy(dataBlock + 6, accessBits, 3); // Bits d'accès
    dataBlock[9] = 0x00; // Byte utilisateur (optionnel)
    memcpy(dataBlock + 10, keyB.keyByte, 6); // Clé B

    // Ecrire le secteur trailer
    if (!writeBlock(sectorTrailer, std::vector<uint8_t>(dataBlock, dataBlock + 16))) {
        return false;
    }

    // Vérification de l'écriture
    if (!verifyBlock(sectorTrailer, std::vector<uint8_t>(dataBlock, dataBlock + 16))) {
        return false;
    }

    return true;
}

} // namespace services
