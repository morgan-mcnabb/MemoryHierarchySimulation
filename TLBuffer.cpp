/////////////////////////////////////
//  Author: Morgan McNabb
//  File: TLBuffer.cpp
//  Class: CSCI-4727-940 Operating Systems
//  Due Date: December 9, 2020
//  Creation Date: December 2, 2020
//  Last Updated: December 3, 2020
/////////////////////////////////////

#include "TLBuffer.h"


TLBuffer::TLBuffer(int capacity)
{
    tlb.clear();
    lru_entries.clear();
    size = capacity;
}

bool TLBuffer::lookup(int virtual_page_number)
{
    // if the iterator reaches the end, the VPN 
    // does not exist in the TLB
    if(tlb.find(virtual_page_number) == tlb.end())
        return false;

    
    remove_lru_entry(virtual_page_number);
    // push the VPN to the front of the TLB cache
    lru_entries.push_front(virtual_page_number);

    return true;
}

void TLBuffer::insert(int virtual_page_number, int physical_frame_number)
{
    // if the VPN is not in the tlb
    if(tlb.find(virtual_page_number) == tlb.end())
    {
        // if the TLB cache is full
        if(size == lru_entries.size())
        {
            // remove the least recently used TLB entry
            int last = lru_entries.back();
            lru_entries.pop_back();
            tlb.erase(last);
        }
    }
    else
    {
        // for indexing through the LRU entries inside the TLB
        std::deque<int>::iterator lru_index = lru_entries.begin();
        //
        // do this until we find the VPN in the LRU cache
        while(*lru_index != virtual_page_number)
        {
            lru_index++;
        }

        // remove the VPN from its current location in both
        // the TLB cache and the TLB
        lru_entries.erase(lru_index);

        remove_lru_entry(virtual_page_number);
        tlb.erase(virtual_page_number);
    }

    // insert the VPN at the most recently used location in the TLB cache
    lru_entries.push_front(virtual_page_number);

    // match the VPN to the PFN in the TLB
    tlb[virtual_page_number] = physical_frame_number;
}

void TLBuffer::invalidate(int virtual_page_number)
{
    if(tlb.find(virtual_page_number) == tlb.end())
        return;
    
    remove_lru_entry(virtual_page_number);
    tlb.erase(virtual_page_number);
}

void TLBuffer::remove_lru_entry(int virtual_page_number)
{
    // for indexing through the LRU entries inside the TLB
    std::deque<int>::iterator lru_index = lru_entries.begin();

    // do this until we find the VPN in the LRU cache
    while(*lru_index != virtual_page_number)
    {
        lru_index++;
    }

    // remove the VPN at its location in the TLB cache
    lru_entries.erase(lru_index);
}
