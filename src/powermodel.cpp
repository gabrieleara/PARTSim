/***************************************************************************
    begin                : Thu Jul 21 15:54:58 CEST 2018
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
#include <cassert>
#include <cmath>

#include "cpu.hpp"
#include "csv.hpp"
#include "powermodel.hpp"
#include "system_descriptor.hpp"

namespace RTSim {
    // =====================================================
    // CPUModel
    // =====================================================

    // TODO: Fake factory, convert to actual factory
    std::unique_ptr<CPUModel>
    CPUModel::create(const std::string &k, const CPUPowerModelDescriptor &desc,
                     volt_type v, freq_type f, freq_type f_max) {
        if (k == CPUModelMinimalParams::key) {
            return std::make_unique<CPUModelMinimal>(v, f, f_max);
        }

        if (k == CPUModelBPParams::key) {
            std::unique_ptr<CPUModelBP> bpp =
                std::make_unique<CPUModelBP>(v, f, f_max);

            for (const auto &p : desc.params) {
                const auto *pp = dynamic_cast<CPUModelBPParams *>(p.get());

                const CPUModelBP::PowerModelBPParams p_params{
                    pp->power_params.d,
                    pp->power_params.e,
                    pp->power_params.g,
                    pp->power_params.k,
                };

                const CPUModelBP::SpeedModelBPParams s_params{
                    pp->speed_params.a,
                    pp->speed_params.b,
                    pp->speed_params.c,
                    pp->speed_params.d,
                };

                bpp->setWorkloadParams(pp->workload, p_params, s_params);
            }

            return bpp;
        }

        if (k == CPUModelTBParams::key) {
            std::unique_ptr<CPUModelTB> tbp =
                std::make_unique<CPUModelTB>(v, f, f_max);

            for (const auto &p : desc.params) {
                const auto *pp = dynamic_cast<CPUModelTBParams *>(p.get());

                CPUModelTB::TBParamsIn params;
                params.freq = pp->freq;
                params.volt = pp->volt;

                tbp->setWorkloadParams(pp->workload, params, pp->power,
                                       pp->speed);
            }

            return tbp;
        }

        throw std::exception{}; // TODO:
    }

    void CPUModel::updatePower() {
        watt_type p = lookupPower(getCPU()->getWorkload(), _V, _F);
        if (!std::isnan(p))
            _P = p;
    }

    void CPUModel::updateSpeed() {
        speed_type s = lookupSpeed(getCPU()->getWorkload(), _V, _F);
        if (!std::isnan(s))
            _S = s;
    }

    // =====================================================
    // CPUModelMinimal
    // =====================================================

    // TODO: why did the old implementation set the frequency to KHz internally?
    // TODO: check formulae against measurement units
    watt_type CPUModelMinimal::lookupPower(const string &, freq_type f,
                                           volt_type v) const {
        return (v * v) * f;
    }

    speed_type CPUModelMinimal::lookupSpeed(const string &, freq_type f,
                                            volt_type) const {
        return _F_max / f;
    }

    // =====================================================
    // CPUModelBP
    // =====================================================

    // TODO: remember to require always at least two workload parameters (idle
    // and busy)

    watt_type CPUModelBP::lookupPower(const std::string &workload, freq_type f,
                                      volt_type v) const {

        f *= 1000; // This model was trained using frequencies in KHz

        double K, eta, gamma, disp;

        // TODO: if needed, export these outside, for now I'll remove them from
        // class attributes
        double P_charge, P_short, P_dyn, P_leak;

        const auto &params = _power_params.at(workload);

        disp = params.d;
        K = params.k;
        eta = params.e;
        gamma = params.g;

        // Evaluation of the P_charge
        P_charge = (K)*f * (v * v);

        // Evaluation of the P_short
        P_short = eta * P_charge;

        // Evalution of the P_dyn
        P_dyn = P_short + P_charge;

        // Evaluation of P_leak
        P_leak = gamma * v * P_dyn;

        // Evaluation of the total Power
        return P_leak + P_dyn + disp;
    }

    speed_type CPUModelBP::lookupSpeed(const std::string &workload, freq_type f,
                                       volt_type) const {

        f *= 1000; // This model was trained using frequencies in KHz

        // TODO: asserts and all!
        assert(_speed_params.find(getCPU()->getWorkload()) !=
               _speed_params.end());

        const auto &params = _speed_params.at(workload);

        long double disp = params.a;
        long double ideal = params.b / static_cast<long double>(f);
        long double slope =
            params.c * std::exp(-(static_cast<long double>(f) / params.d));

        return 1.0 / (disp + ideal + slope);
    }

    void CPUModelBP::setWorkloadParams(const string &workload_name,
                                       const PowerModelBPParams &power_params,
                                       const SpeedModelBPParams &speed_params) {
        _power_params[workload_name] = power_params;
        _speed_params[workload_name] = speed_params;
    }

    // =====================================================
    // CPUModelTB
    // =====================================================

    void CPUModelTB::setWorkloadParams(const string &wname,
                                       const TBParamsIn &params,
                                       watt_type power, speed_type speedup) {
        // NOTICE: this may accept duplicates if supplied
        TBParamsOut v;
        v.power = power;
        v.speedup = speedup;

        std::pair<TBParamsIn, TBParamsOut> kv = std::make_pair(params, v);
        _map[wname].insert(kv);
    }

    // I know that these can be implemented with a lot of templates, I even
    // implemented them all, but the functions became more cryptic and I didn't
    // like them like that, result is the same

    watt_type CPUModelTB::lookupPower(const string &wname, freq_type f,
                                      volt_type v) const {
        TBParamsIn key;
        key.volt = v;
        key.freq = f;

        auto res_map = find_suitable_map(wname);
        if (res_map == _map.end())
            return 0;

        auto &map = res_map->second;

        auto res = map.find_from_key(key);
        if (res == map.cend())
            return 0;
        return res->second.power;
    }

    speed_type CPUModelTB::lookupSpeed(const string &wname, freq_type f,
                                       volt_type v) const {
        TBParamsIn key;
        key.volt = v;
        key.freq = f;

        auto res_map = find_suitable_map(wname);
        if (res_map == _map.end())
            return 0;

        auto &map = res_map->second;

        auto res = map.find_from_key(key);
        if (res == map.cend())
            return 0;
        return res->second.speedup;
    }

    // =====================================================
    // CPUModelTBApproximate
    // =====================================================

    // NOTE: exp must be positive real
    // d is a distance function (metric) between x and xi
    template <class A, class T = long double, class E = long double>
    static inline T weight(A x, A xi, T (*d)(A, A), E exp = 2.0) {
        return T(1.0) / std::pow(d(x, xi), exp);
    }

    // w is a weight function based on a distance between x and xi
    template <class A, class T = long double, class E = long double>
    static inline T interpolate(A x, A x1, A x2, T y1, T y2, T (*w)(A, A, E),
                                E exp = 2.0) {
        T sum = 0.0;

        T w1 = w(x, x1, exp);
        T w2 = w(x, x2, exp);

        sum += w1 * y1;
        sum += w2 * y2;
        return sum / (w1 + w2);
    }

    using Opp = CPUModelTB::TBParamsIn;

    static inline long double opp_power_distance(Opp x, Opp xi) {
        return std::pow(static_cast<long double>(x.volt - xi.volt), 2.0) *
               std::abs(
                   static_cast<long double>(static_cast<long double>(x.freq) -
                                            static_cast<long double>(xi.freq)));
    }

    static inline long double opp_power_weight(Opp x, Opp xi,
                                               long double exp = 2.0) {
        return weight<Opp>(x, xi, opp_power_distance, exp);
    }

    static inline long double opp_power_interpolate(Opp x, Opp x1, Opp x2,
                                                    long double y1,
                                                    long double y2) {
        return interpolate<Opp>(x, x1, x2, y1, y2, opp_power_weight);
    }

    static inline long double opp_speed_distance(Opp x, Opp xi) {
        // TODO: hella not sure about this!
        return std::pow(static_cast<long double>(x.freq) -
                            static_cast<long double>(xi.freq),
                        2);
    }

    static inline long double opp_speed_weight(Opp x, Opp xi,
                                               long double exp = 2.0) {
        return weight<Opp>(x, xi, opp_speed_distance, exp);
    }

    static inline long double opp_speed_interpolate(Opp x, Opp x1, Opp x2,
                                                    long double y1,
                                                    long double y2) {
        return interpolate<Opp>(x, x1, x2, y1, y2, opp_speed_weight);
    }

    watt_type CPUModelTBApproximate::lookupPower(const string &wname,
                                                 freq_type f,
                                                 volt_type v) const {
        TBParamsIn key;
        key.volt = v;
        key.freq = f;

        auto res_map = find_suitable_map(wname);
        if (res_map == _map.end())
            return 0;

        auto &map = res_map->second;

        auto res = map.first_non_less_key(key);
        if (res == map.cend())
            return 0;

        auto res_key = res->first;

        if (!(key < res_key)) {
            // Exact match!
            return res->second.power;
        }

        // The found key-value pair is the one whose key is the first
        // strictly-greater key with respect to the supplied one. The previous
        // pair, if any, is the one with the last strictly-less key than the
        // supplied one.

        if (res == map.cbegin())
            return res->second.power;

        auto &next = res;
        auto prev = res - 1;

        auto domain_next = next->first;
        auto domain_prev = prev->first;

        auto codomain_next = next->second.power;
        auto codomain_prev = prev->second.power;

        return opp_power_interpolate(key, domain_prev, domain_next,
                                     codomain_prev, codomain_next);
    }

    speed_type CPUModelTBApproximate::lookupSpeed(const string &wname,
                                                  freq_type f,
                                                  volt_type v) const {
        TBParamsIn key;
        key.volt = v;
        key.freq = f;

        auto res_map = find_suitable_map(wname);
        if (res_map == _map.end())
            return 0;

        auto &map = res_map->second;

        auto res = map.first_non_less_key(key);
        if (res == map.cend())
            return 0;

        auto res_key = res->first;

        if (!(key < res_key)) {
            // Exact match!
            return res->second.speedup;
        }

        // The found key-value pair is the one whose key is the first
        // strictly-greater key with respect to the supplied one. The previous
        // pair, if any, is the one with the last strictly-less key than the
        // supplied one.

        if (res == map.cbegin())
            return res->second.speedup;

        auto &next = res;
        auto prev = res - 1;

        auto domain_next = next->first;
        auto domain_prev = prev->first;

        auto codomain_next = next->second.speedup;
        auto codomain_prev = prev->second.speedup;

        return opp_speed_interpolate(key, domain_prev, domain_next,
                                     codomain_prev, codomain_next);
    }

} // namespace RTSim
