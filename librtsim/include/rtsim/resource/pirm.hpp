#pragma once
#ifndef RTSIM_PIRM_H_
#define RTSIM_PIRM_H_

#include <map>
#include <set>

#include <metasim/plist.hpp>

#include <rtsim/resource/resmanager.hpp>
#include <rtsim/resource/resource.hpp>

namespace RTSim {
    using namespace MetaSim;

    class PIRM : public ResManager {
    public:
        PIRM(const std::string &name = "");

        // TODO: remember to clear everything anew
        void newRun() override;
        void endRun() override;

    protected:
        bool request(AbsRTTask *t, Resource *r, int nr = 1) override;
        void release(AbsRTTask *t, Resource *r, int nr = 1) override;

    private:
        struct task_request_t {
            TaskModel *model;
            int nr_requested;
        };

        struct task_request_higher_prio {
            /// Returns true if the request on the lhs has a higher priority
            /// than the one on the rhs.
            bool operator()(const task_request_t &lhs,
                            const task_request_t &rhs) const {
                static const TaskModel::TaskModelCmp cmp;
                return (cmp(lhs.model, rhs.model));
            }
        };

        void update_invariant(TaskModel *t, std::set<Resource *> *changed);

        // Chain-boost all running tasks that hold this resource (de-boosts
        // tasks if possible)
        void reboost(Resource *r);

        /// Blocked tasks are ordered by priority.
        ///
        /// NOTE: We assume that all the tasks blocking on a resource affer to
        /// the same scheduler kind and that priorities are comparable to each
        /// other.
        using pqueue_t =
            priority_list<task_request_t, task_request_higher_prio>;

        /// Each resource has its own priority queue for blocked tasks
        std::map<Resource *, pqueue_t> _blocked;

        /// Each resource has its own list of tasks currently using the
        /// resource, for increasing all their priorities in case of an
        /// inversion and clearing its effects afterward
        ///
        /// Invariant: all tasks in this set must always have priority >= than
        /// the priority of the task in front of the _blocked queue for each
        /// resource.
        std::map<Resource *, std::set<TaskModel *>> _running;

        /// This information is stored when a task is first seen by the resource
        /// manager. For simplicity, this can never be released or it can be
        /// released when the task does not interact with any resource anymore
        /// (not blocked nor running for any resource).
        std::map<TaskModel *, int> _original_prio;

        /// Each task can only be blocked on ONE resource at any time
        std::map<TaskModel *, Resource *> _blocked_on;

        /// Each running task has its set of resources that he is actively using
        std::map<TaskModel *, std::map<Resource *, int>> _used_res;

        /// NOTE: Priority of a running task may change when a task blocks on a
        /// resource he "owns", but also when a task blocks on another resource
        /// that he does not own. This because it may boost another task that is
        /// in queue for a resource owned by the running task.
    };
} // namespace RTSim

#endif // RTSIM_PIRM_H_
