#ifndef SD_SERVICE_H
#define SD_SERVICE_H

#include <SD.h>
#include <SPI.h>
#include <vector>
#include <string>
#include <Contexts/GlobalContext.h>
#include <M5Cardputer.h>

using namespace contexts;

namespace services {


class SdService {
public:
    SdService();
    bool begin(); 
    void close();
    bool isFile(std::string filePath);
    bool getSdState();

    std::vector<std::string> listElements(std::string dirPath, size_t limit=0);
    std::string readFile(const char* filePath);
    bool writeFile(const char* filePath, const std::string& data);
    bool writeBinaryFile(const char* filePath, const std::vector<uint8_t>& data);
    bool appendToFile(const char* filePath, const std::string& data);
    std::vector<uint8_t> readBinaryFile(const char* filePath);
    bool deleteFile(const char* filePath);
private:
    GlobalContext& globalContext = GlobalContext::getInstance();
    SPIClass sdCardSPI;
    bool sdCardMounted = false; 
};

}
#endif // SD_SERVICE_H
