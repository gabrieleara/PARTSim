#include <algorithm>

#include "cpu.hpp"
#include "island.hpp"

namespace RTSim {
    Island::OPPs_list_type
    Island::buildOPPs(const std::vector<freq_type> &freqs,
                      const std::vector<volt_type> &volts) {
        if (volts.size() != freqs.size())
            throw std::exception();

        OPPs_list_type opps;
        for (size_type i = 0; i < volts.size(); ++i) {
            opps.emplace_back(freqs[i], volts[i]);
        }
        std::sort(opps.begin(), opps.end());
        return opps;
    }

    Island::Island(const string &name, const CPUs_list_type &cpus,
                   const OPPs_list_type &opps) :
        Entity(name), _cpus{cpus}, _opps{opps} {

        if (!std::is_sorted(_opps.begin(), _opps.end()))
            std::sort(_opps.begin(), _opps.end());

        // Use maximum OPP as default OPP
        setOPP(getMaxOPPIndex());

        // TODO: are CPUs owned by Islands or not?
        /*
        for (auto c : _cpus)
            c->setIsland(this);
        */
    };

    void Island::setOPP(size_type opp_index) {
        // Check for out of bound
        if (opp_index > _opps.size())
            throw std::exception();
        _currentOPP = opp_index;

        // TODO: propagate value/notify all CPUs
    }

    void Island::setOPP(const OPP &opp) {
        // For a non-sorted OPP list use std::find
        // auto res = std::find(_opps.cbegin(), _opps.cend(), opp);
        // Since the OPP list is sorted, use std::lower_bound
        auto res = std::lower_bound(_opps.cbegin(), _opps.cend(), opp);

        // Check for not found case
        if (res < _opps.cend() && *res != opp)
            throw std::exception();

        setOPP(res - _opps.cbegin());
    }

    bool Island::isBusy() const {
        for (auto c : cpus())
            if (c->isBusy())
                return true;
        return false;
    }

    /// Finds an element in a sorted set of elements bounded by begin and end
    /// iterators.
    /// It runs correctly if there is only one element in the set X such that (X
    /// < elem) AND (elem < X) (that is X == elem, but only checked using the
    /// given less operator), or if no element in the list is NOT LESS THAN elem
    /// (returning a "not found" value).
    template <class It, class Elem, class Less_fn>
    static size_t find_sorted_first(It begin, It end, const Elem &elem,
                                    const Less_fn &less) {
        // For a sorted list, we can simply use instead
        // Returns the first one that is NOT LESS THAN elem
        auto res = std::lower_bound(begin, end, elem, less);

        // Check for out of bounds/not found case
        if (res > end)
            throw std::exception();

        // Since it is not less than elem, if it is also bigger than
        // elem then elem is not present!
        if (res < end && less(opp, *res))
            throw std::exception();

        return res - begin;
    }

    template <class It, class Elem>
    static size_t find_sorted_first(It begin, It end, const Elem &elem) {
        return find_sorted_first(begin, end, elem, operator<);
    }

    Island::size_type Island::getOPPIndex(const OPP &opp) const {
        // For a non-sorted list, use find_if with a predicate that checks
        // against the mockup OPP that we create with the given frequency
        // (requires <functional>):
        // using std::placeholders::_1;
        // auto predicate = std::bind(operator_less_freq, _1, mockup);
        // auto res = std::find_if(_opps.cbegin(), _opps.cend(), predicate);

        // For a sorted list, we can use this function instead
        return find_sorted_first(_opps.cbegin(), _opps.cend(), opp);
    }

    Island::size_type
    Island::getOPPIndexByFrequency(freq_type frequency) const {
        const OPP mockup = {frequency, 0};

        // For a non-sorted list, use find_if with a predicate that checks
        // against the mockup OPP that we create with the given frequency
        // (requires <functional>)
        // using std::placeholders::_1;
        // auto predicate = std::bind(operator_less_freq, _1, mockup);
        // auto res = std::find_if(_opps.cbegin(), _opps.cend(), predicate);

        // For a sorted list, we can use this function instead, with an
        // appropriate less operator function that checks only the frequency
        // field of each OPP
        return find_sorted_first(_opps.cbegin(), _opps.cend(), mockup,
                                 operator_less_freq);
    }

    // TODO:
    void Island::newRun() {}
    void Island::endRun() {}
} // namespace RTSim
