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

namespace RTSim
{

    unsigned int CPU::referenceFrequency;

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
        }

        /* Use the maximum OPP by default */
        currentOPP = num_OPPs - 1;

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
        currentOPP = newOPP;

        powmod->setFrequency(OPPs[currentOPP].frequency);
    }

    void CPU::setOPP(struct OPP o) {
        //cout<<endl<<"CPU::setOPP o"<<endl;
        int i = 0;
        for (struct OPP opp : OPPs) {
            if (opp.voltage == o.voltage && opp.frequency == o.frequency && opp.speed == o.speed)
                break;
            i++;
        }

        setOPP(i);
    }

    unsigned long int CPU::getFrequency() const
    {
        return OPPs[currentOPP].frequency;
    }

    double CPU::getMaxPowerConsumption()
    {
        return _max_power_consumption;
    }

    void CPU::setMaxPowerConsumption(double max_p)
    {
        _max_power_consumption = max_p;
    }

    void CPU::updateCPUModelOPP(int opp) {
      if (opp == -1) {
        powmod->setVoltage(OPPs[currentOPP].voltage);
        powmod->setFrequency(OPPs[currentOPP].frequency);
        powmod->update();
      } else {
        powmod->setVoltage(OPPs[opp].voltage);
        powmod->setFrequency(OPPs[opp].frequency);
        powmod->update();
      }
    }

    double CPU::getCurrentPowerConsumption()
    {
        //cout<<endl<<"CPU::getCurPowCoons"<<endl;
        if (PowerSaving) {
            updateCPUModelOPP();
            return (powmod->getPower());
        }
        return 0;
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
    
    long double CPU::getSpeed()
    {
        if (PowerSaving) {
            updateCPUModelOPP();
            return powmod->getSpeed();
        }
        return 1;
    }

    double CPU::getSpeed(unsigned int opp)
    {
        if (!PowerSaving)
            return 1;
        assert(opp < OPPs.size());
        int old_curr_opp = currentOPP;
        updateCPUModelOPP(opp);
        double s = powmod->getSpeed();
        updateCPUModelOPP(old_curr_opp);
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

    double CPU::getPowerConsumption(double frequency) {
        //cout << "\t\t\tpowmod->getPower() " << powmod->getPower() << endl;
        //return powmod->getPower() * frequency / OPPs[OPPs.size() - 1].frequency;
        // Find what OPP corresponds to provided frequency
        int opp = -1;
        for (int i = 0; i < OPPs.size(); i++)
            if (OPPs[i].frequency == frequency)
              opp = i;
        assert(opp != -1);
        updateCPUModelOPP(opp);
        return powmod->getPower();
    }

    double CPU::getCapacity(double freq) {
        double capacity = 0.0;
        double speed    = getSpeed();
        int bckOPP      = getOPP();
        double bckFreq  = getFrequency();
        double maxFreqIsland = max_element(OPPs.begin(), OPPs.end(),
                [](const struct OPP& o1, const struct OPP& o2){return o1.frequency < o2.frequency;}).base()->frequency;

        setOPP(OPPs.size() - 1); // needed by getSpeed()
        //capacity = getSpeed() * bckFreq / maxFreqIsland;
        capacity = speed * bckFreq / maxFreqIsland;

        printf("\n\t\t\tcapacity %.17g*%f/%f=%.17g\n", speed, bckFreq, maxFreqIsland, capacity);

        std::ofstream out("capacities.txt", std::fstream::app);
        std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
        std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
        cout << getSpeed() << "\t\t\tfreq " << bckFreq << "\tmaxfreqisland " << maxFreqIsland << "\tcapacity " << capacity << endl;
        std::cout.rdbuf(coutbuf); //reset to standard output again

        setOPP(bckOPP);

        //return freq / CPU::referenceFrequency;
        return capacity;
    }

    std::vector<struct OPP> CPU::getNextOPPs() {
        std::vector<struct OPP> o = std::vector<struct OPP>();
        int i = 0;

        // big-little, if all CPU in current island are executing tasks, consider only its upper OPPs
        if (isCPUIslandBusy()) {
            i = currentOPP;
            cout << "\t\t(a CPU in the island of " << getName() << " is busy at freq " << getFrequency() << ", big-little) " << endl;
        }

        do {
            struct OPP opp;
            opp.speed = OPPs[i].speed;
            opp.frequency = OPPs[i].frequency;
            opp.voltage = OPPs[i].voltage;

            o.push_back(opp);
            i++;
        } while(i < OPPs.size());

        return o;
    }

    std::vector<CPU*> CPU::getCPUsInIsland(std::vector<CPU*> cpus, CPU::Island island) {
        std::vector<CPU*> output;

        for (CPU* c : cpus)
            if (c->getIsland() == island)
                output.push_back(c);

        return output;
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

}
