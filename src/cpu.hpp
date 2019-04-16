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
#ifndef __CPU_HPP__
#define __CPU_HPP__

#include <set>
#include <string>
#include <vector>

#include <trace.hpp>
#include <timer.hpp>
#include <powermodel.hpp>
#include <abstask.hpp>
#include <assert.h>

#define _KERNEL_DBG_LEV "Kernel"

namespace RTSim
{

    using namespace std;
    using namespace MetaSim;

    struct OPP {
        /// Voltage of each step (in Volts)
        double voltage;

        /// Frequency of each step (in MHz)
        unsigned int frequency;

        /// The speed is a value between 0 and 1
        double speed;
    };

    /**
     * \ingroup kernels
     *
     * A CPU doesn't know anything about who's running on it: it just has a
     * speed factor. This model contains the energy values (i.e. Voltage and
     * Frequency) of each step. The speed of each step is calculated basing
     * upon the step frequencies.  The function setSpeed(load) adjusts the CPU
     * speed accordingly to the system load, and returns the new CPU speed.
     */
    class CPU : public Entity {        
    private:
        double _max_power_consumption;

        /**
             *  Energy model of the CPU
             */
        CPUModel *powmod;

        /**
             * Delta workload
             */
        string _workload;

        vector<OPP> OPPs;

        /// Name of the CPU
        string cpuName;

        /// currentOPP is a value between 0 and OPPs.size() - 1
        unsigned int currentOPP;

        bool PowerSaving;

        /// Number of speed changes
        unsigned long int frequencySwitching;

        // this is the CPU index in a multiprocessor environment
        int index;

        /// update CPU power/speed model according to currentOPP
        void updateCPUModel();

    public:

        /// Constructor for CPUs
        CPU(const string &name="",
            const vector<double> &V= {},
            const vector<unsigned int> &F= {},
            CPUModel *pm = nullptr);

        ~CPU();

        virtual string getName() const {
            return Entity::getName();
        }

        // ------------------------------------------------------ toString()

        friend ostream& operator<<(ostream &strm, CPU &a);

        virtual string toString() const {
            stringstream ss;
            ss << "(CPU) " << getName() << " cur freq " << getFrequency();
            return ss.str();
        }

        /// set the processor index
        void setIndex(int i)
        {
            index = i;
        }

        /// get the processor index
        int getIndex()
        {
            return index;
        }

        /// Useful for debug
        virtual int getOPP();

        /// return the OPPs bigger than the current one
        virtual vector<struct OPP> getNextOPPs();

        /// expose the internal OPPs, read-only
        std::vector<struct OPP> const & getOPPs() const { return OPPs; };

        /// Useful for debug
        virtual void setOPP(unsigned int newOPP);

        unsigned long int getFrequency() const;

        /// Returns the maximum power consumption obtainable with this
        /// CPU
        virtual double getMaxPowerConsumption();

        /// Returns the maximum power consumption obtainable with this
        /// CPU
        virtual void setMaxPowerConsumption(double max_p);

        /// Returns the current power consumption of the CPU If you
        /// need a normalized value between 0 and 1, you should divide
        /// this value using the getMaxPowerConsumption() function.
        virtual double getCurrentPowerConsumption();

        /// Returns the current power saving of the CPU
        virtual double getCurrentPowerSaving();

        /** Sets a new speed for the CPU accordingly to the system
         *  load.  Returns the new speed.
         */
        virtual double setSpeed(double newLoad);

        /**
         * Set the computation workload on the cpu
         */
        virtual void setWorkload(const string &workload);

        virtual string getWorkload() const;

        /// Returns the current CPU speed (between 0 and 1)
        virtual double getSpeed();

        double getSpeed(unsigned int opp);

        virtual unsigned long int getFrequencySwitching();

        virtual void newRun() {}
        virtual void endRun() {}

        ///Useful for debug
        virtual void check();

        bool operator==(const CPU& c) const {
            return getName().compare(c.getName()) == 0;
        }

    };

    /// to string operator
    ostream& operator<<(ostream &strm, CPU &a);

    /**
     * The abstract CPU factory. Is the base class for every CPU factory which
     * will be implemented.
     */
    class absCPUFactory {

    public:
        virtual CPU* createCPU(const string &name="",
                               const vector<double> &V= {},
                               const vector<unsigned int> &F= {},
                               CPUModel *pm = nullptr) = 0;
        virtual ~absCPUFactory() {}
    };


    /**
     * uniformCPUFactory. A factory of uniform CPUs (whose speeds are maximum).
     * Allocates a CPU and returns a pointer to it
     */
    class uniformCPUFactory : public absCPUFactory {

        char** _names;
        int _curr;
        int _n;
        int index;
    public:
        uniformCPUFactory();
        uniformCPUFactory(char* names[], int n);
        /*
             * Allocates a CPU and returns a pointer to it
             */
        CPU* createCPU(const string &name="",
                       const vector<double> &V= {},
                       const vector<unsigned int> &F= {},
                       CPUModel *pm = nullptr);
    };

