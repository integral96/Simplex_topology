#ifndef SIMPLEX_HPP
#define SIMPLEX_HPP

//#include "Vector.hpp"

#include <boost/functional/hash.hpp>

#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/filter_iterator.hpp>

#include <boost/fusion/include/make_vector.hpp>
#include <boost/fusion/iterator.hpp>
#include <boost/fusion/iterator/iterator_adapter.hpp>
#include <boost/mpl/int.hpp>
#include <boost/fusion/container.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/algorithm.hpp>
#include <boost/fusion/iterator/next.hpp>
#include <boost/fusion/include/next.hpp>

#include <algorithm>
#include <type_traits>
#include <initializer_list>
#include <iosfwd>
#include <stdexcept>
#include <vector>
#include <iostream>

namespace fusion = boost::fusion;
namespace mpl = boost::mpl;

template< class T >
struct remove_cvref {
    typedef std::remove_cv_t<std::remove_reference_t<T>> type;
};
template< class T >
using remove_cvref_t = typename remove_cvref<T>::type;

namespace topology {

namespace filtrations
{

template<typename Simplex, typename Compare = std::less<typename Simplex::weight_type>>
class Data {
public:
    bool operator()(const Simplex& s, const Simplex& t) const {
        if(s.weight() == t.weight()) {
            if(s.dimension() == t.dimension())
                return s < t;
            else return s.dimension() < t.dimension();
        } else return Compare()(s.weight(), t.weight());
    }
};

} // namespace filtrations

template <typename ...Args>
class Simplex {
public:
    using weight_type = double;
    using vertex_container_type = fusion::vector<Args...>;
//    template<size_t I>
    using type = typename fusion::result_of::at_c<vertex_container_type, 1>::type;
    class boundary_iterator;
    static constexpr size_t dim = sizeof...(Args);
private:
    vertex_container_type _vertex_base;
    weight_type _weight;
    template<typename F>
    struct find_ {
        const F& value_;
        F& result_;
        find_(const F& value, F& result) : value_(value), result_(result) {}
        template<typename T>
        void operator()(T& t) {
            if constexpr(std::is_same_v<remove_cvref_t<T>, F>) {
                if(t == value_) result_ = t;
            }
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
    template <char Swith, typename ...Args1>
    struct predicat_ {
        template<typename F>
        struct equal_ {
            const F& other_;
            std::vector<int>& predicate_;
            equal_(const F& other, std::vector<int>& predicate) : other_(other), predicate_(predicate) {}
            template<typename T>
            void operator()(T& t) const {
                static_assert (Swith == '=' || Swith == '<', "Символы сравнения не совпадают");
                if constexpr(std::is_same_v<remove_cvref_t<T>, remove_cvref_t<F>> && Swith == '=') {
                    if(other_ == t) {
                        predicate_.push_back(1);
                    }
                }
                if constexpr(std::is_same_v<remove_cvref_t<T>, remove_cvref_t<F>> && Swith == '<') {
                    if(other_ < t) {
                        predicate_.push_back(1);
                    }
                }
            }
        };
        const Simplex<Args1...>& sim_;
        std::vector<int>& predicate_;
        predicat_(const Simplex<Args1...>& sim, std::vector<int>& predicate) : sim_(sim), predicate_(predicate) {}
        template<typename T>
        void operator()(T& t) const {
            fusion::for_each(sim_.get_vector(), equal_<T>(t, predicate_));
        }
    };
public:
    Simplex(Args&&... args) : _weight(), _vertex_base(fusion::make_vector(std::forward<Args>(args)...)) {}
    Simplex(weight_type weight, Args&&... args) : _weight(weight), _vertex_base(fusion::make_vector(std::forward<Args>(args)...)) {}
    auto begin() const {
        return fusion::begin(_vertex_base);
    }
    vertex_container_type const& get_vector() const {
        return _vertex_base;
    }
    auto end() const {
        return fusion::end(_vertex_base);
    }
//    auto /*fusion::result_of::next<typename fusion::result_of::begin<vertex_container_type>::type>*/ next() const {
//        return fusion::next(_vertex_base);
//    }
    template<size_t N>
    auto advance() const {
        return fusion::advance<mpl::int_<N>>(fusion::begin(_vertex_base));
    }
    size_t size() const {
        return fusion::size(_vertex_base);
    }

    void setWeight(weight_type weight = weight_type()) {
        _weight = weight;
    }
    weight_type weight() const {
        return _weight;
    }
    constexpr size_t dimension() const {
        static_assert (fusion::empty(_vertex_base), "Querying dimension of empty simplex");
        return size() - 1;
    }
    template<size_t index>
    auto at() const {
        return fusion::at<mpl::int_<index>>(_vertex_base);
    }
    //comparison operators==================================================================
    template <typename ...Args1>
    bool operator == (const Simplex<Args1...>& other) const {
        std::vector<int> predicate;
        fusion::for_each(_vertex_base, predicat_<'=', Args1...>(other, predicate));
        if(predicate.size() == size())
            return true;
        else return false;
    }
    template <typename ...Args1>
    bool operator != (const Simplex<Args1...>& other) const {
        return !this->operator==( other );
    }
    template <typename ...Args1>
    bool operator < (const Simplex<Args1...>& other) const {
        std::vector<int> predicate;
        fusion::for_each(_vertex_base, predicat_<'<', Args1...>(other, predicate));
        if(predicate.size() == size())
            return true;
        else return false;
    }
    template<typename T>
    bool contains(T vertex) const {
        T result = T();
        fusion::for_each(_vertex_base, find_<T>(vertex, result));
        if(result != T()) return true;
        else return false;
    }
    boundary_iterator begin_boundary() const {
        // Check dimension directly in order to handle the empty simplex
        if( fusion::empty(_vertex_base) || _vertex_base.size() <= 1 )
        return this->end_boundary();

        return boundary_iterator( begin(), _vertex_base);
    }

    boundary_iterator end_boundary() const {
        return boundary_iterator( end(), _vertex_base);
    }
    //Output
    friend std::ostream& operator << (std::ostream& o, const Simplex& s) {
        o << "{ ";
        fusion::for_each(s._vertex_base, print_(o));
//        if( s.data() != data_type() )
//            o << " (" << s.data() << ")";
        o << "}";

        return o;
    }

};

}


#endif // SIMPLEX_HPP


