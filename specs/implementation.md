# Shared Vehicle Access: behavior and implementation

Describes the exported implementation **0.1.0**. The linked source and metadata define this version; gameplay qualification is a separate requirement.

## Behavior

Alt+F borrows an aimed unoccupied vehicle using native holder handover. Local physical vehicles use GiveVehicle/TakeVehicle followed by native Enter; remote views use AskForCrewVehicle. The equipment owner/buyer remains the original owner while the native holder changes. Runtime-sized rooted managed handover values preserve reference fields rather than assuming a fixed native struct.

## State and lifecycle

Holder/occupancy/ownership stay in native records; no extra permission or saved ownership ledger exists. Focus, typing, blocking UI, mounted-state and world-restoration gates apply. Occupied drivers are rejected. Rooted values live only as required for the transaction.

## Integration and configuration

No additional feature-mod runtime dependency is declared. Included shared headers are source reuse, not extra DLL dependencies.

The [module metadata](../source/mods/shared-vehicle-access/mod.json) owns the version and declared integration contract. Native modules start at `MoonLab_ModStart`. Game/Unity integrations target the pinned Steam Windows x64 IL2CPP build.

## Implementation sources

- [mod.c](../source/mods/shared-vehicle-access/mod.c)
- [base.h](../source/mods/shared-vehicle-access/base.h)

## Verification boundary

All peers need matching support. This alternate interaction does not replace every vanilla prompt or prove buyer-lock bypass, guest authority and network replay. Those require real host/guest checks.

`python3 build.py --check` verifies the exported source hashes. It does not execute the mod or qualify the behaviors above. Installation and independent build prerequisites are in the [README](../README.md).
