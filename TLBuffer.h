#ifndef TLBUFFER_H
#define TLBUFFER_H

#include <deque>
#include <map>

class TLBuffer
{
public:
    std::map<int, int> tlb;
    std::deque<int> lru_entries;
    TLBuffer(int capacity);
    bool lookup(int virtual_page_number);
    void insert(int virtual_page_number, int physical_frame_number);

private:
    int size;
};

#endif
