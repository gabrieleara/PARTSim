#ifndef __CPU_HPP__
#define __CPU_HPP__

#include "entity.hpp"
#include "island.hpp"
#include "opp.hpp"
#include "powermodel.hpp"

namespace RTSim {
    // Forward declaration
    class Island;

    class CPU : public MetaSim::Entity {
        // =================================================
        // Constructors
        // =================================================
    public:
        CPU(const std::string &name = "", CPUModel *pm = nullptr);

        DEFAULT_VIRTUAL_DES(CPU);

        // =================================================
        // Methods
        // =================================================
    public:
        void setIsland(Island *i) {
            _island = i;
            // TODO:
        }

        const Island *getIsland() const {
            return _island;
        }

        Island *getIsland() {
            return _island;
        }

        void setIndex(size_t i) {
            _index = i;
        }

        size_t getIndex() const {
            return _index;
        }

        virtual std::string toString() const;

        /// @todo Old implementation stated that this should return the maximum
        /// power consumption value obtainable with this CPU, is this a static
        /// property of the model?
        watt_type getMaxPowerConsumption() const {
            return _max_power_consumption;
        }

        /// @todo Old implementation stated that this should return the maximum
        /// power consumption value obtainable with this CPU, is this a static
        /// property of the model?
        void setMaxPowerConsumption(watt_type max_p) {
            _max_power_consumption = max_p;
        }

        /// Returns the current power consumption of the CPU.
        /// Normalized power consumption can be obtained dividing this value by
        /// the one returned by getMaxPowerConsumption().
        watt_type getCurrentPowerConsumption() const {
            _powermodel->getPower();
        }

        /// Returns the current CPU speed (between 0 and 1)
        speed_type getSpeed() const {
            _powermodel->getSpeed();
        }

        speed_type getPowerByOPP(const OPP &opp) const {
            return _powermodel->lookupPower(_workload, opp);
        }

        speed_type getSpeedByOPP(const OPP &opp) const {
            return _powermodel->lookupSpeed(_workload, opp);
        }

        speed_type getPowerByOPP(size_t opp_index) const {
            return getPowerByOPP(_island->getOPP(opp_index));
        }

        speed_type getSpeedByOPP(size_t opp_index) const {
            return getSpeedByOPP(_island->getOPP(opp_index));
        }

        /// Returns the current power saving of the CPU.
        /// @todo is this implementation correct?
        watt_type getCurrentPowerSaving() const {
            return getMaxPowerConsumption() - getCurrentPowerConsumption();
        }

        /// Sets a new speed for the CPU accordingly to the system
        /// load. Returns the new speed.
        /// @todo what is this thing?
        speed_type setSpeed(double newLoad);

        /// Set the computation workload on the cpu
        /// @todo propagate
        void setWorkload(const string &workload) {
            _workload = workload;
        }

        string getWorkload() const {
            return _workload;
        }

        unsigned long int getFrequencySwitching() {
            return _frequency_switch_counter;
        }

        virtual void newRun() {}
        virtual void endRun() {}

        /// Useful for debug
        virtual void check();

        // =================================================
        // Forwarding Methods
        // =================================================
        const OPP &currentOPP() const {
            return _island->getOPP();
        }

        size_t currentOPPIndex() const {
            // TODO: all these checks are useless if _island is always
            // guaranteed to be here, I'll put only one here and eventually
            // propagate the checks/remove them all depending on final
            // implementation
            // IDEA: maybe the Island itself could instantiate all CPUs (instead
            // of accepting a list as input)!
            if (_island == nullptr)
                return 0;
            return _island->getOPPIndex();
        }

        freq_type getFrequency() const {
            return currentOPP().frequency;
        }

        volt_type getVoltage() const {
            return currentOPP().voltage;
        }

        /// Useful for debug
        void setOPP(size_t opp_index) {
            // TODO: either here or there, no circular stuff!
            _island->setOPP(opp_index);
            _powermodel->setOPP(_island->getOPP());
        }

        virtual freq_type getFrequency() const;

        // =================================================
        // Data
        // =================================================
    private:
        /// CPU index in a multiprocessor environment
        size_t _index;

        /// Currently running workload type
        std::string _workload;

        /// Linked power model
        /// @note all CPUs in the same Island have the same type/parameters of
        /// power model, each an exact clone of the same model
        CPUModel *_powermodel;

        /// Linked Island
        Island *_island;

        /// @todo: what is this?
        bool _power_saving = false;

        /// Maximum registered power level
        watt_type _max_power_consumption = 0;

        /// Number of speed changes from the beginning of time
        unsigned long int _frequency_switch_counter = 0;
    };

    std::ostream &operator<<(std::ostream &os, CPU &c);

    bool operator==(const CPU &rhs, const CPU &lhs);

} // namespace RTSim

#endif // __CPU_HPP__
