// #include "yaml.hpp"

#include <array>
#include <string>
#include <vector>

#include "class_utils.hpp"
#include "csv.hpp"
#include "powermodel.hpp"
#include "yaml.hpp"
#include <memory>
// TODO: change slightly the factory.hpp and then use it for these objects

namespace RTSim {

    // Base class, empty
    class CPUModelParams {
    public:
        using key_type = std::string;

    public:
        CPUModelParams(){};
        DEFAULT_COPIABLE(CPUModelParams);
        DEFAULT_MOVABLE(CPUModelParams);
        DEFAULT_VIRTUAL_DES(CPUModelParams);
    };

    // Parameter class for CPUModelMinimal
    class CPUModelMinimalParams : public CPUModelParams {
    public:
        static const key_type key;

    public:
        CPUModelMinimalParams() = default;
        DEFAULT_COPIABLE(CPUModelMinimalParams);
        DEFAULT_MOVABLE(CPUModelMinimalParams);
        DEFAULT_VIRTUAL_DES(CPUModelMinimalParams);
    };

    // Parameter class for CPUModelBP
    class CPUModelBPParams : public CPUModelParams {
    public:
        static const key_type key;

    public:
        struct PowerModelParams {
            double d;
            double e;
            double g;
            double k;
        };

        struct SpeedModelParams {
            double a;
            double b;
            double c;
            double d;
        };

    public:
        std::string workload;
        PowerModelParams power_params;
        SpeedModelParams speed_params;

    public:
        CPUModelBPParams() = default;
        DEFAULT_COPIABLE(CPUModelBPParams);
        DEFAULT_MOVABLE(CPUModelBPParams);
        DEFAULT_VIRTUAL_DES(CPUModelBPParams);

    public:
        CPUModelBPParams(const std::string &workload,
                         const PowerModelParams &pp,
                         const SpeedModelParams &sp) :
            workload(workload), power_params{pp}, speed_params{sp} {}
    };

    // Parameter class for CPUModelTB
    class CPUModelTBParams : public CPUModelParams {
    public:
        static const key_type key;

    public:
        std::string workload;
        freq_type freq;
        volt_type volt;
        watt_type power;
        speed_type speed;

    public:
        CPUModelTBParams() = default;
        DEFAULT_COPIABLE(CPUModelTBParams);
        DEFAULT_MOVABLE(CPUModelTBParams);
        DEFAULT_VIRTUAL_DES(CPUModelTBParams);
    };

    extern std::unique_ptr<CPUModelParams>
    createCPUModelParams(CPUModelParams::key_type k, csv::CSVDocument &doc,
                         size_t rix);

    extern std::unique_ptr<CPUModelParams>
    createCPUModelParams(CPUModelParams::key_type k, yaml::Object_ptr ptr);

} // namespace RTSim
