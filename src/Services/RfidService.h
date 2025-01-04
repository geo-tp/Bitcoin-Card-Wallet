#ifndef RFID_SERVICE_H
#define RFID_SERVICE_H

#include <vector>
#include <string>
#include <array>
#include <Wire.h>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iomanip>
#include "mfrc522_i2c.h"
#include <Contexts/GlobalContext.h>

using namespace contexts;

namespace services {

class RfidService {
public:
    explicit RfidService();
    void initialize();
    bool isCardPresent();
    std::string getCardUID();
    bool authenticateBlock(uint8_t blockAddr);
    std::vector<uint8_t> readBlock(uint8_t blockAddr);
    bool writeBlock(uint8_t blockAddr, const std::vector<uint8_t>& data);
    bool verifyBlock(uint8_t blockAddr, const std::vector<uint8_t>& expectedData);
    bool savePrivateKey(const std::vector<uint8_t>& data1, const std::vector<uint8_t>& data2);
    std::vector<uint8_t> getPrivateKey();
    bool saveSalt(const std::string& salt);
    std::string getSalt();
    std::vector<uint8_t> getSignature();
    bool saveSignature(const std::vector<uint8_t>& signature);
    bool lockSectorAsReadOnly(uint8_t sector);
    void end();

private:
    MFRC522 mfrc522;
    GlobalContext& globalContext = GlobalContext::getInstance();
};

} // namespace services

#endif // RFID_SERVICE_H
