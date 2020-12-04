////////////////////////////////////////////////////
//  Author: Morgan McNabb
//  File: tlb_tests.cpp
//  Class: CSCI 4727-940 Operating Systems
//  Due Date: December 9, 2020
//  Creation Date: December 3, 2020
//  Last Updated: December 3, 2020
///////////////////////////////////////////////////
#include "boost/test/unit_test.hpp"
#include "../TLBuffer.h"

BOOST_AUTO_TEST_CASE(TLB_single_insert_test)
{
    TLBuffer tlb(1);

    tlb.insert(10,10);
    BOOST_TEST(tlb.lru_entries.size() == 1);

    std::deque<int> dq = tlb.lru_entries;
    
    auto it = dq.begin();
    BOOST_TEST(*it == 10);
}

BOOST_AUTO_TEST_CASE(TLB_multiple_insert_test)
{
    TLBuffer tlb(3);

    tlb.insert(1,1);
    tlb.insert(2,2);
    tlb.insert(3,3);
    BOOST_TEST(tlb.lru_entries.size() == 3);

    std::deque<int> dq = tlb.lru_entries;

    auto it = dq.begin();
    BOOST_TEST(*it == 3);
    it++;
    BOOST_TEST(*it == 2);
    it++;
    BOOST_TEST(*it == 1);
}

BOOST_AUTO_TEST_CASE(TLB_insert_order_test)
{
    TLBuffer tlb(3);

    tlb.insert(2,2);
    tlb.insert(1,1);
    tlb.insert(3,3);

    BOOST_TEST(tlb.lru_entries.size() == 3);

    std::deque<int> dq = tlb.lru_entries;
    auto it = dq.begin();

    BOOST_TEST(*it == 3);
    it++;
    BOOST_TEST(*it == 1);
    it++;
    BOOST_TEST(*it == 2);
}

BOOST_AUTO_TEST_CASE(TLB_lookup_test)
{
    TLBuffer tlb(3);

    tlb.insert(1,1);
    tlb.insert(2,2);
    tlb.insert(3,3);

    BOOST_TEST(tlb.lru_entries.size() == 3);

    BOOST_TEST(tlb.lookup(3));
    BOOST_TEST(tlb.lookup(2));
    BOOST_TEST(tlb.lookup(1));
    BOOST_TEST(!tlb.lookup(4));
}

BOOST_AUTO_TEST_CASE(TLB_lru_order_after_lookup_test)
{
    TLBuffer tlb(5);

    tlb.insert(1,1);
    tlb.insert(2,2);
    tlb.insert(3,3);
    tlb.insert(4,4);
    tlb.insert(5,5);

    BOOST_TEST(tlb.lru_entries.size() == 5);

    BOOST_TEST(tlb.lookup(3));
    BOOST_TEST(tlb.lookup(1));
    BOOST_TEST(tlb.lookup(5));
    BOOST_TEST(tlb.lookup(4));
    
    BOOST_TEST(!tlb.lookup(6));

    std::deque<int> dq = tlb.lru_entries;
    auto it = dq.begin();

    BOOST_TEST(*it == 4);
    it++;
    BOOST_TEST(*it == 5);
    it++;
    BOOST_TEST(*it == 1);
    it++;
    BOOST_TEST(*it == 3);
    it++;
    BOOST_TEST(*it == 2);
}

BOOST_AUTO_TEST_CASE(TLB_capacity_test)
{
    TLBuffer tlb(2);

    tlb.insert(1,1);
    tlb.insert(2,2);
    tlb.insert(3,3);
    
    BOOST_TEST(tlb.lru_entries.size() == 2);

    std::deque<int> dq = tlb.lru_entries;
    auto last = dq.back();

    BOOST_TEST(last == 2);
}

