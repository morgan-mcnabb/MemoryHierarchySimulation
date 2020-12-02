#include "TLBuffer.h"


TLBuffer::TLBuffer(int capacity)
{
    tlb.clear();
    lru_entries.clear();
    size = capacity;
}

bool TLBuffer::lookup(int virtual_page_number)
{
    if(tlb.find(virtual_page_number) == tlb.end())
        return false;

    std::deque<int>::iterator lru_index = lru_entries.begin();
    while(*lru_index != virtual_page_number)
    {
        lru_index++;
    }

    lru_entries.erase(lru_index);
    lru_entries.push_front(virtual_page_number);

    return true;
}

void TLBuffer::insert(int virtual_page_number, int physical_frame_number)
{
    // m = tlb
    // tlb = lru_entries
    if(tlb.find(virtual_page_number) == tlb.end())
    {
        if(size == lru_entries.size())
        {
            int last = lru_entries.back();
            lru_entries.pop_back();
            tlb.erase(last);
        }
    }
    else
    {
        std::deque<int>::iterator lru_index = lru_entries.begin();
        while(*lru_index != virtual_page_number)
        {
            lru_index++;
        }

        lru_entries.erase(lru_index);
        tlb.erase(virtual_page_number);
    }

    lru_entries.push_front(virtual_page_number);
    tlb[virtual_page_number] = physical_frame_number;

















    /*
    if(tlb.find(virtual_page_number) == tlb.end())
    {
        if(size == lru_entries.size())
        {
            int last = lru_entries.back();
            lru_entries.pop_back();
            tlb.erase(last);
        }
        else
        {
            std::deque<int>::iterator lru_index = lru_entries.begin();
            while(*lru_index != virtual_page_number)
            {
                lru_index++;
            }

            lru_entries.erase(lru_index);
            tlb.erase(virtual_page_number);
        }
    }

    lru_entries.push_front(virtual_page_number);
    tlb[virtual_page_number] = physical_frame_number;
*/
}

