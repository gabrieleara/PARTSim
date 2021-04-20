# Migrating from `CPU`+`CPU_BL`+`Island_BL` to New `CPUIsland`+`CPU` implementation

Author: *Gabriele Ara (gabriele.ara@santannapisa.it)*<br>
Date: *2021-04-20*

This file tracks the necessary changes to make this transition work, what has
been deprecated, and what has been renamed, and other general facts about the
new implementation.

It should help people already familiar to the old implementation to move on to
the new one (and also myself when updating the other components as well, like
the Kernel, etc.).

## Document Structure

After a brief motivation behind the introduction of a completely new
implementation of the `CPU`/`CPUIsland` classes, the document moves on listing
what happened to each method/behavior of the original classes in file order (in
the hope that I did not forget anything).

It is useful to keep a copy of the original `cpu.hpp` and `cpu.cpp` files
alongside the new implementation when following this document (because that's
how it has been written in the first place).

> **Note:** It is advisable to read these tables using a renderer for Markdown
> documents. They tend to span a large number of columns.

## Motivation

The old implementation had a `CPU` class, which was kind of outdated, and a
`CPU_BL` class, which implemented some stuff, including the grouping of CPUs
under CPU islands, which was not present in the original implementation.

The new implementation aims to generalize the `CPU` class itself, removing
unused or obscure stuff from the original class and implementing the grouping of
CPUs into islands in that base class. In the process, I probably broke far too
many implemented components elsewhere, sometimes because a name of a method was
far too obscure or misleading and sometimes because certain internal behaviors
clashed with others developed later.

In the hope that the new implementation will be one single coherent box, I
decided to re-implement everything from scratch and with that it was inevitable
that code using these classes would break.

This document is provided in an attempt to make sense of all the changes between
the new and the old implementation, to justify the changes and as a reference to
fix what is now broken elsewhere in RTSim.

# Complete list of changes

<!-- --- -->

## Class Attributes

Following is a list of each attribute of each class and what is new about them.
First, I will list what happened to old attributes, then I will list attributes
of new classes.

### Old `CPU` Class

Most of the attributes originally part of this class have been moved to the
`CPUIsland` class, which manages all `OPP`s and the linked `CPUModel`, or have
been removed.

