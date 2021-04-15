#ifndef __RTSIM_CPU_ISLAND__
#define __RTSIM_CPU_ISLAND__

#include <vector>

#include "class_utils.hpp"
#include "entity.hpp"
#include "memory.hpp"
#include "powermodel.hpp"

#include "opp.hpp"

namespace RTSim {
    class CPU;

    class Island : public MetaSim::Entity {
    public:
        using size_type = size_t;
        using OPPs_list_type = std::vector<OPP>;
        using CPUs_list_type = std::vector<CPU *>;

        // =================================================
        // Static
        // =================================================

        /// Constructs a list of OPPs from two (equally sized) vectors of
        /// parameters
        /// @returns the constructed list
        static OPPs_list_type
        buildOPPs(const std::vector<freq_type> &freqs = {},
                  const std::vector<volt_type> &volts = {});

        // =================================================
        // Constructors
        // =================================================
    public:
        Island(const string &name, const CPUs_list_type &cpus = {},
               const OPPs_list_type &opps = {});

        DEFAULT_VIRTUAL_DES(Island);

        // =================================================
        // Methods
        // =================================================
    public:
        const OPPs_list_type &opps() const {
            return _opps;
        }

        const CPUs_list_type &cpus() const {
            return _cpus;
        }

        size_type getOPPIndex() const {
            return _currentOPP;
        }

        const OPP &getOPP(size_type index) const {
            return _opps.at(index);
        }

        const OPP &getOPP() const {
            return _opps.at(getOPPIndex());
        }

        using const_opps_iterator = OPPs_list_type::const_iterator;
        // using const_opps_slice =
        // std::pair<const_opps_iterator, const_opps_iterator>;

        const_opps_iterator higherThanCurrent() const {
            return _opps.begin() + _currentOPP + 1;
        }

        void setOPP(size_type opp_index);

        void setOPP(const OPP &opp);

        /// @returns true if at least one CPU in this Island is busy
        bool isBusy() const;

        size_type getOPPIndex(const OPP &opp) const;

        size_type getOPPIndexByFrequency(freq_type frequency) const;

        const OPP &getOPPByFrequency(freq_type frequency) const {
            return _opps.at(getOPPIndexByFrequency(frequency));
        }

        size_type getMinOPPIndex() const {
            // Assumes sorted OPP list
            return 0;
        }

        size_type getMaxOPPIndex() const {
            // Assumes sorted OPP list
            size_type size = _opps.size();
            if (size == 0)
                return 0;
            return size - 1;
        }

        /// @note calling getMinOPP() on an Island with an empty list of OPPs is
        /// undefined
        const OPP &getMinOPP() const {
            // NOTICE: assumes sorted list
            return _opps.front();
        }

        /// @note calling getMaxOPP() on an Island with an empty list of OPPs is
        /// undefined
        const OPP &getMaxOPP() const {
            // NOTICE: assumes sorted list
            return _opps.back();
        }

        virtual void newRun() override;
        virtual void endRun() override;

        // =================================================
        // Data
        // =================================================
    private:
        OPPs_list_type _opps;
        const CPUs_list_type _cpus;

        size_type _currentOPP;
    };
} // namespace RTSim

// void function() {
//     RTSim::Island i{"big"};

//     auto begin = i.higherThanCurrentOPPsBegin();
// }

#endif // __RTSIM_CPU_ISLAND__
