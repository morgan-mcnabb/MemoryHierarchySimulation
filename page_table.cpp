/////////////////////////////////////////////////////////////
//  Authors: Alec Hamaker, Morgan McNabb, Alexander Wittman
//  File: page_table.cpp
//  Class: CSCI-4727-940 Operating Systems
//  Due Date: December 9, 2020
//  Start Date: December 2, 2020
//  Last Updated: December 3, 2020
///////////////////////////////////////////////////////////////
#include "page_table.h"

page_table::page_table(int _virt_page_count, int _phys_frame_count, int page_size)
{
    virtual_page_count = _virt_page_count;
    phys_frame_count = _phys_frame_count;
    for(int i = 0; i < virtual_page_count; i++)
    {
        pte tmp;
        tmp.phys_frame_num = -1;
        tmp.valid_bit = false;
        tmp.dirty_bit = false;
        tmp.time_accessed = time(0);
        entries.push_back(tmp);
    }
}

//returns -1 on a miss, returns -2 if out of range
int page_table::lookup(int virtual_page)
{
    if(virtual_page > virtual_page_count-1)//if virtual page out of bounds
        return -2;

    if(entries[virtual_page].valid_bit)
    { 
        entries[virtual_page].time_accessed = time(0);
        return entries[virtual_page].phys_frame_num;
    }
    else//if the physical frame number isnt valid...
    {
        if(valid_pte_count < phys_frame_count)//there is a free spot in memory.
        {
            entries[virtual_page].phys_frame_num = phys_frame_index;
            phys_frame_index++;
            entries[virtual_page].valid_bit = true;
            valid_pte_count++;
            entries[virtual_page].time_accessed = time(0);
        }
        else//we have to evict here
        {
            int min = INT_MAX;
            int eviction_index = -1;
            for(int i = 0; i < virtual_page_count; i++)//walk the table
            {
                if(entries[i].valid_bit && 
                        entries[i].time_accessed < min)
                {
                    min = entries[i].time_accessed;
                    eviction_index = i;
                }
            }

            entries[eviction_index].valid_bit = false;//evict the least recently used pte
            entries[virtual_page].valid_bit = true;
            entries[virtual_page].phys_frame_num = 
                entries[eviction_index].phys_frame_num;//I could be assigning the 
            //wrong frame number here???
            entries[virtual_page].time_accessed = time(0); 
        }
        return -1;//we missed, but we have the entry now...
    }
}
