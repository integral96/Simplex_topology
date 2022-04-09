#ifndef SIMPLICIALCOMPLEX_HPP
#define SIMPLICIALCOMPLEX_HPP

#include <boost/multi_index_container.hpp>

#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/fusion/algorithm/iteration/fold.hpp>
#include <boost/fusion/include/fold.hpp>

#include <algorithm>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <ostream>
#include <limits>
#include <set>
#include <type_traits>
#include <vector>

#include "Simplex.hpp"

namespace multi_index = boost::multi_index;

namespace topology {

template <typename ...Simplex>
class SimplicialComplex {
public:
private:
    fusion::vector<Simplex...> _simplices;
//    simplex_container_t _simplices;
    template<typename Simplex_>
    struct find_n {
        const Simplex_& simplex_;
        std::vector<int>& true_;
        find_n(const Simplex_& simplex, std::vector<int>& true_) :
            simplex_(simplex), true_(true_) { }
        template<typename T>
        void operator()(T& t) {
            if constexpr(std::is_same_v<remove_cvref_t<T>, remove_cvref_t<Simplex_>>) {
                true_.push_back(1);
            } else true_.push_back(0);
        }
    };
    struct print_ {
        std::ostream& o_;
        print_(std::ostream& o) : o_(o) {}
        template<typename T>
        void operator()(T& t) const {
            o_ << t << " ";
        }
    };

public:
    SimplicialComplex() = delete ;
    SimplicialComplex(Simplex&... simplices)
            : _simplices(fusion::make_vector(simplices...)) {}

    //iterators==========================================================================
    const fusion::vector<Simplex...>& simplices() const {
        return _simplices;
    }
    auto begin() {
        return fusion::begin(_simplices);
//        return _simplices.template get<index_t>().begin();
    }

    auto begin() const {
        return fusion::begin(_simplices);
//        return _simplices.template get<index_t>().begin();
    }

    auto end() {
        return fusion::end(_simplices);
//        return _simplices.template get<index_t>().end();
    }

    auto end() const {
        return fusion::end(_simplices);
//        return _simplices.template get<index_t>().end();
    }
    bool empty() const {
        return fusion::empty(_simplices);
    }
    template<size_t index>
    size_t dimension() const {
        if(!this->empty())
            fusion::at<mpl::int_<index>>(_simplices).dimension();
//            return _simplices.template get<dimension_t>().rbegin()->dimension();
        else
            throw std::runtime_error( "Unable to query dimensionality of empty simplicial complex" );
    }
    template <class Predicat>
    void sort(Predicat&& predicat) {
        _simplices.sort(std::ref(predicat));
    }
    void sort() {
        _simplices.sort();
    }
    size_t size() const {
        return fusion::size(_simplices);
    }
    template<size_t index>
    auto at() const {
        return fusion::at<mpl::int_<index>>(_simplices);
    }
//    lexicographical_iterator begin_lexicographical() {
//        return _simplices.template get<lexicographical_t>().begin();
//    }

//    const_lexicographical_iterator begin_lexicographical() const {
//        return _simplices.template get<lexicographical_t>().begin();
//    }

//    lexicographical_iterator end_lexicographical() {
//        return _simplices.template get<lexicographical_t>().end();
//    }

//    const_lexicographical_iterator end_lexicographical() const {
//        return _simplices.template get<lexicographical_t>().end();
//    }
    template<typename Simplex_>
    size_t find(const Simplex_& simplex) {
        std::vector<int> true_;
        fusion::for_each(_simplices, find_n<Simplex_>(simplex, true_));
        size_t tmp{};
        for(const auto& x : true_) {
//            std::cout << x << std::endl;
            if(x == 0) ++tmp;
            else return tmp;
        }
//        return true_.size();
    }
//    iterator find(const Simplex& simplex) {
//        auto&& it = _simplices.template get<lexicographical_t>().find( simplex );

//        if( it != this->end_lexicographical() )
//            return _simplices.template project<index_t>(it);
//        else
//            return this->end();
//    }
    template<typename Simplex_>
    bool contains(const Simplex_& simplex) const {
        return find(simplex) < size();
    }
//    bool contains(const Simplex& simplex) const {
//        return _simplices.template get<lexicographical_t>().find(simplex) != _simplices.template get<lexicographical_t>().end();
//    }
    template<typename Simplex_>
    size_t index(const Simplex_& simplex) const {
        return find();
    }
    //Output
    friend std::ostream& operator << (std::ostream& o, const SimplicialComplex& s) {
        o << "{ ";
        fusion::for_each(s._simplices, print_(o));
//        if( s.data() != data_type() )
//            o << " (" << s.data() << ")";
        o << "}";

        return o;
    }

};

}


#endif // SIMPLICIALCOMPLEX_HPP


