#ifndef __RTSIM_OPP_H__
#define __RTSIM_OPP_H__

namespace RTSim {
    // TODO: move these somewhere else
    using freq_type = unsigned int;
    using volt_type = double;

    struct OPP {
        /// Frequency of each step (in MHz)
        freq_type frequency;

        /// Voltage of each step (in Volts)
        volt_type voltage;

        /// This constructor is needed by the struct to properly use
        /// emplace_back
        OPP(freq_type f = {}, volt_type v = {}) : frequency(f), voltage(v) {}
    };

    // =====================================================
    // Operators
    // =====================================================

    static inline bool operator_less_freq(const OPP &lhs, const OPP &rhs) {
        return lhs.frequency < rhs.frequency;
    }

    static inline bool operator<(const OPP &lhs, const OPP &rhs) {
        if (operator_less_freq(lhs, rhs))
            return true;
        if (operator_less_freq(rhs, lhs))
            return false;
        return lhs.voltage < rhs.voltage;
    }

    static inline bool operator>(const OPP &lhs, const OPP &rhs) {
        return rhs < lhs;
    }

    static inline bool operator==(const OPP &lhs, const OPP &rhs) {
        return lhs.frequency == rhs.frequency && lhs.voltage == lhs.voltage;
    }

    static inline bool operator!=(const OPP &lhs, const OPP &rhs) {
        return !(lhs == rhs);
    }
} // namespace RTSim

#endif // __RTSIM_OPP_H__
