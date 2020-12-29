/***************************************************************************
begin                : Thu Jul 14:23:58 CEST 2018
copyright            : (C) 2018 by Luigi Pannocchi
email                : l.pannocchi@gmail.com
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef __POWERMODEL_HPP__
#define __POWERMODEL_HPP__

#include <cmath>
#include <cstdlib>
#include <istream>
#include <limits>
#include <map>
#include <string>

#include "class_utils.hpp"
#include "cloneable.hpp"
#include "sortedcont.hpp"
#include <memory>

namespace RTSim {
    // Forward declarations
    class CPU;
    class CPUPowerModelDescriptor;

    using watt_type = double;       /// Represents power in Watts
    using volt_type = double;       /// Represents voltage in Volts
    using freq_type = unsigned int; /// Represents frequency in MHz
    using speed_type = double;      /// Represents speedup ratio

    constexpr freq_type FREQ_MAX = std::numeric_limits<freq_type>::max();

    class CPUModel {
        // =================================================
        // Static
        // =================================================
    public:
        /// Factory method
        /// @todo add proper description
        ///
        /// @param key the key that identifies the type of CPUModel to be
        /// instantiated
        /// @param desc the descriptor of the CPUModel, used to set all its
        /// parameters before returning it to the user
        /// @param v the initial voltage of the CPU (in Volts)
        /// @param f the initial frequency of the CPU (in MHz)
        /// @param f_max the maximum frequency of the CPU (in MHz)
        ///
        /// @returns a new CPUModel (wrapped in a std::unique_ptr) generated
        /// from the given description and initialized by forwarding the given
        /// parameters to the model constructor.
        ///
        /// @todo Placeholder, until the integration with the GenericFactory is
        /// ready use this method.
        static std::unique_ptr<CPUModel>
        create(const std::string &key, const CPUPowerModelDescriptor &desc,
               volt_type v = 0, freq_type f = 0, freq_type f_max = FREQ_MAX);

        // =================================================
        // Constructors and destructors
        // =================================================
    public:
        /// @todo Add a proper description
        /// @param v     the initial voltage of the CPU (in Volts)
        /// @param f     the initial frequency of the CPU (in MHz)
        /// @param f_max the maximum frequency of the CPU (in MHz)
        CPUModel(watt_type v = 0, freq_type f = 0, freq_type f_max = FREQ_MAX) :
            _V(v), _F(f), _F_max(f_max) {}

        DEFAULT_COPIABLE(CPUModel);

        /// @returns a new instance of this class obtained by copy construction,
        /// wrapped in a std::unique_ptr
        BASE_CLONEABLE(CPUModel);

        /// Requires a virtual destructor, but adopting default compiler
        /// behavior
        DEFAULT_VIRTUAL_DES(CPUModel);

        // =================================================
        // Methods
        // =================================================
    public:
        /// Links this CPUModel to the given CPU
        virtual void setCPU(CPU *c) {
            _cpu = c;
        }

        /// @returns a bare pointer the CPU linked to this CPUModel
        CPU *getCPU() const {
            return _cpu;
        }

        // TODO: update power both times? Why not use a single method?

        /// Set the current voltage of the CPU (in Volts)
        virtual void setVoltage(volt_type v) {
            setOPP(_F, v);
        }

        /// Set the current frequency of the CPU (in MHz)
        ///
        /// @note Any value of @p f greater than the maximum allowed frequency
        /// will be capped to the maximum frequency value.
        virtual void setFrequency(freq_type f) {
            setOPP(f, _V);
        }

        /// Set the current Operating Performance Point (OPP)
        ///
        /// @note Any value of @p f greater than the maximum allowed frequency
        /// will be capped to the maximum frequency value.
        ///
        /// @param f the new frequency (in MHz)
        /// @param v the new voltage (in Volts)
        virtual void setOPP(freq_type f, volt_type v) {
            _F = f;
            _V = v;

            if (_F > _F_max)
                _F = _F_max;

            updatePowerSpeed();
        }

        /// Set the maximum frequency that can be set to this model.
        /// Any frequency supplied greater than the maximum one will be
        /// implicitly capped implicitly when using setFrequency/setOPP methods.
        ///
        /// @note The maximum frequency value is used as base frequency to
        /// calculate the speedup of a task when the OPP changes.
        ///
        /// @note Setting a maximum value that is lower than the current
        /// frequency will implicitly trigger an OPP change for the linked CPU
        /// (basically the current frequency will be capped to the new maximum
        /// value).
        ///
        /// @param f_max the new maximum frequency to be used from now on
        virtual void setFrequencyMax(freq_type f_max) {
            _F_max = f_max;
            setOPP(_F, _V);
        }

        /// @returns the current frequency of the CPU in MHz
        virtual freq_type getFrequency() const {
            return _F; // / 1000;
        }

        /// @returns the current voltage of the CPU in Volts
        virtual volt_type getVoltage() const {
            return _V; // / 1000;
        }

        /// @todo
        virtual std::pair<freq_type, volt_type> getOPP() {
            return std::make_pair<freq_type, volt_type>(getFrequency(),
                                                        getVoltage());
        }

        /// Checks what would be the power consumption when the triple given as
        /// input is selected for the current cpu.
        ///
        /// @param wname the workload class of the task
        /// @param v desired volage (in Volts)
        /// @param f desired frequency (in MHz)
        ///
        /// @returns the forshadowed power
        virtual watt_type lookupPower(const std::string &wname, freq_type f,
                                      volt_type v) const = 0;

        /// Checks what would be the task speedup when the triple given as input
        /// is selected for the current cpu.
        ///
        /// @param wname the workload class of the task
        /// @param v desired volage (in Volts)
        /// @param f desired frequency (in MHz)
        ///
        /// @returns the forshadowed speedup
        virtual speed_type lookupSpeed(const std::string &wname, freq_type f,
                                       volt_type v = 0) const = 0;

        /// Simple getter
        ///
        /// @note The value returned from this function is valid only if the
        /// system is in a "valid" state (i.e. the OPP set on this model is one
        /// acceptable for the linked CPU). On some implementations of this
        /// class, this method may work after simly calling setFrequency(), but
        /// it is advisable to call always both setFrequency() and setVoltage()
        /// (or even better, setOPP()) to ensure the system is in a consistent
        /// state.
        ///
        /// @returns the current power consumption of the linked CPU
        virtual watt_type getPower() {
            updatePower(); // TODO: remove
            return _P;
        }

        /// Simple getter
        ///
        /// @note The value returned from this function is valid only if the
        /// system is in a "valid" state (i.e. the OPP set on this model is one
        /// acceptable for the linked CPU). On some implementations of this
        /// class, this method may work after simly calling setFrequency(), but
        /// it is advisable to call always both setFrequency() and setVoltage()
        /// (or even better, setOPP()) to ensure the system is in a consistent
        /// state.
        ///
        /// @returns the current speedup of the task running on the linked CPU
        virtual long double getSpeed() {
            updateSpeed(); // TODO: remove
            return _S;
        }

    protected:
        /// Updates the value of power consumption using current task
        /// parameters, retrieved from the linked CPU.
        ///
        /// @deprecated Do not call manually. Called automatically by
        /// updateVoltage(), updateFrequency(), or updateOPP().
        virtual void updatePower();

        /// Updates the speedup of the current task under current working
        /// conditions.
        ///
        /// @deprecated Do not call manually. Called automatically by
        /// updateVoltage() and updateFrequency().
        virtual void updateSpeed();

        /// Updates both power consumption and speedup under current working
        /// conditions.
        ///
        /// @deprecated Do not call manually. Called automatically by
        /// updateVoltage(), updateFrequency(), or updateOPP().
        virtual void updatePowerSpeed() {
            updatePower();
            updateSpeed();
        }

        // =================================================
        // Data
        // =================================================
    protected:
        // Following values correspond to current task running on the linked CPU

        /// Voltage supply to the processor (Volts)
        volt_type _V;

        /// Frequency of the processor (MHz)
        freq_type _F;

        /// Maximum frequency that can be set to the given processor (Hz)
        freq_type _F_max = FREQ_MAX;

        /// Current power consumption (Watt)
        watt_type _P;

        /// Current speedup ratio
        speed_type _S;

    private:
        CPU *_cpu = nullptr;
    };

    class CPUModelMinimal : public CPUModel {
    public:
        CPUModelMinimal(volt_type v = 0, freq_type f = 0,
                        freq_type f_max = FREQ_MAX) :
            CPUModel(v, f, f_max){};

        /// @returns a new instance of this class obtained by copy construction,
        /// wrapped in a std::unique_ptr
        CLONEABLE(CPUModel, CPUModelMinimal, override);

    public:
        /// Checks what would be the power consumption when the triple given as
        /// input is selected for the current cpu.
        ///
        /// @param wname the workload class of the task
        /// @param v desired volage (in Volts)
        /// @param f desired frequency (in MHz)
        ///
        /// @returns the forshadowed power
        virtual watt_type lookupPower(const std::string &wname, freq_type f,
                                      volt_type v) const override;

        /// Checks what would be the task speedup when the triple given as input
        /// is selected for the current cpu.
        ///
        /// @param wname the workload class of the task
        /// @param v desired volage (in Volts)
        /// @param f desired frequency (in MHz)
        ///
        /// @returns the forshadowed speedup
        virtual speed_type lookupSpeed(const std::string &wname, freq_type f,
                                       volt_type v = 0) const override;
    };

    class CPUModelBP : public CPUModel {
        // =================================================
        // Model parameters
        // =================================================
    public:
        struct PowerModelBPParams {
            /// Constant "displacement"
            double d;

            /// Constant "eta"
            /// Factor modeling the P_short = eta * P_charge
            double e;

            /// Constant "gamma"
            /// Factor modeling the Temperature effect on
            /// P_leak = gamma * V * P_dyn
            double g;

            /// Constant "K"
            /// Factor modeling the percentage of CPU activity
            double k;
        };

        /// @todo do we really need long double after all?
        struct SpeedModelBPParams {
            /// @todo document this
            long double a;
            /// @todo document this
            long double b;
            /// @todo document this
            long double c;
            /// @todo document this
            long double d;
        };

        // =================================================
        // Constructors and destructors
        // =================================================
    public:
        CPUModelBP(volt_type v = 0, freq_type f = 0,
                   freq_type f_max = FREQ_MAX) :
            CPUModel(v, f, f_max) {}

        CLONEABLE(CPUModel, CPUModelBP, override);

        // =================================================
        // Methods
        // =================================================
    public:
        /// Sets both power and speedup model parameters for the given workload
        /// @param workload_name the name of the workload; if it does not exist
        /// yet, it is added to the set of supported workload types
        /// @param power_params the parameters used to calculate power
        /// consumption for the given workload
        /// @param speed_params the parameters used to calculate task speedup
        /// for the given workload
        virtual void setWorkloadParams(const std::string &workload_name,
                                       const PowerModelBPParams &power_params,
                                       const SpeedModelBPParams &speed_params);

        /// Checks what would be the power consumption when the triple given as
        /// input is selected for the current cpu.
        ///
        /// @param wname the workload class of the task
        /// @param v desired volage (in Volts)
        /// @param f desired frequency (in MHz)
        ///
        /// @returns the forshadowed power
        virtual watt_type lookupPower(const std::string &wname, freq_type f,
                                      volt_type v) const override;

        /// Checks what would be the task speedup when the triple given as input
        /// is selected for the current cpu.
        ///
        /// @param wname the workload class of the task
        /// @param v desired volage (in Volts)
        /// @param f desired frequency (in MHz)
        ///
        /// @returns the forshadowed speedup
        virtual speed_type lookupSpeed(const std::string &wname, freq_type f,
                                       volt_type v = 0) const override;

        // =================================================
        // Data
        // =================================================
    protected:
        /// @todo
        std::map<std::string, PowerModelBPParams> _power_params;
        /// @todo
        std::map<std::string, SpeedModelBPParams> _speed_params;

        /// Current power consumption due to leakage effects
        double _P_leak;

        /// Current power consumption due to the transistors switching
        double _P_dyn;

        /// Current value of the part of the dynamic power consumption due to
        /// short circuit effect during the switching
        double _P_short;

        /// Current value of the part of the dynamic power consumption due to
        /// the charging of the gate capacitors
        double _P_charge;
    };

    /// Implementation of a table-based CPU Power Model.
    /// Uses an internal lookup table built using the setWorkloadParams() to
    /// estimate the power consumption of the target CPU.
    /// @todo re-implement efficiently not using maps of maps!
    class CPUModelTB : public CPUModel {
        // =================================================
        // Model parameters
        // =================================================
    public:
        struct TBParamsIn {
            freq_type freq;
            volt_type volt;

            friend bool operator<(TBParamsIn const &lhs,
                                  TBParamsIn const &rhs) {
                if (lhs.freq < rhs.freq)
                    return true;
                if (lhs.freq > rhs.freq)
                    return false;
                return lhs.volt < rhs.volt;
            }
        };

        struct TBParamsOut {
            watt_type power;
            speed_type speedup;
        };

        // =================================================
        // Constructors and destructors
        // =================================================
    public:
        /// Constructs a new CPUModelTB.
        /// @note Some constructors for this class may throw exceptions!
        /// @todo Document properly exceptions
        CPUModelTB(volt_type v = 0, freq_type f = 0,
                   freq_type f_max = FREQ_MAX) :
            CPUModel(v, f, f_max) {}

        /// @returns a new instance of this class obtained by copy construction,
        /// wrapped in a std::unique_ptr
        CLONEABLE(CPUModel, CPUModelTB, override);

        // =================================================
        // Methods
        // =================================================
    public:
        /// Adds the given values to the lookup table.
        /// In particular, it creates (or substitutes) two associations:
        ///   - (wname, frequency, voltage) -> power;
        ///   - (wname, frequency, voltage) -> speedup.
        /// Both associations can then be used from this point onwards to look
        /// up what the power consumption of the CPU or the speedup of the task
        /// would be if the triple (wname, frequency, voltage) is selected for
        /// execution.
        ///
        /// @param wname the workload class name
        /// @param params the pair of voltage/frequency values
        /// @param power the power estimation
        /// @param speedup the speedup estimation
        ///
        /// @todo use OPPs
        virtual void setWorkloadParams(const std::string &wname,
                                       const TBParamsIn &params,
                                       watt_type power, speed_type speedup);

        /// Checks what would be the power consumption when the triple given as
        /// input is selected for the current cpu.
        ///
        /// @param wname the workload class of the task
        /// @param v desired volage (in Volts)
        /// @param f desired frequency (in MHz)
        ///
        /// @returns the forshadowed power
        virtual watt_type lookupPower(const std::string &wname, freq_type f,
                                      volt_type v) const override;

        /// Checks what would be the task speedup when the triple given as input
        /// is selected for the current cpu.
        ///
        /// @param wname the workload class of the task
        /// @param v desired volage (in Volts)
        /// @param f desired frequency (in MHz)
        ///
        /// @returns the forshadowed speedup
        virtual speed_type lookupSpeed(const std::string &wname, freq_type f,
                                       volt_type v = 0) const override;

    protected:
        using smap = sorted_map<TBParamsIn, TBParamsOut>;
        using map_map = std::map<std::string, smap>;

    protected:
        map_map::const_iterator
        find_suitable_map(const std::string &wname) const {
            constexpr char fallbacks[2][5] = {"busy", "idle"};

#define FIND_RETURN(res, key)                                                  \
    ({                                                                         \
        res = _map.find(key);                                                  \
        if (res != _map.cend())                                                \
            return res;                                                        \
    })

            map_map::const_iterator res;
            FIND_RETURN(res, wname);
            for (auto key : fallbacks) {
                FIND_RETURN(res, key);
            }

#undef FIND_RETURN

            return _map.cend();
        }

        // =================================================
        // Data
        // =================================================

    protected:
        map_map _map;
    };

    class CPUModelTBApproximate : public CPUModelTB {
        // =================================================
        // Methods
        // =================================================
    public:
        /// Checks what would be the power consumption when the triple given as
        /// input is selected for the current cpu.
        ///
        /// @param wname the workload class of the task
        /// @param v desired volage (in Volts)
        /// @param f desired frequency (in MHz)
        ///
        /// @returns the forshadowed power
        virtual watt_type lookupPower(const std::string &wname, freq_type f,
                                      volt_type v) const override;

        /// Checks what would be the task speedup when the triple given as input
        /// is selected for the current cpu.
        ///
        /// @param wname the workload class of the task
        /// @param v desired volage (in Volts)
        /// @param f desired frequency (in MHz)
        ///
        /// @returns the forshadowed speedup
        virtual speed_type lookupSpeed(const std::string &wname, freq_type f,
                                       volt_type v = 0) const override;
    };
} // namespace RTSim

#endif
