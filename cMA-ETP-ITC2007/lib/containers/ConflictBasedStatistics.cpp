#include <vector>
#include <tuple>
#include <iostream>
#include "ConflictBasedStatistics.h"
#include <boost/unordered_map.hpp>


typedef std::tuple<int,int,int> i3tuple;
typedef std::tuple<int, int, int, int, int, int> i6tuple;


void ConflictBasedStatistics::test()
{
    std::vector <i3tuple> v;
    for (int i=0; i<10; i++)
    {
        v.push_back(i3tuple(i,i*2, i*2+1) );
    }

    for(i3tuple t: v)
    {
        std::cout << std::get<0>(t) << ' ';
        std::cout << std::get<1>(t) << ' ';
        std::cout << std::get<2>(t) << '\n';
    }
}



void ConflictBasedStatistics::test1()
{
    std::vector <i6tuple> v;
    boost::unordered_map<i6tuple, int> hashMap;

    for (int i=0; i<10; i++)
    {
        v.push_back(i6tuple(i, i*2, i*2+1, i*10, i*10+1, i*10+2) );
        hashMap[std::make_tuple(i, i*2, i*2+1, i*10, i*10+1, i*10+2)] = i; // Associates each tuple with index i
    }

    for(i6tuple t: v)
    {
        std::cout << std::get<0>(t) << ' ';
        std::cout << std::get<1>(t) << ' ';
        std::cout << std::get<2>(t) << ' ';
        std::cout << std::get<3>(t) << ' ';
        std::cout << std::get<4>(t) << ' ';
        std::cout << std::get<5>(t) << '\n';
    }

    std::cout << "hashMap contains:" << std::endl;
    for (auto& x: hashMap) {
        auto &t = x.first;
        std::cout << std::get<0>(t) << ' ';
        std::cout << std::get<1>(t) << ' ';
        std::cout << std::get<2>(t) << ' ';
        std::cout << std::get<3>(t) << ' ';
        std::cout << std::get<4>(t) << ' ';
        std::cout << std::get<5>(t) << ' ';
        std::cout << ": " << x.second << std::endl;
    }

    std::cout << std::endl;

}
