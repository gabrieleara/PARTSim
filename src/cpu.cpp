/***************************************************************************
begin                : Thu Apr 24 15:54:58 CEST 2003
copyright            : (C) 2003 by Giuseppe Lipari
email                : lipari@sssup.it
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <cpu.hpp>
#include <assert.h>
#include <energyMRTKernel.hpp>

namespace RTSim
{

    unsigned int CPU_BL::referenceFrequency;

    string Island_BL::IslandName[NUM_ISLANDS] = {"BIG","LITTLE"};


    CPU::CPU(const string &name,
             const vector<double> &V,
             const vector<unsigned int> &F,
             CPUModel *pm) :

        Entity(name), frequencySwitching(0), index(0)
    {
        auto num_OPPs = V.size();

        cpuName = name;

        if (num_OPPs == 0) {
            PowerSaving = false;
            return;
        }

        PowerSaving = true;

        // Setting voltages and frequencies
        for (int i = 0; i < num_OPPs; i ++)
        {
            OPP opp;
            opp.voltage = V[i];
            opp.frequency = F[i];
            OPPs.push_back(opp);
        }

        // Creating the Energy Model class
        // and initialize it with the max values
        if (!pm) {
            powmod = new CPUModelMinimal(OPPs[currentOPP].voltage, OPPs[currentOPP].frequency);
            powmod->setFrequencyMax(OPPs[OPPs.size()-1].frequency);
        } else {
            powmod = pm;
            pm->setCPU(this);
        }

        /* Use the maximum OPP by default */
        currentOPP = num_OPPs - 1;

        _workload = "idle";

        // Setting speeds (basing upon frequencies)
        for (unsigned int opp = 0; opp < OPPs.size(); ++opp)
          OPPs[opp].speed = getSpeed(opp);
    }

    CPU::~CPU()
    {
        OPPs.clear();
    }

    int CPU::getOPP()
    {
        if (PowerSaving)
            return currentOPP;
        return 0;
    }

    void CPU::setOPP(unsigned int newOPP)
    {
        //std::cout << __func__ << " setting currentOPP from " << currentOPP << " to " << newOPP << ", OPPs.size()=" << OPPs.size() << std::endl;
        assert(newOPP < OPPs.size() && newOPP >= 0);
        currentOPP = newOPP;
        updateCPUModel();
    }

    unsigned long int CPU::getFrequency() const
    {
        return OPPs[currentOPP].frequency;
    }

    double CPU::getVoltage() const {
        return OPPs[currentOPP].voltage;
    }

    double CPU::getMaxPowerConsumption()
    {
        return _max_power_consumption;
    }

    void CPU::setMaxPowerConsumption(double max_p)
    {
        _max_power_consumption = max_p;
    }

    double CPU::getCurrentPowerConsumption()
    {
        if (PowerSaving) {
            updateCPUModel();
            return (powmod->getPower());
        }
        return 0.0;
    }

    double CPU::getCurrentPowerSaving()
    {
        if (PowerSaving)
        {
            long double maxPowerConsumption = getMaxPowerConsumption();
            long double saved = maxPowerConsumption - getCurrentPowerConsumption();
            return static_cast<double>(saved / maxPowerConsumption);
        }
        return 0;
    }

    double CPU::setSpeed(double newLoad)
    {
        DBGENTER(_KERNEL_DBG_LEV);
        DBGPRINT("pwr: setting speed in CPU::setSpeed()");
        DBGPRINT("pwr: New load is " << newLoad);
        if (PowerSaving)
        {
            DBGPRINT("pwr: PowerSaving=on");
            DBGPRINT("pwr: currentOPP=" << currentOPP);
            for (int i=0; i < (int) OPPs.size(); i++)
                if (OPPs[i].speed >= newLoad)
                {
                    if (i != currentOPP)
                        frequencySwitching++;
                    currentOPP = i;
                    DBGPRINT("pwr: New OPP=" << currentOPP <<" New Speed=" << OPPs[currentOPP].speed);

                    return OPPs[i].speed; //It returns the new speed
                }
        }
        else
            DBGPRINT("pwr: PowerSaving=off => Can't set a new speed!");

        return 1; // An error occurred or PowerSaving is not enabled
    }

    void CPU::setWorkload(const string &workload)
    {
        _workload = workload;
    }

    string CPU::getWorkload() const
    {
        return _workload;
    }
    
    double CPU::getSpeed()
    {
        if (PowerSaving) {
            updateCPUModel();
            return powmod->getSpeed();
        }
        return 1.0;
    }

    double CPU::getSpeed(unsigned int opp)
    {
        if (!PowerSaving)
            return 1;
        assert(opp < OPPs.size() && opp >= 0);
        int old_curr_opp = currentOPP;
        setOPP(opp);
        double s = getSpeed();
        setOPP(old_curr_opp);
        return s;
    }

    unsigned long int CPU::getFrequencySwitching()
    {
        DBGENTER(_KERNEL_DBG_LEV);
        DBGPRINT("frequencySwitching=" << frequencySwitching);

        return frequencySwitching;
    }

    void CPU::check()
    {
        cout << "Checking CPU:" << cpuName << endl;;
        cout << "Max Power Consumption is :" << getMaxPowerConsumption() << endl;
        for (vector<OPP>::iterator iter = OPPs.begin(); iter != OPPs.end(); iter++)
        {
            cout << "-OPP-" << endl;
            cout << "\tFrequency:" << (*iter).frequency << endl;
            cout << "\tVoltage:" << (*iter).voltage << endl;
            cout << "\tSpeed:" << (*iter).speed << endl;
        }
        for (unsigned int i = 0; i < OPPs.size(); i++)
            cout << "Speed level" << getSpeed(i) << endl;
        for (vector<OPP>::iterator iter = OPPs.begin(); iter != OPPs.end(); iter++)
        {
            cout << "Setting speed to " << (*iter).speed << endl;
            setSpeed((*iter).speed);
            cout << "New speed is  " << getSpeed() << endl;
            cout << "Current OPP is  " << getOPP() << endl;
            cout << "Current Power Consumption is  " << getCurrentPowerConsumption() << endl;
            cout << "Current Power Saving is  " << getCurrentPowerSaving() << endl;
        }
    }

    uniformCPUFactory::uniformCPUFactory()
    {
        _curr=0;
        _n=0;
        index = 0;
    }

    uniformCPUFactory::uniformCPUFactory(char* names[], int n)
    {
        _n=n;
        _names = new char*[n];
        for (int i=0; i<n; i++)
        {
            _names[i]=names[i];
        }
        _curr=0;
        index = 0;
    }

    CPU* uniformCPUFactory::createCPU(const string &name,
                                      const vector<double> &V,
                                      const vector<unsigned int> &F,
                                      CPUModel *pm)
    {
        CPU *c;

        if (_curr==_n)
            c = new CPU(name, V, F, pm);
        else
            c = new CPU(_names[_curr++], V, F, pm);

        c->setIndex(index++);
        return c;
    }

    /// to string operator
    ostream& operator<<(ostream &strm, CPU &a) {
      return strm << a.toString();
    }

    // useless, because entity are copied and thus they change name by implementation of Entity...
    bool operator==(const CPU& lhs, const CPU& rhs) {
      return lhs.getName() == rhs.getName();
    }

    void CPU::updateCPUModel() {
        powmod->setVoltage(getVoltage());
        powmod->setFrequency(getFrequency());
    }

    // ------------------------------------------------------------- big little
    unsigned long int CPU_BL::getFrequency() const {
        return _island->getFrequency();
    }

    double CPU_BL::getVoltage() const {
        return _island->getVoltage();
    }

    unsigned int CPU_BL::getOPP() const {
        return _island->getOPP();
    }

    void CPU_BL::setOPP(unsigned int opp) {
        _island->setOPP(opp);
    }

    vector<struct OPP> CPU_BL::getHigherOPPs() {
        return _island->getHigherOPPs();
    }

    void CPU_BL::setBusy(bool busy) {
        _isBusy = busy;
    }

    bool CPU_BL::isIslandBusy() {
        return _island->isBusy();
    }

    IslandType CPU_BL::getIslandType() {
        return _island->getIslandType();
    }

    unsigned long int CPU_BL::getFrequency(unsigned int opp) const {
        return _island->getFrequency(opp);
    }

    double CPU_BL::getPowerConsumption(double frequency) {
        // Find what OPP corresponds to provided frequency
        unsigned int old_opp = getOPP();
        unsigned int opp = _island->getOPPByFrequency(frequency);
        setOPP(opp);
        double pow = getCurrentPowerConsumption();
        setOPP(old_opp);
        return pow;
    }

    double CPU_BL::getSpeed(double freq) {
        unsigned int opp = _island->getOPPByFrequency(freq);
        return getSpeed(opp);
    }

    double CPU_BL::getSpeed(unsigned int opp) {
        int old_curr_opp = getOPP();
        setOPP(opp);
        double s = getSpeed();
        setOPP(old_curr_opp);
        return s;
    }

    double CPU_BL::getSpeed() {
        assert(getOPP() < _island->getOPPsize() && getOPP() >= 0);
        updateCPUModel();
        return _pm->getSpeed();
    }

    void CPU_BL::updateCPUModel() {
        _pm->setVoltage(getVoltage());
        _pm->setFrequency(getFrequency());
    }


    void Island_BL::setOPP(unsigned int opp) {
        assert(opp >= 0 && opp < _opps.size());
        _currentOPP = opp;
        for (CPU_BL* c : getProcessors())
            c->updateCPUModel();
        _kernel->onOppChanged(_currentOPP, this);
    }

}
