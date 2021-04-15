#include "cpu_new.hpp"
#include "island.hpp"

namespace RTSim {
    CPU::CPU(const std::string &name, CPUModel *pm) :
        MetaSim::Entity(name),
        _index(0),         // TODO:
        _workload("idle"), // TODO:
        _powermodel(pm),
        _island(nullptr),     // TODO:
        _power_saving(false), // TODO: ???
        _max_power_consumption(0),
        _frequency_switch_counter(0) {

        size_t num_opps = 1;

        if (num_opps < 1) {
            _power_saving = false;
            return;
        }

        // So all this stuff works only if you link a CPU with an Island
        // Skipping to build the OPPs list, it is kept by the Island
        // Basic implementation requires at least a minimal CPUModel

        if (pm == nullptr) {
            // I don't like this at all
            pm = new CPUModelMinimal();
            // TODO:
            // pm->setFrequencyMax(somevalue);
        }

        _powermodel = pm;
    }

    std::string CPU::toString() const {
        auto freq = _island->currentOPP().frequency;
        return getName() + " freq " + std::to_string(freq);
    }
} // namespace RTSim
