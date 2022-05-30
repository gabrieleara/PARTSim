#include <rtsim/abskernel.hpp>
#include <rtsim/scheduler/scheduler.hpp>

#include <rtsim/resource/pirm.hpp>

namespace RTSim {
    PIRM::PIRM(const std::string &name) : ResManager(name) {}

    void PIRM::newRun() {
        _blocked_on.clear();
        _blocked.clear();
        _original_prio.clear();
        _running.clear();
        _used_res.clear();
    }

    void PIRM::endRun() {}

#define getTaskScheduler(t_ptr) ((t_ptr)->getKernel()->getScheduler())
#define map_has(map, key) ((map).count(key) > 0)

    static const auto req_higher_prio = task_request_higher_prio();
    static const auto tmodel_higher_prio = TaskModel::TaskModelCmp();

    bool PIRM::request(AbsRTTask *t, Resource *r, int nr) {
        auto sched = getTaskScheduler(t);
        auto tmodel = sched->find(t);
        assert(tmodel != nullptr);

        // Insert acts only if tmodel has not already a saved original priority
        // so we never override even if already boosted
        _original_prio.insert({tmodel, tmodel->getPriority()});

        task_request_t req = {tmodel, nr};

        if (r->available() < nr) {
            goto block;
        }

        if (_blocked[r].size()) {
            // We do not allow any more lower-priority task to get resources
            // when a higher-priority task is blocked and there are already
            // "boosted" tasks around.
            auto &front_req = _blocked[r].front();
            if (req_higher_prio(front_req, req)) {
                goto block;
            }
        }

        // All good, we have a higher priority(1) of any of the blocked tasks
        // (if any) and there are enough resources to satisfy our request
        // immediately.
        //
        // 1: in RTSim tasks cannot have the same priority.

        // Acquire the resource
        r->lock(t, nr);
        _running[r].insert(tmodel);

        // Update used resource (automatically sets zero if no resources were
        // previously used)
        _used_res[tmodel][r] += nr;
        return true;

    block:
        _blocked[r].insert(req);
        _blocked_on[tmodel] = r;
        reboost(r);

        // Extract the task from the scheduler and signal the kernel
        sched->extract(t);
        t->getKernel()->dispatch();
    }

    void PIRM::update_invariant(TaskModel *t, std::set<Resource *> *changed) {
        // It is easier to re-check the invariant on all resources the holds and
        // add all the resources that it is blocked on to `changed` rather than
        // doing something more sophisticated

        // The priority of a task is the highest between its original priority
        // and the priority of the highest blocked task on all the resources it
        // is currently holding
        auto new_prio = _original_prio[t];
        // Assumes that _used_res does NOT contain resourced with 0 as counter
        for (auto [r, _] : _used_res[t]) {
            auto prio = _blocked[r].front().model->getPriority();

            // Lower value => higher priority
            if (new_prio < prio)
                new_prio = prio;
        }

        if (t->getPriority() != new_prio) {
            // boost or de-boost the task
            task_request_t re_request = {t, 0};

            // Check if the task is blocked on some resource
            if (map_has(_blocked_on, t)) {
                auto r = _blocked_on[t];

                // Signal that at least one of the tasks in r has been requeued
                changed->insert(r);

                // Extract the original request (will be requeued after the
                // priority is changed), will update the nr_requested value
                re_request = *_blocked[r].find(re_request);
                _blocked[r].erase(re_request);
            }

            // Raise the priority of the task by extracting it from its
            // scheduler and re-issuing an activation for the same task in its
            // kernel once the priority is changed
            auto task = t->getTask();
            auto kernel = task->getKernel();
            auto sched = kernel->getScheduler();
            sched->extract(task);
            t->changePriority(new_prio);
            kernel->activate(task);
            kernel->dispatch();

            // Re-insert the extracted request (if any), it will be
            // automagically sorted in the right position
            if (map_has(_blocked_on, t)) {
                auto r = _blocked_on[t];
                _blocked[r].insert(re_request);
            }
        }
    }

    void PIRM::reboost(Resource *r) {
        std::set<Resource *> changed;

        for (auto t : _running[r]) {
            update_invariant(t, &changed);
        }

        // For all resources in which at least one task has been re-queued,
        // update the running tasks by recursively calling this function
        for (auto r : changed) {
            reboost(r);
        }
    }

    void PIRM::release(AbsRTTask *t, Resource *r, int nr) {
        auto sched = getTaskScheduler(t);
        auto tmodel = sched->find(t);
        assert(tmodel != nullptr);

        // Release the resources (if enough available)
        auto new_nr = _used_res[tmodel][r];
        new_nr -= nr;
        if (new_nr > 0) {
            _used_res[tmodel][r] = new_nr;
        } else if (new_nr == 0) {
            _used_res[tmodel].erase(r);
            _running[r].erase(tmodel);
        } else {
            throw BaseExc("Cannot release " + std::to_string(nr) +
                          " resources, only " +
                          std::to_string(_used_res[tmodel][r]) + " available!");
        }
        r->unlock(nr);

        int nr_available = r->available();
        bool should_reboost = false;
        while (nr_available > 0 && !_blocked[r].empty()) {
            auto request = _blocked[r].front();
            if (request.nr_requested <= nr_available) {
                should_reboost = true;

                // We can unlock the front task
                _blocked[r].erase(request);
                _blocked_on.erase(request.model);

                auto task = request.model->getTask();
                r->lock(task, request.nr_requested);
                _running[r].insert(request.model);
                _used_res[request.model][r] += nr;

                // The extracted task can be activated as-is, without having to
                // update its invariant, because it was the highest-priority
                // task in the blocked queue
                auto kernel = task->getKernel();
                kernel->activate(task);
                kernel->dispatch();
            }

            // Remove anyway so that even in the case in which we do not lock
            // the resource again we exit the loop because there's not enough
            // available
            nr_available -= request.nr_requested;
        }

        std::set<Resource *> changed;

        // At least one task was extracted from the blocked queue
        if (should_reboost) {
            changed.insert(r);
        }

        // The previously running task may be deboosted here
        update_invariant(tmodel, &changed);

        // Chain-update the other ones
        for (auto r : changed) {
            reboost(r);
        }
    }

} // namespace RTSim
