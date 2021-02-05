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

#include "interpolate.hpp"
namespace RTSim {
    // =====================================================
    // CPUModel
    // =====================================================

    // TODO: Fake factory, convert to actual factory
    std::unique_ptr<CPUModel>
    CPUModel::create(const std::string &k, const CPUPowerModelDescriptor &desc,
                     const OPP &opp, freq_type f_max) {
        if (k == CPUModelMinimalParams::key) {
            return std::make_unique<CPUModelMinimal>(opp, f_max);
        }

        if (k == CPUModelBPParams::key) {
            std::unique_ptr<CPUModelBP> bpp =
                std::make_unique<CPUModelBP>(opp, f_max);

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
                std::make_unique<CPUModelTB>(opp, f_max);

            for (const auto &p : desc.params) {
                const auto *pp = dynamic_cast<CPUModelTBParams *>(p.get());

                OPP params = {pp->freq, pp->volt};

                tbp->setWorkloadParams(pp->workload, params, pp->power,
                                       pp->speed);
            }

            return tbp;
        }

        throw std::exception{}; // TODO:
    }

    void CPUModel::updatePower() {
        OPP opp{_F, _V};
        watt_type p = lookupPower(getCPU()->getWorkload(), opp);
        if (!std::isnan(p))
            _P = p;
    }

    void CPUModel::updateSpeed() {
        OPP opp{_F, _V};
        speed_type s = lookupSpeed(getCPU()->getWorkload(), opp);
        if (!std::isnan(s))
            _S = s;
    }

    // =====================================================
    // CPUModelMinimal
    // =====================================================

    // TODO: why did the old implementation set the frequency to KHz internally?
    // TODO: check formulae against measurement units
    watt_type CPUModelMinimal::lookupPower(const string &,
                                           const OPP &opp) const {
        return (opp.voltage * opp.voltage) * opp.frequency;
    }

    speed_type CPUModelMinimal::lookupSpeed(const string &,
                                            const OPP &opp) const {
        return speed_type(getFrequencyMax()) / speed_type(opp.frequency);
    }

    // =====================================================
    // CPUModelBP
    // =====================================================

    // TODO: remember to require always at least two workload parameters (idle
    // and busy)

    watt_type CPUModelBP::lookupPower(const std::string &workload,
                                      const OPP &opp) const {

        // This model was trained using frequencies in KHz
        const freq_type f = opp.frequency * 1000;
        const volt_type v = opp.voltage;

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
        P_charge = K * f * (v * v);

        // Evaluation of the P_short
        P_short = eta * P_charge;

        // Evalution of the P_dyn
        P_dyn = P_short + P_charge;

        // Evaluation of P_leak
        P_leak = gamma * v * P_dyn;

        // Evaluation of the total Power
        return P_leak + P_dyn + disp;
    }

    speed_type CPUModelBP::lookupSpeed(const std::string &workload,
                                       const OPP &opp) const {
        // This model was trained using frequencies in KHz
        const freq_type f = opp.frequency * 1000;
        const volt_type v = opp.voltage;

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

    void CPUModelTB::setWorkloadParams(const string &wname, const OPP &params,
                                       watt_type power, speed_type speedup) {
        // NOTICE: this may accept duplicates if supplied
        TBParamsOut v;
        v.power = power;
        v.speedup = speedup;

        std::pair<OPP, TBParamsOut> kv = std::make_pair(params, v);
        _map[wname].insert(kv);
    }

    // I know that these functions can be implemented with a lot of templates, I
    // even implemented them all, but the functions became more cryptic and I
    // didn't like them like that, result is the same

    watt_type CPUModelTB::lookupPower(const string &wname,
                                      const OPP &key) const {
        auto res_map = find_suitable_map(wname);
        if (res_map == _map.end())
            return 0;

        auto &map = res_map->second;

        auto res = map.find_from_key(key);
        if (res == map.cend())
            return 0;
        return res->second.power;
    }

    speed_type CPUModelTB::lookupSpeed(const string &wname,
                                       const OPP &key) const {
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

#define cast(T, x) (static_cast<T>(x))
    template <class RT = long double>
    static inline RT opp_power_distance(OPP x, OPP xi) {
        return std::pow(cast(RT, x.voltage) - cast(RT, xi.voltage), 2) *
               std::abs(cast(RT, x.frequency) - cast(RT, xi.frequency));
    }
    template <class RT = long double>
    static inline RT opp_speed_distance(OPP x, OPP xi) {
        // TODO: hella not sure about this!
        return std::pow(cast(RT, x.frequency) - cast(RT, xi.frequency), 2);
    }
#undef cast

    template <class T>
    static inline OPP to_domain(T v) {
        return v->first;
    }

    template <class T, class RT = watt_type>
    static inline RT to_power(T v) {
        return v->second.power;
    }

    template <class T, class RT = speed_type>
    static inline RT to_speed(T v) {
        return v->second.speedup;
    }

    template <class Codomain_fn, class Distance_fn, class Codomain>
    Codomain CPUModelTBApproximate::lookupApproximate(
        const std::string &wname, const OPP &key, Codomain_fn codomain,
        Distance_fn distance) const {

        auto res_map = this->find_suitable_map(wname);
        if (res_map == _map.end())
            return 0;

        auto &map = res_map->second;

        auto res = map.first_non_less_key(key);
        if (res == map.cend())
            return 0;

        auto res_key = res->first;
        if (!(key < res_key)) {
            // Exact match!
            return codomain(res);
        }

        // The found key-value pair is the one whose key is the first
        // strictly-greater key with respect to the supplied one. The previous
        // pair, if any, is the one with the last strictly-less key than the
        // supplied one.

        if (res == map.cbegin())
            return codomain(res);

        auto next = res;
        auto prev = res - 1;

        return interpolate<OPP, long double>(key, prev, next,
                                             to_domain<smap::const_iterator>,
                                             codomain, distance);
    }

    watt_type CPUModelTBApproximate::lookupPower(const string &wname,
                                                 const OPP &opp) const {
        return lookupApproximate(wname, opp,
                                 to_power<smap::const_iterator, long double>,
                                 opp_power_distance<long double>);
    }

    speed_type CPUModelTBApproximate::lookupSpeed(const string &wname,
                                                  const OPP &opp) const {
        return lookupApproximate(wname, opp,
                                 to_speed<smap::const_iterator, long double>,
                                 opp_speed_distance<long double>);
    }

    /*
    watt_type CPUModelTBApproximate::lookupPower(const string &wname,
                                                 const OPP &opp) const {
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

        auto next = res;
        auto prev = res - 1;

        return interpolate<OPP, double_prec>(
            key, prev, next, to_domain<decltype(prev)>,
            to_power<decltype(prev), double_prec>, opp_power_distance);
    }

    speed_type CPUModelTBApproximate::lookupSpeed(const string &wname,
                                                  const OPP &opp) const {
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

        auto next = res;
        auto prev = res - 1;

        return interpolate<OPP, double_prec>(
            key, prev, next, to_domain<decltype(prev)>,
            to_speed<decltype(prev), double_prec>, opp_speed_distance);
    }
    */

} // namespace RTSim
