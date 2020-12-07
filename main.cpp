///////////////////////////////////////////////////////////////
//  Authors: Alec Hamaker, Morgan McNabb, Alexander Wittman
//  File: main.cpp
//  Class: CSCI-4727-940 Operating Systems
//  Due Date: December 9, 2020
//  Start Date: November 30, 2020
//  Last Updated: November 30, 2020
///////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string.h>
#include "cache.h"
#include "TLBuffer.h"
#include "page_table.h"
#include "Services.h"

int main()
{
    parse_config("trace.config");
    display_config_settings();
    parse_traces("trace.dat");
    track_traces();
    print_statistics();
    return 0;
}
