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

BOOST_AUTO_TEST_CASE(services_parse_config_file_test)
{
    auto old_buffer = std::cout.rdbuf();
    std::cout.rdbuf(nullptr);
    // this will print an error message from parse config, but
    // that is what we are expecting...
    BOOST_CHECK(!parse_config("../bad_trace.config"));

    BOOST_TEST(parse_config("../trace.config"));

    // this will print an error message from parse config, but
    // that is what we are expecting...
    BOOST_TEST(!parse_config("ThisFileDoesNotExist.txt"));
    std::cout.rdbuf(old_buffer);
}

BOOST_AUTO_TEST_CASE(services_parse_config_data_test)
{
    auto old_buffer = std::cout.rdbuf();
    std::cout.rdbuf(nullptr);

    BOOST_TEST(parse_config("../trace.config"));
    BOOST_TEST(TLB_c.num_entries == 2);

    BOOST_TEST(page_table_c.num_virtual_pages == 64);
    BOOST_TEST(page_table_c.num_physical_pages == 4);
    BOOST_TEST(page_table_c.page_size == 256);

    BOOST_TEST(cache_c.num_entries == 4);
    BOOST_TEST(cache_c.set_size == 1);
    BOOST_TEST(cache_c.line_size == 16);
    BOOST_TEST(!cache_c.write_through_no_write_allocate);
    
    BOOST_TEST(cache_c.virtual_addresses);
    BOOST_TEST(cache_c.TLB);
    std::cout.rdbuf(old_buffer);

}

