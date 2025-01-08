#ifndef WALLET_INFORMATION_ENUM_H
#define WALLET_INFORMATION_ENUM_H

namespace enums {

enum class WalletInformationEnum {
    NONE = -1, // Aucun choix
    BALANCE,  // Balance de l'adresse
    ADDRESS,  // Adresse Bitcoin
    SIGNATURE, // Signe tx
    PUBLIC_KEY, // Public key du wallet
    FINGERPRINT, // Master public fingerprint
    DERIVE_PATH, // Derivation path for key
    COUNT
};

} // namespace enums

#endif // WALLET_INFORMATION_ENUM_H
