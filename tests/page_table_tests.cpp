#include "boost/test/unit_test.hpp"
#include "../page_table.h"
#include <iostream>

using namespace boost::unit_test;


BOOST_AUTO_TEST_CASE(page_table_constructor)
{
    page_table pt(8,2,128);//virtpagecount, physpagecount, pagesize

    BOOST_CHECK(8 == pt.virtual_page_count);
    BOOST_CHECK(8 == pt.entries.capacity());
    BOOST_CHECK(0 == pt.valid_pte_count);
    BOOST_CHECK(0 == pt.phys_frame_index);
    BOOST_CHECK(2 == pt.phys_frame_count);

    for(int i = 0; i < 8; i++)
    {
        BOOST_CHECK(false ==  pt.entries[i].valid_bit);
        BOOST_CHECK(false == pt.entries[i].dirty_bit);
        BOOST_CHECK(-1 == pt.entries[i].phys_frame_num);
    }
}

BOOST_AUTO_TEST_CASE(page_table_lookup)
{
    page_table pt(8,2,128);
    int result = pt.lookup(0);//find virt page 0,
    BOOST_CHECK(result == -1);//miss
    result = pt.lookup(0);//find again
    BOOST_CHECK(result == 0);//we hit
    

    result = pt.lookup(5);//find virt page 5... assigned to phys mem ind 1
    BOOST_CHECK(result == -1);//we miss, it's not loaded yet..
    result = pt.lookup(5);//find 5
    BOOST_CHECK(result == 1);//we hit it's loaded now. (phys mem full)

    result = pt.lookup(2);//find virt page 2.. should be assigned to phys mem ind 0
    BOOST_CHECK(result == -1);//we miss, not loaded yet
    result = pt.lookup(2);//find 2 again
    BOOST_CHECK(result == 0);//we hit

    BOOST_CHECK(pt.valid_pte_count == 2);//should only have 2 valid ptes
    //since there are only 2 physical frames
    int assigned_ptes_count = 0;
    for(int i = 0; i < (int)pt.entries.capacity(); i++)
    {
        if(pt.entries[i].phys_frame_num != -1)
            assigned_ptes_count++;
    }
    BOOST_CHECK(assigned_ptes_count == 3);//should be 3 ptes with assignments 
    //to phys frame numbers
}

BOOST_AUTO_TEST_CASE(page_table_translate)
{
    page_table pt(8,2,128);
    int phys_addr = pt.translate(0, 0x45);
    BOOST_CHECK(phys_addr == 0x45);
    phys_addr = pt.translate(1, 0x45);
    BOOST_CHECK(phys_addr == 0xc5);
}

