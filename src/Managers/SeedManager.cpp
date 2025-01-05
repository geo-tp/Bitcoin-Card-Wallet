#include "SeedManager.h"
#include <algorithm>
#include <stdexcept>

namespace managers {

SeedManager::SeedManager(const GlobalManager& gm)
    : GlobalManager(gm) // calls GlobalManager's copy constructor
{}

std::vector<uint8_t> SeedManager::managePrivateKey() {
  // Generate private keys and verify randomness
  std::vector<uint8_t> privateKey;
  do {
    privateKey = cryptoService.generatePrivateKey();
  } while (cryptoService.calculateShanonEntropy(privateKey) < 4.9);

  return privateKey;
}

void SeedManager::manageMnemonic(std::vector<std::string>& mnemonic) {
  bool mnemonicIsBackedUp = false;
  bool mnemonicVerification = false;
  std::string word;
  uint8_t randomNumber;

  do {
    // Show the 24 words
    display.displayTopBar("Write Seed", false, false, true);
    mnemonicSelection.select(mnemonic);

    // Verify Backup
    mnemonicVerification = confirmationSelection.select(" Verify backup?");
    if (mnemonicVerification) {
      // Random num for a word index
      randomNumber = rand() % mnemonic.size();
      // Ask user the correct word for the given index
      display.displayTopBar("Verify Seed", false, false, true);
      auto question = "What's the " + std::to_string(randomNumber + 1) + " word ?";
      word = stringPromptSelection.select(question, 8);
    }

    // User want to verify the seed but words are different
    if (mnemonicVerification && word != mnemonic[randomNumber]) {
      if (!word.empty()) {display.displaySubMessage("Wrong answer", 50, 2000);}
    } else {
      display.displaySubMessage("Seed backup done", 36, 2000);
      mnemonicIsBackedUp = true;
    }
  } while (!mnemonicIsBackedUp);
}

} // namespace managers
