# SKYFALL Dynamic Weather System

Unreal Engine 5 C++ starter for a scalable regional weather simulation.

## Included in this first implementation
- Four-season calendar: spring, summer, autumn, winter.
- Regional climate profiles: temperate, desert, tropical, arctic, mountain, coastal.
- Weather states: clear, clouds, fog, drizzle, rain, heavy rain, thunderstorms, snow, blizzards, sandstorms, heatwaves, cold snaps and hail.
- Blueprint events for weather changes and disaster warnings.
- Temperature, precipitation, wind, visibility, and time values for gameplay and UI.

## Install into Unreal Engine 5
1. Create a C++ Games project named SKYFALL. The module must be named SKYFALL because the source uses SKYFALL_API.
2. Copy Source/SKYFALL into the project's Source directory. If Unreal already generated SKYFALL.Build.cs, merge the dependencies instead of blindly overwriting your existing rules.
3. Make sure the .uproject declares a Runtime module named SKYFALL.
4. Generate project files and compile Development Editor.
5. Create a Blueprint class derived from ASKYFALLWeatherManager and place it in a test level.
6. Select a climate zone in Details and bind OnWeatherChanged / OnDisasterWarning.
7. In Blueprint, connect snapshot values to Niagara rain/snow/sand systems, Sky Atmosphere, Volumetric Clouds, Exponential Height Fog, lightning, thunder audio and wet-road materials.

## Next modules for a production system
- Weather presentation controller: blend sky, clouds, fog, post-process, Niagara and audio.
- World Partition-aware climate subsystem: persist weather per streamed region and update distant regions less often.
- Gameplay integration: traction, NPC shelter behavior, crop yield, flooding, heat/cold exposure.
- Disaster director: staged warnings, evacuation, damage budgets and recovery.
- Automation tests for season rollover, climate constraints and deterministic weather seeds.

## Honest limitations
This is a foundation, not a finished AAA renderer. It has not been compiled against your exact UE version in this session. Visual assets, physical forecast modeling, flood/water simulation, multiplayer replication, save-game persistence and Android profiling are not included yet. For a huge streamed world, use one authoritative snapshot per climate region and lower update frequency for distant regions.
