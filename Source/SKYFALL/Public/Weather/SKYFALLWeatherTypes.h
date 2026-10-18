#pragma once
#include "CoreMinimal.h"
#include "SKYFALLWeatherTypes.generated.h"

UENUM(BlueprintType)
enum class ESKYFALLSeason : uint8 { Spring, Summer, Autumn, Winter };

UENUM(BlueprintType)
enum class ESKYFALLWeatherType : uint8
{
    Clear, PartlyCloudy, Overcast, Fog, Drizzle, Rain, HeavyRain,
    Thunderstorm, Snow, Blizzard, Sandstorm, Heatwave, ColdSnap, Hail
};

UENUM(BlueprintType)
enum class ESKYFALLClimateZone : uint8
{
    Temperate, Desert, Tropical, Arctic, Mountain, Coastal
};

USTRUCT(BlueprintType)
struct FSKYFALLWeatherSnapshot
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SKYFALL|Weather")
    ESKYFALLSeason Season = ESKYFALLSeason::Spring;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SKYFALL|Weather")
    ESKYFALLWeatherType Weather = ESKYFALLWeatherType::Clear;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SKYFALL|Weather")
    float TemperatureCelsius = 22.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SKYFALL|Weather", meta=(ClampMin="0.0", ClampMax="1.0"))
    float Precipitation = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SKYFALL|Weather", meta=(ClampMin="0.0", ClampMax="1.0"))
    float WindStrength = 0.1f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SKYFALL|Weather", meta=(ClampMin="0.0", ClampMax="1.0"))
    float VisibilityLoss = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SKYFALL|Weather")
    bool bDisasterWarning = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SKYFALL|Time")
    int32 DayOfYear = 1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SKYFALL|Time")
    float HourOfDay = 8.0f;
};
