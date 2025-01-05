#include "SeedController.h"

namespace controllers {

SeedController::SeedController(SeedManager& seedManager) : manager(seedManager)  {}

void SeedController::handleSeedInformations() {
  // Infos screens
  manager.display.displaySeedGeneralInfos();
  manager.input.waitPress();
  manager.display.displaySdSaveGeneralInfos();
  manager.input.waitPress();
  manager.display.displaySeedFormatGeneralInfos();
  manager.input.waitPress();

  selectionContext.setIsModeSelected(false); // go back to menu
}

void SeedController::handleSeedGeneration() {
  manager.manageNewSeedCreation();
}

void SeedController::handleSeedRestoration() {
  manager.display.displayTopBar("Load Seed", true, false, true, 15);
  auto restorationMethod = manager.seedRestorationSelection.select();

  if (restorationMethod == SeedRestorationModeEnum::NONE) {
      selectionContext.setIsModeSelected(false); // go to menu
      return;
  }

  std::vector<uint8_t> privateKey;
  std::vector<std::string> mnemonic;
  std::string walletName;
  std::string mnemonicString;
  std::string passphrase;
  std::string address;
  HDPublicKey publicKey;
  Wallet wallet;

  switch (restorationMethod) {
      case SeedRestorationModeEnum::NONE:
          selectionContext.setIsModeSelected(false); // go to menu

      case SeedRestorationModeEnum::RFID:
          manager.manageRfidSeedRestoration();
          break;

      case SeedRestorationModeEnum::SD:
          selectionContext.setCurrentSelectedFileType(FileTypeEnum::SEED);
          selectionContext.setCurrentSelectedMode(SelectionModeEnum::LOAD_SD); // sd file browser
          break;
          
      case SeedRestorationModeEnum::WORDS_12:
          manager.manageMnemonicRestore(12);
          break;

      case SeedRestorationModeEnum::WORDS_24:
          manager.manageMnemonicRestore(24);
          break;

      default:
          manager.display.displayDebug("Invalid resto mode");
          break;
  }

}

} // namespace controllers
