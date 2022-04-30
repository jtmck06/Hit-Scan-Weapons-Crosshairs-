#pragma once

UENUM(BlueprintType)
enum class EAmmoType : uint8 {
	EAT_RifleAmmo UMETA(DisplayName = "RifleAmmo"),
	EAT_SMG UMETA(DisplayName = "SMG"),

	EAT_MAX UMETA(DisplayName = "DefaultMAX")
};
