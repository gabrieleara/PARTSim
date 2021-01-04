#ifndef __RTSIM_CONSTS_H__
#define __RTSIM_CONSTS_H__

namespace RTSim {
    // =====================================================
    // YAML File Attributes
    // =====================================================
    static constexpr char ATTR_NAME[] = "name";
    static constexpr char ATTR_CPU_ISLANDS[] = "cpu_islands";
    static constexpr char ATTR_NUMCPUS[] = "numcpus";
    static constexpr char ATTR_KERNEL[] = "kernel";
    static constexpr char ATTR_SCHEDULER[] = "scheduler";
    static constexpr char ATTR_VOLTS[] = "volts";
    static constexpr char ATTR_FREQS[] = "freqs";
    static constexpr char ATTR_POWER_MODEL[] = "power_model";
    static constexpr char ATTR_POWER_MODELS[] = "power_models";
    static constexpr char ATTR_TYPE[] = "type";
    static constexpr char ATTR_FILENAME[] = "filename";
    static constexpr char ATTR_PARAMS[] = "params";
    static constexpr char ATTR_WORKLOAD[] = "workload";
    static constexpr char ATTR_POWER_PARAMS[] = "power_params";
    static constexpr char ATTR_SPEED_PARAMS[] = "speed_params";

    // =====================================================
    // CSV File Fields
    // =====================================================
    static constexpr char FIELD_POWER_D[] = "power_d";
    static constexpr char FIELD_POWER_E[] = "power_e";
    static constexpr char FIELD_POWER_G[] = "power_g";
    static constexpr char FIELD_POWER_K[] = "power_k";
    static constexpr char FIELD_SPEED_A[] = "speed_a";
    static constexpr char FIELD_SPEED_B[] = "speed_b";
    static constexpr char FIELD_SPEED_C[] = "speed_c";
    static constexpr char FIELD_SPEED_D[] = "speed_d";
    static constexpr char FIELD_FREQ[] = "freq";
    static constexpr char FIELD_VOLT[] = "voltage";
    static constexpr char FIELD_POWER[] = "power";
    static constexpr char FIELD_SPEED[] = "speed";
    static constexpr char FIELD_MODEL[] = "model";
    static constexpr char FIELD_WORKLOAD[] = "workload";
} // namespace RTSim

#endif // __RTSIM_CONSTS_H__
