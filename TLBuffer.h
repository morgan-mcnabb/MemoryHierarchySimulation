///////////////////////////////////////
//  Author: Morgan McNabb
//  File: TLBuffer.h
//  Class: CSCI-4727-940 Operating Systems
//  Due Date: December 9, 2020
//  Creation Date: December 2, 2020
//  Last Update: December 2, 2020
///////////////////////////////////////
#ifndef TLBUFFER_H
#define TLBUFFER_H

#include <deque>
#include <map>

class TLBuffer
{
public:
    // key:value = virtual_address:physical_frame_number
    std::map<int, int> tlb;

    // keeps track of most recent TLB entry. 
    // lru_entries.front() = most recently used
    // lru_entries.back() = least recently used
    std::deque<int> lru_entries;

    TLBuffer(int capacity);
    bool lookup(int virtual_page_number);
    void insert(int virtual_page_number, int physical_frame_number);

private:
    // the current size of the TLB
    int size;
};

#endif
