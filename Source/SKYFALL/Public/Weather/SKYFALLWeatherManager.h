#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weather/SKYFALLWeatherTypes.h"
#include "SKYFALLWeatherManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSKYFALLWeatherChanged, FSKYFALLWeatherSnapshot, Snapshot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSKYFALLDisasterWarning, ESKYFALLWeatherType, Disaster, int32, WarningMinutes);

/** Regional simulation authority. Visual effects should subscribe from Blueprint/Niagara. */
UCLASS(BlueprintType, Blueprintable)
class SKYFALL_API ASKYFALLWeatherManager : public AActor
{
    GENERATED_BODY()
public:
    ASKYFALLWeatherManager();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(BlueprintAssignable, Category="SKYFALL|Weather")
    FSKYFALLWeatherChanged OnWeatherChanged;

    UPROPERTY(BlueprintAssignable, Category="SKYFALL|Disasters")
    FSKYFALLDisasterWarning OnDisasterWarning;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SKYFALL|Climate")
    ESKYFALLClimateZone ClimateZone = ESKYFALLClimateZone::Temperate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SKYFALL|Time", meta=(ClampMin="1.0"))
    float RealSecondsPerGameHour = 60.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SKYFALL|Time", meta=(ClampMin="1"))
    int32 DaysPerSeason = 7;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SKYFALL|Weather", meta=(ClampMin="1.0"))
    float MinHoursBetweenTransitions = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SKYFALL|Weather", meta=(ClampMin="1.0"))
    float MaxHoursBetweenTransitions = 9.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SKYFALL|Disasters", meta=(ClampMin="0.0", ClampMax="1.0"))
    float DisasterChancePerTransition = 0.08f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SKYFALL|Disasters", meta=(ClampMin="0.1"))
    float DisasterWarningHours = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SKYFALL|Disasters")
    bool bEnableDisasters = true;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="SKYFALL|Weather")
    FSKYFALLWeatherSnapshot CurrentSnapshot;

    UFUNCTION(BlueprintCallable, Category="SKYFALL|Weather")
    void ForceWeather(ESKYFALLWeatherType NewWeather);

    UFUNCTION(BlueprintCallable, Category="SKYFALL|Time")
    void SetGameTime(int32 NewDayOfYear, float NewHourOfDay);

    UFUNCTION(BlueprintPure, Category="SKYFALL|Weather")
    FSKYFALLWeatherSnapshot GetWeatherSnapshot() const { return CurrentSnapshot; }

private:
    float SecondsAccumulator = 0.0f;
    float HoursUntilTransition = 4.0f;
    float HoursUntilDisaster = -1.0f;

    void AdvanceGameTime(float GameHours);
    void ChooseNextWeather();
    void UpdateWeatherProperties();
    ESKYFALLSeason CalculateSeason(int32 DayOfYear) const;
    float CalculateSeasonalTemperature() const;
};
