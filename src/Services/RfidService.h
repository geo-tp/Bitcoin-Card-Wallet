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

/* 
 MIFARE 1K TAG

- **Manufacturer Block** (Sector 0, Block 0)  
  Stores the **UID** (unique identifier) and factory data. Typically read-only.

- **Data Blocks** (e.g., Block 1 and Block 2 of Sector 0, Block 4, 5, 6 of Sector 1, etc.)  
  Store user data (16 bytes each). The *access conditions* for these blocks are defined in the sector’s trailer block.

- **Sector Trailer** (the last block in each sector, e.g., Block 3 in Sector 0, Block 7 in Sector 1, etc.)  
  Contains:
  - **Key A** (6 bytes)
  - **Access Bits** (3 bytes) + **User Byte** (1 byte)
  - **Key B** (6 bytes)
*/

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

    // Block Number
    int blockSalt         = globalContext.getBlockSalt();
    int blockPrivateKey1  = globalContext.getBlockPrivateKey1();
    int blockPrivateKey2  = globalContext.getBlockPrivateKey2();
    int blockSign         = globalContext.getBlockSign();

    // Pin and address
    int sda = globalContext.getSdaPin();
    int scl = globalContext.getSclPin();
    int address = globalContext.getRfidAddress();
};

} // namespace services

#endif // RFID_SERVICE_H
