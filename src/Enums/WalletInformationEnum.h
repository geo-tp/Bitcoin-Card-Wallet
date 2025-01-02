#ifndef WALLET_INFORMATION_ENUM_H
#define WALLET_INFORMATION_ENUM_H

namespace enums {

enum class WalletInformationEnum {
    NONE = -1, // Aucun choix
    BALANCE,  // Balance de l'adresse
    ADDRESS,  // Adresse Bitcoin
    PUBLIC_KEY, // Public key du wallet
    COUNT
};

} // namespace enums

#endif // WALLET_INFORMATION_ENUM_H
