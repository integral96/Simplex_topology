#include <iostream>
#include <string>
#include <sstream>
#include "SimplicialComplex.hpp"
#include "helper.hpp"
#include <boost/type_index.hpp>

class GRAPHWriter {
    struct print_ {
        std::stringstream& streamNodes_;
        std::stringstream& streamEdges_;
        std::vector<std::string>& _nodeLabels;
        size_t& _indexOffset;
        bool& _writeSimplexDataForVertices;
        print_(std::stringstream& streamNodes, std::stringstream& streamEdges,
               std::vector<std::string>& nodeLabels, size_t& indexOffset, bool& writeSimplexDataForVertices) :
                streamNodes_(streamNodes), streamEdges_(streamEdges), _nodeLabels(nodeLabels),
                _indexOffset(indexOffset), _writeSimplexDataForVertices(writeSimplexDataForVertices) {}
        template<typename T>
        void operator()(T& simplex) const {
            if constexpr(T::dim == 0) {
                auto id = *simplex.begin();

                streamNodes_ << "  node [\n"
                            << "    id " << id << "\n";

                if(!_nodeLabels.empty())
                    streamNodes_ << "    label \"" << _nodeLabels.at( id - _indexOffset ) << "\"\n";

                if(_writeSimplexDataForVertices)
                    streamNodes_ << "    data " << simplex.weight() << "\n";

                streamNodes_ << "  ]\n";
            }
            else if constexpr(T::dim >= 1) {
                auto u = *(simplex.begin()    );
//                typedef typename fusion::result_of::next<typename fusion::result_of::begin<T>::type>::type second;
//                typedef typename fusion::result_of::value_of<second>::type next_type;
//                auto v = boost::typeindex::type_id<next_type>();

                streamEdges_ << "  edge [\n"
                            << "    source " << u << "\n"
//                            << "    target " << v << "\n"
                            << "    weight " << simplex.weight() << "\n"
                            << "  ]\n";
            }
        }
    };
public:
    template<class SimplicialComplex>
    void operator()(std::ostream& out, SimplicialComplex& K) {
        std::stringstream streamNodes;
        std::stringstream streamEdges;

        fusion::for_each(K.simplices(), print_(streamNodes, streamEdges, _nodeLabels, _indexOffset, _writeSimplexDataForVertices));


        out << "graph [\n"
            << "  directed 0\n"
            << streamNodes.str() << "\n"
            << streamEdges.str() << "\n"
            << "]\n";
    }
    template <class InputIterator>
    void setNodeLabels(InputIterator begin, InputIterator end) {
        _nodeLabels.assign(begin, end);
    }
    void writeSimplexDataForVertices(bool value = true) {
        _writeSimplexDataForVertices = value;
    }
private:
    std::vector<std::string> _nodeLabels;
    size_t _indexOffset = 1;
    bool _writeSimplexDataForVertices = false;
};

template<class SimplicialComplex>
void storeGraph(const SimplicialComplex& K, std::ostream& out) {
    GRAPHWriter writer;
    writer(out, K);
}

using AK_type  = topology::Simplex<int, int, std::string, double>;
using AK0_type = topology::Simplex<int, int, std::string, char>;
using AK1_type = topology::Simplex<int, int, int, int>;

int main()
{
    AK_type AK(.5, 1, 2, "3", 6.);
    AK0_type AK0(.5, 1, 2, "3", '_');
    AK1_type AK1(1, 2, 3, 2);
//    std::cout << *AK.advance<3>() << std::endl;
//    std::cout << AK.size() << std::endl;
//    std::cout << AK.dimension() << std::endl;
//    std::cout << AK.at<2>() << std::endl;
    std::cout << std::boolalpha;
    std::cout << (AK == AK1) << std::endl;
    std::cout << (AK != AK1) << std::endl;
    std::cout << (AK < AK1) << std::endl;
//    std::cout << AK.contains(std::to_string(3)) << std::endl;
//    std::cout << AK.contains(6.) << std::endl;
//    std::cout << AK.contains(6) << std::endl;
//    std::cout << AK << std::endl;


//    storeGraph(K1, std::cout);

      // 2---1
      // |  /|
      // | / |
      // |/  |
      // 0---3
      //
      // : {0,1,2}, {0,3,1}
//      std::vector<Simplex_graph> trianglesConnected = {
//            {0}, {1}, {2}, {3},
//            {0,1}, {0,2}, {0,3}, {1,2}, {1,3},
//            {0,1,2}, {0,1,3}
//      };

      // 2---1   5
      // |  /   /|\
      // | /   / | \
      // |/   /  |  \
      // 0---3---4---6
      //
      // : {0,3}, {0,1,2}, {3,4,5}, {3,4,6}, {3,5,6}, {4,5,6}, {3,4,5,6}
//      std::vector<Simplex_graph> trianglesDisconnected = {
//            {0}, {1}, {2}, {3}, {4}, {5},
//            {0,1}, {0,2}, {0,3}, {1,2}, {3,4}, {3,5}, {3,6}, {4,5}, {4, 6}, {5, 6},
//            {0,1,2}, {3,4,5}, {3,4,6}, {3,5,6}, {4,5,6}, {3,4,5,6}
//      };

      topology::SimplicialComplex K1(/*AK, */AK0, AK1, AK);
//    K1.find(/*AK1*/);
      std::cout << K1 << std::endl;
//      K1.find(AK1);
      std::cout << K1.find(AK) << std::endl;
//      auto diagrams = topology::calculateDiagrams( K2 );

      storeGraph<decltype (K1)>(K1, std::cout);

//      for( auto&& D : diagrams ) {
//          D.removeDiagonal();
//          std::cout << D << "\n";
//      }
//      std::cout << K2.contains( {1,2} ) << "\n";
//      std::cout << K2.contains( {0,2} ) << "\n";
//      std::cout << K2.index( {0,2} )    << "\n";
//      std::cout << K2.at( 3 )           << "\n";
//      std::cout << K2.size()            << "\n";
//      std::cout << K2.empty()           << "\n";
//      std::cout << K2.dimension()       << "\n";
    return 0;
}


