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
#include <powermodel.hpp>
#include <cpu.hpp>

namespace RTSim
{

    // Constructors/Destructor

	// Base Parent Class
	//
    CPUModel::CPUModel(double v, unsigned long int f, unsigned long f_max) :
        _cpu(nullptr)
    {
        _V = v;
        _F = f;//setFrequency(f);
        if (f_max == -1)
            _F_max = -1;
        else
            _F_max = f_max;//setFrequencyMax(f_max);
    }

    CPU *CPUModel::getCPU() const
    {
        return _cpu;
    }

    void CPUModel::setCPU(CPU *c)
    {
        _cpu = c;
    }

    double CPUModel::getPower() const
    {
        return _P;
    }

    long double CPUModel::getSpeed()
    {
        //cout << "CPUModel::getSpeed()"<<endl;
        long double res = (double)_F / (double)_F_max;
        return res;
    }

    void CPUModel::setVoltage(double v)
    {
        DBGPRINT_2("CPUModel::setVoltage ", v);
        //cout << endl << "CPUModel::setVoltage" << endl;
        _V = v;
        update();
    }

    void CPUModel::setFrequency(unsigned long int f)
    {
        //cout << endl << "CPUModel::setFrequency " <<f << endl;
        _F = 1000 * f;
        update();
    }

    void CPUModel::setFrequencyMax(unsigned long int f)
    {
        _F_max = 1000 * f;
    }

    // Minimal Class
    //
    CPUModelMinimal::CPUModelMinimal(double v, unsigned long int f) :
        CPUModel(v, f)
    {
    }

    // deprecated, it might introduce bugs if you forget to update after setVoltage/setFrequency()
    void CPUModelMinimal::update()
    {
        //cout << endl << "CPUModelMinimal::update " << _V << " " << _F << endl;
        _P = (_V * _V) * _F;
    }

    // BP Class
    //
    CPUModelBP::CPUModelBP(double v, unsigned long f, unsigned long f_max,
                               double g_idle,
                               double e_idle,
                               double k_idle,
                               double d_idle) :
        CPUModel(v, f, f_max)
    {
        PowerModelBPParams mp;

        mp.d = d_idle;
        mp.e = e_idle;
        mp.g = g_idle;
        mp.k = k_idle;

        _wl_param["idle"] = mp;
    }

    void CPUModelBP::update()
    {
        double K, eta, gamma, disp;
        string _curr_wl = getCPU()->getWorkload();

        disp = _wl_param[_curr_wl].d;
        K = _wl_param[_curr_wl].k;
        eta = _wl_param[_curr_wl].e;
        gamma = _wl_param[_curr_wl].g;

        //cout << endl <<"CPUModelBP::update wl=" << _curr_wl << " _F "<<_F<<" _V "<<_V<< " disp " << disp<<" k "<<K<<" eta "<<eta<<" gamma "<<gamma<<endl;

        // Evaluation of the P_charge
        _P_charge = (K) * _F * (_V * _V);

        // Evaluation of the P_short
        _P_short =  eta * _P_charge;

        // Evalution of the P_dyn
        _P_dyn = _P_short + _P_charge;

        // Evaluation of P_leak
        _P_leak = gamma * _V * _P_dyn;

        // Evaluation of the total Power
        _P = _P_leak + _P_dyn + disp;

        //cout << "_P = " << _P << " " << _P_charge<<" "<<_P_short<<" "<<_P_dyn<<" "<<_P_leak<<endl;

    }

    long double CPUModelBP::speedModel(const ComputationalModelBPParams &m,
                      unsigned long int f) const
    {
        //cout<<endl<<"\t\t\tm.a b c d " << m.a << " "<< m.b << " " << m.c << " " << m.d << " f " <<f<<endl;

        long double disp = m.a;
        long double ideal = m.b / static_cast<long double>(f);
        long double slope = m.c * exp(-(static_cast<long double>(f) / m.d));

        //cout<<endl<<"\t\t\tdisp ideal slope " << disp << " " << ideal << " " << slope << endl;

        // todo remove return
        //return CPUModel::getSpeed();

        return disp + ideal + slope;
    }

    void CPUModelBP::setWorkloadParams(const string &workload_name,
                                         const PowerModelBPParams &power_params,
                                         const ComputationalModelBPParams &computing_params)
    {
        _wl_param[workload_name] = power_params;
        _comp_param[workload_name] = computing_params;
    }

    long double CPUModelBP::getSpeed()
    {
        string curr_wl = getCPU()->getWorkload();
        long double ret = speedModel(_comp_param[curr_wl], _F);
        DBGPRINT("CPUModelBP::getSpeed() " << curr_wl << " " << ret << " " << _F);

        cout<<endl << "\t\t\tCPUModelBP::getSpeed() wl _F " << curr_wl << " " << _F << " ret " << ret << endl;
        return ret;
    }

} // namespace RTSim
