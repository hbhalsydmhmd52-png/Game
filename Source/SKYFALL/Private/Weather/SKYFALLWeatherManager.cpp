#include "Weather/SKYFALLWeatherManager.h"
#include "Math/UnrealMathUtility.h"

ASKYFALLWeatherManager::ASKYFALLWeatherManager()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 0.25f;
}

void ASKYFALLWeatherManager::BeginPlay()
{
    Super::BeginPlay();
    DaysPerSeason = FMath::Max(1, DaysPerSeason);
    RealSecondsPerGameHour = FMath::Max(1.0f, RealSecondsPerGameHour);
    MinHoursBetweenTransitions = FMath::Max(1.0f, MinHoursBetweenTransitions);
    MaxHoursBetweenTransitions = FMath::Max(MinHoursBetweenTransitions, MaxHoursBetweenTransitions);
    CurrentSnapshot.DayOfYear = FMath::Clamp(CurrentSnapshot.DayOfYear, 1, DaysPerSeason * 4);
    CurrentSnapshot.HourOfDay = FMath::Clamp(CurrentSnapshot.HourOfDay, 0.0f, 23.99f);
    CurrentSnapshot.Season = CalculateSeason(CurrentSnapshot.DayOfYear);
    HoursUntilTransition = FMath::FRandRange(MinHoursBetweenTransitions, MaxHoursBetweenTransitions);
    UpdateWeatherProperties();
    OnWeatherChanged.Broadcast(CurrentSnapshot);
}

void ASKYFALLWeatherManager::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (RealSecondsPerGameHour <= 0.0f) return;

    SecondsAccumulator += DeltaSeconds;
    const float SimulatedHours = SecondsAccumulator / RealSecondsPerGameHour;
    if (SimulatedHours < 0.01f) return;
    SecondsAccumulator -= SimulatedHours * RealSecondsPerGameHour;

    AdvanceGameTime(SimulatedHours);
    HoursUntilTransition -= SimulatedHours;
    if (HoursUntilTransition <= 0.0f)
    {
        ChooseNextWeather();
        HoursUntilTransition = FMath::FRandRange(
            FMath::Max(1.0f, MinHoursBetweenTransitions),
            FMath::Max(MinHoursBetweenTransitions, MaxHoursBetweenTransitions));
    }

    if (HoursUntilDisaster >= 0.0f)
    {
        HoursUntilDisaster -= SimulatedHours;
        if (HoursUntilDisaster <= 0.0f)
        {
            HoursUntilDisaster = -1.0f;
            CurrentSnapshot.bDisasterWarning = false;
            UpdateWeatherProperties();
            OnWeatherChanged.Broadcast(CurrentSnapshot);
        }
    }
}

void ASKYFALLWeatherManager::AdvanceGameTime(float GameHours)
{
    const float TotalHours = CurrentSnapshot.HourOfDay + GameHours;
    const int32 DaysPassed = FMath::FloorToInt(TotalHours / 24.0f);
    CurrentSnapshot.HourOfDay = FMath::Fmod(TotalHours, 24.0f);

    if (DaysPassed > 0)
    {
        const int32 YearLength = FMath::Max(4, DaysPerSeason * 4);
        CurrentSnapshot.DayOfYear = ((CurrentSnapshot.DayOfYear - 1 + DaysPassed) % YearLength) + 1;
        const ESKYFALLSeason NewSeason = CalculateSeason(CurrentSnapshot.DayOfYear);
        if (NewSeason != CurrentSnapshot.Season)
        {
            CurrentSnapshot.Season = NewSeason;
            ChooseNextWeather();
        }
    }
    UpdateWeatherProperties();
}

ESKYFALLSeason ASKYFALLWeatherManager::CalculateSeason(int32 DayOfYear) const
{
    const int32 Index = ((FMath::Max(1, DayOfYear) - 1) / FMath::Max(1, DaysPerSeason)) % 4;
    switch (Index)
    {
        case 0: return ESKYFALLSeason::Spring;
        case 1: return ESKYFALLSeason::Summer;
        case 2: return ESKYFALLSeason::Autumn;
        default: return ESKYFALLSeason::Winter;
    }
}

