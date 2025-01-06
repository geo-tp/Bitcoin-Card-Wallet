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
  auto transactionOngoing = selectionContext.getTransactionOngoing();
  if (transactionOngoing) {
    manager.display.displaySeedLoadInfos();
    manager.input.waitPress();
  }

  manager.display.displayTopBar("Load Seed", true, false, true, 15);
  auto restorationMethod = manager.seedRestorationSelection.select();


  if (restorationMethod == SeedRestorationModeEnum::NONE) {
      selectionContext.setIsModeSelected(false); // go to menu
      return;
  }

  switch (restorationMethod) {
      case SeedRestorationModeEnum::NONE:
          selectionContext.setIsModeSelected(false); // go to menu

      case SeedRestorationModeEnum::RFID:
          if (transactionOngoing) {
            manager.manageRfidSeedSignature();
          } else {
            manager.manageRfidSeedRestoration();
          }
          break;

      case SeedRestorationModeEnum::SD:
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
