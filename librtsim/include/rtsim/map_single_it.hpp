#ifndef RTSIM_MAP_SINGLE_IT
#define RTSIM_MAP_SINGLE_IT

namespace RTSim {
    template <class MapIt, bool ReturnFirst = true>
    // TODO: implement either iterator concepts or tagging!
    class MapSingleIt {
    private:
        using self_type = MapSingleIt<MapIt, ReturnFirst>;
        using iterator = MapIt;
        static constexpr bool return_first = ReturnFirst;

    public:
        MapSingleIt() : _it() {}
        MapSingleIt(const iterator &it) : _it(it) {}

        auto operator*() {
            if constexpr (return_first)
                return _it->first;
            else
                return _it->second;
        }

        self_type &operator++() {
            ++_it;
            return *this;
        }

        self_type operator++(int) {
            self_type prev(this);
            ++_it;
            return prev;
        }

        bool operator!=(const self_type &oth) const {
            return !(*this == oth);
        }

        bool operator==(const self_type &oth) const {
            return _it == oth._it;
        }

    private:
        iterator _it;
    };
} // namespace RTSim

#endif // RTSIM_MAP_SINGLE_IT