    /**
     * Stores already created CPUs and returns the pointers, one by one, to the
     * requesting class.
     */
    class customCPUFactory : public absCPUFactory {

        list<CPU *> CPUs;

    public:

        customCPUFactory() {}

        void addCPU(CPU *c)
        {
            CPUs.push_back(c);
        }

        /*
             * Returns the pointer to one of the stored pre-allocated CPUs.
             */
        CPU *createCPU(const string &name="",
                       const vector<double> &V= {},
                       const vector<unsigned int> &F= {},
                       CPUModel *pm = nullptr)
        {
            if (CPUs.size() > 0) {
                CPU *ret = CPUs.front();
                CPUs.pop_front();
                return ret;
            }
            return nullptr;
        }
    };

  // ------------------------------------------------------------ Big-Little. Classes not meant to be extended
  // Design: I had a class "CPU", which already seemed ok for managing Big-littles. However, this brough bugs,
  // for example when updating an OPP, since it didn't update automatically the OPP of the island.
  // With CPU_BL, a core belongs to an island and changing OPP means changing island OPP. Moreover, classes were
  // build while realizing EnergyMRTKernel, which is a kinda self-adaptive kernel and thus needs to try different
  // OPPs. Thus, those 2 concepts are tight. One aim was not to break the existing code written by others.

  class Island_BL;

  class CPU_BL : public CPU {
  private:
    Island_BL* _island;

    /// Is CPU holding a task, either running and ready (= dispatching)?
    bool _isBusy;

  public:
    /// Reference CPUs frequency to compute CPU capacity. It is a global field to all CPUs
    static unsigned int referenceFrequency;

    CPU_BL(const string &name="",
        const vector<double> &V= {},
        const vector<unsigned int> &F= {},
        CPUModel *pm = nullptr, Island_BL* island = nullptr) : CPU(name, V, F, pm) {
        _island = island;
        _isBusy = false;
    };

    ~CPU_BL();

    virtual unsigned int getOPP() const;

    virtual void setOPP(unsigned int opp);

    vector<struct OPP> getHigherOPPs();

    void setBusy(bool busy);

    bool isIslandBusy();

    bool isBusy() {
        return _isBusy;
    }

    Island_BL* getIsland() { 
        return _island;
    }

    double getPowerConsumption(double frequency);

    void updateCPUModel();

    double getSpeed(double freq);

    virtual unsigned long int getFrequency() const;

    double getVoltage() const;

  };

  class Island_BL : public Entity {
  public:
    typedef enum { BIG=0, LITTLE, NUM_ISLANDS } Island;
    static string IslandName[NUM_ISLANDS];

  private:
    Island _island;

    vector<CPU_BL*> _cpus;

    vector<struct OPP> _opps;

  public:
    Island_BL(const string &name, const Island island, const vector<CPU_BL *> cpus, const vector<struct OPP> opps)
            : Entity(name) {
        _island = island;
        _cpus   = cpus;
        _opps   = opps;
    };

    ~Island_BL();

    Island getIslandType() { return _island; }

    unsigned int getOPP() const { return findMaxOPP(); }

    void setOPP(unsigned int opp) {
        for (CPU_BL* c : _cpus)
            c->setOPP(opp);
    }

    void updateBusy() {
        bool b = false;
        for (CPU_BL* c : _cpus)
            if (c->isBusy())
                b = true;
        for(CPU_BL* c : _cpus)
            c->setBusy(b);
    }

    unsigned int findMaxOPP() const {
        unsigned int opp = 0;
        for (CPU* c : _cpus)
            if (c->getOPP() > opp)
                opp = c->getOPP();
        return opp;
    }

    vector<struct OPP> getHigherOPPs() {
        int maxOPP = findMaxOPP();
        vector<struct OPP> opps;
        for (int i = maxOPP; i < _opps.size(); i++) {
            opps.push_back(_opps.at(i));
        }
        return opps;
    }

    bool isBusy() {
        cout << __func__ << " for " << IslandName[_island] << endl;
        for (CPU_BL* c : _cpus)
            if (c->isBusy()){
                cout << c->toString() << " is busy"<<endl;
                return true;
            }
        cout << "island is free"<<endl;
        return false;
    }

    unsigned int getOPPByFrequency(double frequency) {
        assert(frequency > 0.0);
        for (int i = 0; i < _opps.size(); i++)
            if (_opps.at(i).frequency == frequency)
                return i;
        // exception...
        abort();
        return -1;
    }

    struct OPP getMinOPP() const {
        return getStructOPP(0);
    }

    struct OPP getStructOPP(int i) const {
        return _opps.at(i);
    }

    struct OPP getStructOPP() const {
        return getStructOPP(getOPP());
    }

};


  
} // namespace RTSim

#endif
