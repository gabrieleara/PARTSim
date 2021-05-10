
#include <algorithm>
#include <sstream>

#include <consts.hpp>
#include <system_descriptor.hpp>

namespace RTSim {
    // Generic converter from string to any type (that supports std::streams)
    template <typename T>
    T from_str(const std::string &str) {
        std::istringstream ss(str);
        T num;
        ss >> num;
        return num;
    }

    // Base template, use one of its full specializations below
    template <class T>
    static T createFrom(yaml::Object_ptr ptr);

    // TODO: more parameters to choose from, including kernel type and so on
    template <>
    KernelDescriptor createFrom(yaml::Object_ptr ptr) {
        KernelDescriptor kd;
        kd.name = ptr->get(ATTR_NAME)->get();
        kd.scheduler_type = ptr->get(ATTR_SCHEDULER)->get();
        return kd;
    }

    template <>
    CPUMDescriptor createFrom(yaml::Object_ptr ptr) {
        CPUMDescriptor pm;

        pm.name = ptr->get(ATTR_NAME)->get();
        pm.type = ptr->get(ATTR_TYPE)->get();

        // Load parameters from csv table
        const std::string fname = ptr->get(ATTR_FILENAME)->get();

        if (fname.length() > 0) {
            // TODO: from csv file
            csv::CSVDocument doc(fname);

            const size_t num_rows = doc.rows();

            for (size_t rix = 0; rix < num_rows; ++rix) {
                // Skip rows that do not match
                if (doc.at(FIELD_MODEL, rix) == pm.name) {
                    pm.params.push_back(
                        createCPUModelParams(pm.type, doc, rix));
                }
            }
        } else {
            for (auto param : *ptr->get(ATTR_PARAMS)) {
                pm.params.push_back(createCPUModelParams(pm.type, param));
            }
        }

        return pm;
    }

    template <>
    IslandDescriptor createFrom(yaml::Object_ptr ptr) {
        IslandDescriptor island;

        island.name = ptr->get(ATTR_NAME)->get();
        island.power_model = ptr->get(ATTR_POWER_MODEL)->get();
        island.numcpus = from_str<size_t>(ptr->get(ATTR_NUMCPUS)->get());
        island.kernel = createFrom<KernelDescriptor>(ptr->get(ATTR_KERNEL));

        for (auto v : *ptr->get(ATTR_VOLTS)) {
            island.volts.push_back(from_str<volt_type>(v->get()));
        }
        std::sort(island.volts.begin(), island.volts.end());

        for (auto f : *ptr->get(ATTR_FREQS)) {
            island.freqs.push_back(from_str<freq_type>(f->get()));
        }
        std::sort(island.freqs.begin(), island.freqs.end());

        return island;
    }

    SystemDescriptor::SystemDescriptor(std::string fname) {
        yaml::Object_ptr descriptor_obj = yaml::parse(fname);

        for (auto i : *descriptor_obj->get(ATTR_CPU_ISLANDS)) {
            islands.push_back(createFrom<IslandDescriptor>(i));
        }

        for (auto pm : *descriptor_obj->get(ATTR_POWER_MODELS)) {
            auto pmd = createFrom<CPUMDescriptor>(pm);
            power_models.emplace(pmd.name, std::move(pmd));
        }
    }
} // namespace RTSim