float ASKYFALLWeatherManager::CalculateSeasonalTemperature() const
{
    float Base = 16.0f;
    switch (ClimateZone)
    {
        case ESKYFALLClimateZone::Desert: Base = 32.0f; break;
        case ESKYFALLClimateZone::Tropical: Base = 28.0f; break;
        case ESKYFALLClimateZone::Arctic: Base = -8.0f; break;
        case ESKYFALLClimateZone::Mountain: Base = 7.0f; break;
        case ESKYFALLClimateZone::Coastal: Base = 21.0f; break;
        default: break;
    }
    switch (CurrentSnapshot.Season)
    {
        case ESKYFALLSeason::Spring: return Base + 2.0f;
        case ESKYFALLSeason::Summer: return Base + 10.0f;
        case ESKYFALLSeason::Autumn: return Base - 1.0f;
        case ESKYFALLSeason::Winter: return Base - 12.0f;
        default: return Base;
    }
}

void ASKYFALLWeatherManager::ChooseNextWeather()
{
    ESKYFALLWeatherType Next = ESKYFALLWeatherType::Clear;
    const float Roll = FMath::FRand();

    if (bEnableDisasters && Roll < FMath::Clamp(DisasterChancePerTransition, 0.0f, 1.0f))
    {
        switch (ClimateZone)
        {
            case ESKYFALLClimateZone::Desert: Next = ESKYFALLWeatherType::Sandstorm; break;
            case ESKYFALLClimateZone::Arctic: Next = ESKYFALLWeatherType::Blizzard; break;
            case ESKYFALLClimateZone::Tropical:
            case ESKYFALLClimateZone::Coastal: Next = ESKYFALLWeatherType::Thunderstorm; break;
            default: Next = FMath::FRand() < 0.5f ? ESKYFALLWeatherType::Thunderstorm : ESKYFALLWeatherType::Hail; break;
        }
        CurrentSnapshot.bDisasterWarning = true;
        HoursUntilDisaster = FMath::Max(0.1f, DisasterWarningHours);
        OnDisasterWarning.Broadcast(Next, FMath::RoundToInt(DisasterWarningHours * 60.0f));
    }
    else
    {
        CurrentSnapshot.bDisasterWarning = false;
        HoursUntilDisaster = -1.0f;
        const float R = FMath::FRand();

        if (ClimateZone == ESKYFALLClimateZone::Desert)
        {
            Next = R < 0.58f ? ESKYFALLWeatherType::Clear :
                R < 0.78f ? ESKYFALLWeatherType::PartlyCloudy :
                R < 0.90f ? ESKYFALLWeatherType::Sandstorm : ESKYFALLWeatherType::Heatwave;
        }
        else if (ClimateZone == ESKYFALLClimateZone::Arctic || ClimateZone == ESKYFALLClimateZone::Mountain)
        {
            Next = R < 0.18f ? ESKYFALLWeatherType::Clear :
                R < 0.38f ? ESKYFALLWeatherType::Overcast :
                R < 0.72f ? ESKYFALLWeatherType::Snow :
                R < 0.88f ? ESKYFALLWeatherType::Blizzard : ESKYFALLWeatherType::ColdSnap;
        }
        else
        {
            Next = R < 0.26f ? ESKYFALLWeatherType::Clear :
                R < 0.43f ? ESKYFALLWeatherType::PartlyCloudy :
                R < 0.58f ? ESKYFALLWeatherType::Overcast :
                R < 0.66f ? ESKYFALLWeatherType::Fog :
                R < 0.76f ? ESKYFALLWeatherType::Drizzle :
                R < 0.90f ? ESKYFALLWeatherType::Rain : ESKYFALLWeatherType::HeavyRain;
        }
    }

    CurrentSnapshot.Weather = Next;
    UpdateWeatherProperties();
    OnWeatherChanged.Broadcast(CurrentSnapshot);
}

