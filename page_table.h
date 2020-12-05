/////////////////////////////////////////////////////////////
//  Authors: Alec Hamaker, Morgan McNabb, Alexander Wittman
//  File: page_table.h
//  Class: CSCI-4727-940 Operating Systems
//  Due Date: December 9, 2020
//  Start Date: December 2, 2020
//  Last Updated: December 3, 2020
///////////////////////////////////////////////////////////////
#include "pte.h"
#include <ctime>
#include <bits/stdc++.h>

#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

using namespace std;

class page_table
{
    public:
        int virtual_page_count;
        int valid_pte_count = 0;
        int phys_frame_index = 0;
        int phys_frame_count;
        vector<pte> entries;

        page_table(int _virt_page_count, int _phys_page_count, int page_size);

        int lookup(int virtual_page);
};
#endif 
