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
  manager.display.displayRfidInfos();
  manager.input.waitPress();
  manager.display.displayRfidTagInfos();
  manager.input.waitPress();

  selectionContext.setIsModeSelected(false); // go back to menu
}

void SeedController::handleSeedGeneration() {
  manager.manageNewSeedCreation();
}

void SeedController::handleSeedRestoration() {
  auto transactionOngoing = selectionContext.getTransactionOngoing();
  if (transactionOngoing) {
    manager.display.displaySeedLoadInfos();
    manager.input.waitPress();
  } else {
    manager.display.displaySubMessage("Restore a wallet", 45, 1500);
  }

  auto title = transactionOngoing ? "Load Seed" : "Restore";
  manager.display.displayTopBar(title, true, false, true, 15);

  auto restorationMethod = manager.seedRestorationSelection.select();
  switch (restorationMethod) {
      case SeedRestorationModeEnum::NONE:
        if (transactionOngoing) {
          selectionContext.setCurrentSelectedMode(SelectionModeEnum::PORTFOLIO);
        } else {
          selectionContext.setIsModeSelected(false); // go to menu
        }
        break;

      case SeedRestorationModeEnum::RFID:
          if (transactionOngoing) {
            manager.manageRfidSeedLoading();
          } else {
            manager.manageRfidSeedRestoration();
          }
          break;

      case SeedRestorationModeEnum::SD:
          manager.display.displaySubMessage("12 or 24 words txt file", 19, 3500);
          selectionContext.setCurrentSelectedFileType(FileTypeEnum::SEED);
          selectionContext.setCurrentSelectedMode(SelectionModeEnum::LOAD_SD); // sd file browser
          break;

      case SeedRestorationModeEnum::WORDS_12:
          if(transactionOngoing) {
            manager.manageMnemonicLoading(12);
          } else {
            manager.manageMnemonicRestore(12);
          }
          break;

      case SeedRestorationModeEnum::WORDS_24:
          if(transactionOngoing) {
            manager.manageMnemonicLoading(24);
          } else {
            manager.manageMnemonicRestore(24);
          }
          break;
  }

}

} // namespace controllers
