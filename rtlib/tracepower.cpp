#include <tracepower.hpp>

namespace RTSim {

    TracePowerConsumption::TracePowerConsumption(CPU *c, Tick period,
                                                 const string &filename) :
        PeriodicTimer(period),
        TraceAscii(filename),
        counter(0),
        totalPowerSaved(0),
        totalPowerConsumed(0) {
        cpu = c;
    }

    TracePowerConsumption::~TracePowerConsumption() {
        cpu = NULL;
    }

    long double TracePowerConsumption::getAveragePowerSaving() {
        long double TPS = (totalPowerSaved) / ((long double) counter);
        if (counter > 0)
            return TPS;
        return 0;
    }

    long double TracePowerConsumption::getAveragePowerConsumption() {
        long double TPC = (totalPowerConsumed) / ((long double) counter);
        if (counter > 0)
            return TPC / (cpu->getPowerMax());
        return 0;
    }

    void TracePowerConsumption::action() {
        /* It periodically updates the variables: */
        double currentPowerConsumption = cpu->getPowerMax();
        totalPowerConsumed += currentPowerConsumption;
        counter++;

        long double TPC = getAveragePowerConsumption();
        // record("Average Normalized Power Consumption:");
        // record(TPC);
        record("Current Power Consumption:");
        record(currentPowerConsumption);
    }

} // namespace RTSim