| Original Attribute       | Change                  | Reason                                                                                                                                                                                                                         |
| :----------------------- | :---------------------- | :----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `_max_power_consumption` | Renamed to `_max_power` | -                                                                                                                                                                                                                              |
| `powmod`                 | Moved to `CPUIsland`    | All `CPU`s now share the same model when belonging to the same island. Models are accessed in a stateless fashion (with updates meant to cache some values only when actual changes to current system condition is triggered). |
| `_workload`              | -                       | -                                                                                                                                                                                                                              |
| `OPPs`                   | Moved to `CPUIsland`    | All OPP management operations now belong to the `CPUIsland`.                                                                                                                                                                   |
| `speeds`                 | **Deprecated**          | This was used by old implementations using only what would later become the *Minimal CPU Model* to select the correct OPP to run the current CPU load. See [this section](#considerable-changes-in-behavior) for more details. |
| `cpuName`                | **Deprecated**          | I thought it was useless since the `Entity` class already manages the name of the entity itself (including the CPU).                                                                                                           |
| `currentOPP`             | Moved to `CPUIsland`    | All OPP management operations now belong to the `CPUIsland`.                                                                                                                                                                   |
| `PowerSaving`            | **Deprecated**          | It was a redundant flag, a new invariant now returns whether the CPU is capable of energy saving by checking if it has a `CPUIsland` with a `CPUModel` and multiple `OPP`s linked to it.                                       |
| `frequencySwitching`     | Moved to `CPUIsland`    | All OPP management operations now belong to the `CPUIsland`.                                                                                                                                                                   |
| `index`                  | Renamed to `_index`     | Consistency with the naming of other new attributes (and the naming of attributes in the old `CPU_BL` class).                                                                                                                  |

### Old `CPU_BL` Class

This class has been integrated in the base `CPU` class.

| Original Attribute | Change                 | Reason                                                                                                                                                                                                                                                                                                                                                                                                   |
| :----------------- | :--------------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `_island`          | Changed attribute type | Changed from `Island_BL` to the now more generic `CPUIsland`.                                                                                                                                                                                                                                                                                                                                            |
| `isBusy`           | **Deprecated**         | Now a simple invariant checks whether a `CPU` is busy or not depending on its `disabled` state and the current assigned `workload`.                                                                                                                                                                                                                                                                      |
| `_pm`<sup>1<sup>   | Moved to `CPUIsland`   | Originally, the linked `CPUModel` to a `CPU` was used as a stateful object, keeping the current power consumption/speed of the executing `workload` at any time. Now (despite still supporting stateful usage), the `CPUModel` is always queried as a stateless class, and as such it can be safely moved to the `CPUIsland` class, where it naturally belongs, shared by all `CPU`s in the same island. |
| `_disabled`        | -                      | Kept. This is meant for debugging purposes only.                                                                                                                                                                                                                                                                                                                                                         |

> Note:
> 1. This model is used differently than the one in the base old `CPU` class.

### Old `Island_BL` Class

This class has been integrated in the new more generic `CPUIsland` class.

| Original Attribute  | Change                    | Reason                                                                                                                                                             |
| :------------------ | :------------------------ | :----------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `static IslandName` | **Deprecated**            | There is no need anymore, the new `CPUIsland::Type` data field supports automatic conversion to `std::string`.                                                     |
| `_island`           | Renamed to `_type`        | It used to indicate the `Type` of the island, might as well be called as such.                                                                                     |
| `_cpus`             | Changed to a `std::set`   | Adding multiple times pointers to the same `CPU` object no longer results in multiple `CPU`s counted.                                                              |
| `_opps`             | -                         | -                                                                                                                                                                  |
| `_currentOPP`       | Renamed to `_current_opp` | -                                                                                                                                                                  |
| `_kernel`           | **Deprecated**            | *Provisional change.* Previously used to notify the linked `EnergyMRTKernel` of an `OPP` change. I'm still investigating the usage of this notification mechanism. |

### New `CPUIsland` Class

The following list denotes the origin of each attribute.

| New Attribute         | Origin (old implementation)    | Notes |
| :-------------------- | :----------------------------- | :---- |
| `_type`               | `Island_BL::_island`           | -     |
| `_powermodel`         | `CPU_BL::_pm`                  | -     |
| `_cpus`               | `Island_BL::_cpus`             | -     |
| `_opps`               | `Island_BL::_opps`/`CPU::OPPs` | -     |
| `_current_opp`        | `Island_BL::_currentOPP`       | -     |
| `_frequency_switches` | `CPU::frequencySwitching`      | -     |


### New `CPU` Class

The following list denotes the origin of each attribute.

| New Attribute | Origin (old implementation)   | Notes                                                                                                        |
| :------------ | :---------------------------- | :----------------------------------------------------------------------------------------------------------- |
| `_index`      | `CPU::index`                  | -                                                                                                            |
| `_island`     | `CPU_BL::_island`             | -                                                                                                            |
| `_workload`   | `CPU::_workload`              | -                                                                                                            |
| `_cpu_power`  | **New**                       | Caches the power consumption in current `OPP`+`workload` configuration, obtained from the linked `CPUModel`. |
| `_cpu_speed`  | **New**                       | Caches the speed of the current `workload` in current `OPP`, obtained from the linked `CPUModel`.            |
| `_max_power`  | `CPU::_max_power_consumption` | -                                                                                                            |
| `_disabled`   | `CPU_BL::_disabled`           | -                                                                                                            |

<!-- --- -->

## Class Methods

Following is a list of each method of both old and new implementations, with
each change to them.

I will list first methods of old classes, then the ones in new implementations.
The new implementation entirely is documented in the code, so I'll indicate here
only meaningful notes for the transition to the new implementation from the old
one.

Methods are ordered as they appear at the time of writing in `cpu.hpp` header
files.

> **Legend for the *Behavior Change* column:** the value "**Internal**" means just a
> simple change of the internal behavior, while "**Break**" a breaking change from
> an external perspective as well. "**Removed**" methods implicitly break API
> contract with external classes. "**Renamed**" methods of course break API
> contracts as well, but it is an easy fix to just rename stuff in other files
> by looking at these tables.

### Old `CPU` Class

With respect to the old CPU class, the new one has new constructors (while still
providing the same constructor interface as before).

The constructor that is compatible with the one of the old implementation now
creates a linked `CPUIsland` whenever a `CPU` without an island is created. For
this reason, it is advisable in the new implementation to create `CPUIsland`s
first and then use the new `CPU` constructor that accepts a `CPUIsland` as
argument to create each `CPU`. `CPU`s created using this method will
automatically be linked to the provided `CPUIsland`.

Now, on to the methods:

| Original Method              | Behavior Change | Renamed?                    | Description                                                                                                                                                               |
| :--------------------------- | :-------------- | :-------------------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `updateCPUModel`             | Internal        | `CPU::updateModel`          | The new implementation does not **strictly** need this method (`CPUModel`s can now be easily used as stateless classes), but it is provided to cache values upon changes. |
| `getName`                    | **Removed**     | -                           | It simply returned the same value as `Entity::getName`                                                                                                                    |
| `toString`                   | -               | -                           | -                                                                                                                                                                         |
| `setIndex`                   | -               | -                           | -                                                                                                                                                                         |
| `getIndex`                   | -               | -                           | -                                                                                                                                                                         |
| `getOPPs`                    | Internal        | -                           | Forwards always to `CPUIsland::getOPPs`.                                                                                                                                  |
| `getOPP`                     | Internal        | -                           | Forwards always to `CPUIsland::getOPPIndex`. **TODO:** rename to `getOPPIndex` as well.                                                                                   |
| `setOPP`                     | Internal        | -                           | Forwards always to `CPUIsland::setOPP`.                                                                                                                                   |
| `getFrequency`               | Internal        | -                           | Forwards always to `CPUIsland::getFrequency`.                                                                                                                             |
| `getVoltage`                 | Internal        | -                           | Forwards always to `CPUIsland::getVoltage`.                                                                                                                               |
| `getMaxPowerConsumption`     | -               | `CPU::getPowerMax`          | -                                                                                                                                                                         |
| `setMaxPowerConsumption`     | -               | `CPU::setPowerMax`          | -                                                                                                                                                                         |
| `getCurrentPowerConsumption` | -               | `CPU::getPower`             | **NOTE:** this new method has some overloads. The basic version with no argument is the equivalent one.                                                                   |
| `getCurrentPowerSaving`      | -               | `CPU::getPowerSaving`       | **NOTE:** this new method has some overloads. The basic version with no argument is the equivalent one.                                                                   |
| `setSpeed`                   | **Removed**     | -                           | **TODO:** implement it.                                                                                                                                                   |
| `setWorkload`                | -               | -                           | -                                                                                                                                                                         |
| `getWorkload`                | -               | -                           | -                                                                                                                                                                         |
| `getSpeed`                   | -               | -                           | -                                                                                                                                                                         |
| `getSpeedByOPP`              | Internal        | -                           | This method does not trigger an OPP change anymore, thanks to the new lookup mechanism.                                                                                   |
| `getFrequencySwitching`      | Internal        | `CPU::getFrequencySwitches` | Forwards always to `CPUIsland::getFrequencySwitches`.                                                                                                                     |
| `check`                      | **Removed**     | -                           | **TODO:** under investigation.                                                                                                                                            |
| `newRun`                     | -               | -                           | -                                                                                                                                                                         |
| `endRun`                     | -               | -                           | -                                                                                                                                                                         |

### Old `CPU_BL` Class

Stuff from this class has been integrated into the new `CPU` implementation.

Considerations in previous section about constructors are valid for this class
as well.

All power and speed methods still implicitly require the existence of a linked
`CPUIsland`.

Following is the list of this class methods (implicitly, methods with same name
have been moved to `CPU`):

| Original Method                  | Behavior Change | Renamed?                        | Description                                                                                                                               |
| :------------------------------- | :-------------- | :------------------------------ | :---------------------------------------------------------------------------------------------------------------------------------------- |
| `getOPP`                         | -               | -                               | **TODO:** rename to `getOPPIndex` as well.                                                                                                |
| `setOPP`                         | -               | -                               | -                                                                                                                                         |
| `getHigherOPPs`                  | -               | -                               | -                                                                                                                                         |
| `setBusy`                        | **Removed**     | -                               | See `isBusy`.                                                                                                                             |
| `isIslandBusy`                   | **Removed**     | -                               | Simply call `getIsland()->busy()`.                                                                                                        |
| `isBusy`                         | **Break?**      | `CPU::busy`                     | The `busy` state of a `CPU` is now tied to the following invariant: `!disabled() && getWorkload() != "idle"`                              |
| `isDisabled`                     | -               | `CPU::disabled`/`CPU::enabled`  |                                                                                                                                           |
| `toggleDisabled`                 | **Removed**     | -                               | Call `CPU::disable`/`CPU::enable` in the proper order to accomplish similar results.                                                      |
| `getIsland`                      | -               | -                               | -                                                                                                                                         |
| `getIslandType`                  | **Removed**     | -                               | Simply call `getIsland()->getIslandType()`.                                                                                               |
| `getCurrentPowerConsumption`     | Internal        | `CPU::getPower`                 | No longer triggers an OPP change. **NOTE:** this new method has some overloads. The basic version with no argument is the equivalent one. |
| `getPowerConsumption`            | Internal        | `CPU::getPower(freq_type freq)` | No longer triggers an OPP change.                                                                                                         |
| `getSpeed`                       | Internal        | -                               | No longer triggers an OPP change.                                                                                                         |
| `getSpeed(freq_type freq)`       | Internal        | -                               | No longer triggers an OPP change.                                                                                                         |
| `getSpeedByOPP`                  | Internal        | -                               | No longer triggers an OPP change.                                                                                                         |
| `getFrequency`                   | -               | -                               | -                                                                                                                                         |
| `getFrequency(size_t opp_index)` | -               | -                               | -                                                                                                                                         |
| `getVoltage`                     | -               | -                               | -                                                                                                                                         |

### Old `Island_BL` Class

Constructor of the new `CPUIsland` class is similar, but different than the one
provided by the old class.

| Original Method                  | Behavior Change | Renamed?                              | Description                                                                                                 |
| :------------------------------- | :-------------- | :------------------------------------ | :---------------------------------------------------------------------------------------------------------- |
| `setKernel`                      | **Removed**     | -                                     | **TODO:** Investigate this mechanism.                                                                       |
| `getOPPsize`                     | -               | -                                     | **TODO:** rename to getNumOPP?                                                                              |
| `getIslandType`                  | -               | -                                     | -                                                                                                           |
| `getProcessors`                  | -               | -                                     | -                                                                                                           |
| `getVoltage`                     | -               | -                                     | -                                                                                                           |
| `getFrequency`                   | -               | -                                     | -                                                                                                           |
| `getFrequency(size_t opp_index)` | -               | -                                     | -                                                                                                           |
| `getOPP`                         | -               | `CPUIsland::getOPPIndex`              | -                                                                                                           |
| `setOPP`                         | **Break?**      | -                                     | Removed notification system to linked `EnergyMRTKernel`. **TODO:** check if this system is actually needed. |
| `getHigherOPPs`                  | Equivalent?     | -                                     | Changed implementation to a more efficient one, to be checked.                                              |
| `isBusy`                         | -               | `CPUIsland::busy`                     | -                                                                                                           |
| `getOPPByFrequency`              | -               | `CPUIsland::getOPPIndexByFrequency`   | -                                                                                                           |
| `getMinOPP`                      | **Removed**     | -                                     | Simply call `CPUIsland::getOPP(0)`.                                                                         |
| `getStructOPP`                   | -               | `CPUIsland::getOPP`                   | **NOTE:** Dangerous switch with `Island_BL::getOPP`! Careful!                                               |
| `getStructOPP(size_t opp_index)` | -               | `CPUIsland::getOPP(size_t opp_index)` | **NOTE:** Dangerous switch with `Island_BL::getOPP`! Careful!                                               |
| `getOPPindex(OPP opp)`           | -               | `CPUIsland::getOPPIndexByOPP`         | -                                                                                                           |
| `static buildOPPs`               | **Removed**     |                                       | Use the new (efficient) static methods provided by the `OPP` structure.                                     |
| `toString`                       | **Break?**      |                                       | Changed output of string conversion.                                                                        |
| `getFrequencySwitching`          | **New!**        | `CPUIsland::getFrequencySwitches`     | Previous implementation was just a placeholder.                                                             |
| `getCurrentPowerConsumption`     | **New!**        | `CPUIsland::getPower`                 | Previous implementation was just a placeholder.                                                             |
| `setMaxPowerConsumption`         | **Removed**     | -                                     | Already deprecated in old implementation.                                                                   |
| `getMaxConsumption`              | **Removed**     | -                                     | Already deprecated in old implementation.                                                                   |
| `check`                          | **Removed**     | -                                     | Already deprecated in old implementation.                                                                   |
| `newRun`                         | -               | -                                     | -                                                                                                           |
| `endRun`                         | -               | -                                     | -                                                                                                           |

### New `CPUIsland` Class and New `CPU` Class

Methods and behaviors of the new classes are documented in details in the code.
You can also generate Doxygen from the header files.


# What do to now

Time to migrate the rest of RTSim to use the changed classes. This section will
contain a checklist to use to convert the master branch and other branches to
this new implementation in the future.

Status of master RTSim branch conversion to the new `CPU`/`CPUIsland`
implementation will be updated as work progresses in this file.

***TODO:*** Actually create the list from the previous tables.
