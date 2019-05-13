#include <piresman_multi.hpp>
#include <multi_cores_scheds.hpp>

namespace RTSim {

    using namespace std;
    using namespace MetaSim;

    PIRManagerMulti::PIRManagerMulti(const std::string &name, MultiCoresScheds* scheds,
    	vector<CPU*> cpus) : PIRManager(name), _scheds(scheds) {
        for (CPU* c : cpus)
        	_resManagers[c] = new PIRManager("PIRManager for " + c->getName());
    }

    bool PIRManagerMulti::request(AbsRTTask *t, Resource *r, int n) {
        assert(t != NULL); assert(r != NULL); assert(n >= 0);
        cout << "t = " << SIMUL.getTime() << " " << taskname(t) << " requests " << n << " x " << r->toString() << endl;

        // Take PIRManager of core where task is
        CPU *c = _scheds->getProcessor(t);
        Scheduler *s = _scheds->getScheduler(c);
        PIRManager *pirm = _resManagers[c];

        // Request resource to PIRManager
        //setSchedulerForTask(_scheds->findTask(t));
        pirm->setScheduler(s);
        bool acquired = pirm->request(t, r, n);
        return acquired;
	}

	/**
	   Releases the resource.
	 */
	void PIRManagerMulti::release(AbsRTTask *t, Resource *r, int n) {
		assert(t != NULL); assert(r != NULL); assert(n >= 0);
		cout << "t = " << SIMUL.getTime() << " " << taskname(t) << " releases " << n << " x " << r->toString() << endl;

		//setSchedulerForTask(_scheds->findTask(t));
		CPU *c = _scheds->getProcessor(t);
		Scheduler *s = _scheds->getScheduler(c);
		PIRManager *pirm = _resManagers[c];

		pirm->release(t, r, n);
	}

}