void ASKYFALLWeatherManager::UpdateWeatherProperties()
{
    CurrentSnapshot.Season = CalculateSeason(CurrentSnapshot.DayOfYear);
    CurrentSnapshot.TemperatureCelsius = CalculateSeasonalTemperature() + FMath::FRandRange(-3.0f, 3.0f);
    CurrentSnapshot.Precipitation = 0.0f;
    CurrentSnapshot.WindStrength = 0.15f;
    CurrentSnapshot.VisibilityLoss = 0.0f;

    switch (CurrentSnapshot.Weather)
    {
        case ESKYFALLWeatherType::Drizzle:
            CurrentSnapshot.Precipitation = 0.25f; CurrentSnapshot.WindStrength = 0.25f; break;
        case ESKYFALLWeatherType::Rain:
            CurrentSnapshot.Precipitation = 0.55f; CurrentSnapshot.WindStrength = 0.4f; break;
        case ESKYFALLWeatherType::HeavyRain:
            CurrentSnapshot.Precipitation = 0.85f; CurrentSnapshot.WindStrength = 0.65f;
            CurrentSnapshot.VisibilityLoss = 0.25f; break;
        case ESKYFALLWeatherType::Thunderstorm:
            CurrentSnapshot.Precipitation = 1.0f; CurrentSnapshot.WindStrength = 0.9f;
            CurrentSnapshot.VisibilityLoss = 0.4f; break;
        case ESKYFALLWeatherType::Snow:
            CurrentSnapshot.Precipitation = 0.5f; CurrentSnapshot.WindStrength = 0.35f;
            CurrentSnapshot.TemperatureCelsius = FMath::Min(CurrentSnapshot.TemperatureCelsius, -1.0f); break;
        case ESKYFALLWeatherType::Blizzard:
            CurrentSnapshot.Precipitation = 0.9f; CurrentSnapshot.WindStrength = 1.0f;
            CurrentSnapshot.VisibilityLoss = 0.85f;
            CurrentSnapshot.TemperatureCelsius = FMath::Min(CurrentSnapshot.TemperatureCelsius, -8.0f); break;
        case ESKYFALLWeatherType::Sandstorm:
            CurrentSnapshot.WindStrength = 1.0f; CurrentSnapshot.VisibilityLoss = 0.9f; break;
        case ESKYFALLWeatherType::Fog:
            CurrentSnapshot.VisibilityLoss = 0.65f; break;
        case ESKYFALLWeatherType::Heatwave:
            CurrentSnapshot.TemperatureCelsius += 12.0f; break;
        case ESKYFALLWeatherType::ColdSnap:
            CurrentSnapshot.TemperatureCelsius -= 12.0f; break;
        case ESKYFALLWeatherType::Hail:
            CurrentSnapshot.Precipitation = 0.75f; CurrentSnapshot.WindStrength = 0.7f; break;
        default: break;
    }
}

void ASKYFALLWeatherManager::ForceWeather(ESKYFALLWeatherType NewWeather)
{
    CurrentSnapshot.Weather = NewWeather;
    CurrentSnapshot.bDisasterWarning = false;
    HoursUntilDisaster = -1.0f;
    UpdateWeatherProperties();
    OnWeatherChanged.Broadcast(CurrentSnapshot);
}

void ASKYFALLWeatherManager::SetGameTime(int32 NewDayOfYear, float NewHourOfDay)
{
    const int32 YearLength = FMath::Max(4, DaysPerSeason * 4);
    CurrentSnapshot.DayOfYear = ((FMath::Max(1, NewDayOfYear) - 1) % YearLength) + 1;
    CurrentSnapshot.HourOfDay = FMath::Clamp(NewHourOfDay, 0.0f, 23.99f);
    CurrentSnapshot.Season = CalculateSeason(CurrentSnapshot.DayOfYear);
    UpdateWeatherProperties();
    OnWeatherChanged.Broadcast(CurrentSnapshot);
}
