////////////////////////////////////////////////////
//  Author: Morgan McNabb
//  File: tlb_tests.cpp
//  Class: CSCI 4727-940 Operating Systems
//  Due Date: December 9, 2020
//  Creation Date: December 6, 2020
//  Last Updated: December 6, 2020
///////////////////////////////////////////////////
#include "boost/test/unit_test.hpp"
#include "../Services.h"

BOOST_AUTO_TEST_CASE(services_parse_config_test)
{
    // this will print an error message from parse config, but
    // that is what we are expecting...
    BOOST_CHECK(!parse_config("../bad_trace.config"));

    BOOST_TEST(parse_config("../trace.config"));

    // this will print an error message from parse config, but
    // that is what we are expecting...
    BOOST_TEST(!parse_config("ThisFileDoesNotExist.txt"));
}

