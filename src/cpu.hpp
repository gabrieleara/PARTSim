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
        virtual void updateCPUModel();

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

        /// expose the internal OPPs, read-only
        std::vector<struct OPP> const & getOPPs() const { return OPPs; };

        /// Useful for debug
        virtual void setOPP(unsigned int newOPP);

        virtual unsigned long int getFrequency() const;

        virtual double getVoltage() const;

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

        virtual double getSpeed(unsigned int opp);

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
  typedef enum { LITTLE=0, BIG, NUM_ISLANDS } Island;

  class CPU_BL : public CPU {
      friend class Island_BL; // otherwise I would have infinite recursion
  private:
    Island_BL* _island;

    /// Is CPU holding a task, either running and ready (= dispatching)?
    bool _isBusy;

    // pm belongs to CPU_BL and not to Island_BL because CPUs may execute different workloads
    CPUModel* _pm;

    virtual void updateCPUModel();

  public:
    /// Reference CPUs frequency to compute CPU capacity. It is a global field to all CPUs
    static unsigned int referenceFrequency;

    CPU_BL(const string &name="",
        const string &wl = "idle", CPUModel* powermodel = nullptr) : CPU(name, {}, {}, NULL) {
        _isBusy = false;
        setWorkload(wl);
        _pm = powermodel;
        assert(_pm != nullptr);
    };

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

    Island getIslandType();

    double getCurrentPowerConsumption()
    {
        updateCPUModel();
        return (_pm->getPower());
    }

    double getPowerConsumption(double frequency);

    virtual double getSpeed();

    double getSpeed(double freq);

    virtual double getSpeed(unsigned int opp);

    unsigned long int getFrequency(unsigned int opp) const;

    virtual unsigned long int getFrequency() const;

    virtual double getVoltage() const;

  };

  class EnergyMRTKernel;

  class Island_BL : public Entity {
  public:
    static string IslandName[NUM_ISLANDS];

  private:
    Island _island;

    vector<CPU_BL*> _cpus;

    vector<struct OPP> _opps;

    unsigned int _currentOPP;

    EnergyMRTKernel* _kernel;

  public:
    Island_BL(const string &name, const Island island, const vector<CPU_BL *> cpus,
            const vector<struct OPP> opps)
            : Entity(name) {
        _island     = island;
        _cpus       = cpus;
        _opps       = opps;
        _currentOPP = 0;

        for (CPU_BL* c : _cpus)
            c->_island = this;

        assert(!opps.empty() && !cpus.empty());
        assert(_island == Island::BIG || _island == Island::LITTLE);
    };

    ~Island_BL() {
        _opps.clear();
        _cpus.clear();
    }

    void setKernel(EnergyMRTKernel* e) { _kernel = e; }

    unsigned int getOPPsize() const { return _opps.size(); }

    Island getIslandType() { return _island; }

    vector<CPU_BL*> getProcessors() { return _cpus; }

    double getVoltage() const {
        return getStructOPP(getOPP()).voltage;
    }

    unsigned long int getFrequency(unsigned int opp) const {
        return getStructOPP(opp).frequency;
    }

    unsigned long int getFrequency() const {
        return getStructOPP(getOPP()).frequency;
    }

    unsigned int getOPP() const {
        return _currentOPP;
    }

    void setOPP(unsigned int opp);

    void updateBusy() {
        bool b = false;
        for (CPU_BL* c : _cpus)
            if (c->isBusy())
                b = true;
        for(CPU_BL* c : _cpus)
            c->_isBusy = b;
    }

    vector<struct OPP> getHigherOPPs() {
        int maxOPP = _currentOPP;
        assert(_currentOPP >= 0 && _currentOPP < _opps.size());
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
            };
        return false;
    }

    unsigned int getOPPByFrequency(double frequency) const {
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
        assert(i >= 0 && i < _opps.size());
        return _opps.at(i);
    }

    struct OPP getStructOPP() const {
        return getStructOPP(getOPP());
    }

    unsigned int getOPPindex(struct OPP opp) const {
        return getOPPByFrequency(opp.frequency);
    }

    // static to oblige you to call this method first
    static vector<struct OPP> buildOPPs(const vector<double> &V= {},
                                        const vector<unsigned int> &F= {}) {
        assert(V.size() > 0 && V.size() == F.size());
        vector<struct OPP> OPPs;
        for (int i = 0; i < V.size(); i ++)
        {
            struct OPP opp;
            opp.voltage = V[i];
            opp.frequency = F[i];
            OPPs.push_back(opp);
        }
        return OPPs;
    }

    virtual void newRun() {}
    virtual void endRun() {}

};


  
} // namespace RTSim

#endif
