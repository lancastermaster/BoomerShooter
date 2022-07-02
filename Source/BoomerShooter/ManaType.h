#pragma once

UENUM(BlueprintType)
	enum class EManaType : uint8 {
		EMT_Fire = 0 UMETA(DisplayName = "Fire"),
		EMT_Ice = 1 UMETA(DisplayName = "Ice"),
		EMT_Lightning = 2 UMETA(DisplayName = "Lightning"),

		EMT_MAX = 7 UMETA(DisplayName = "DefaultMAX")
	};