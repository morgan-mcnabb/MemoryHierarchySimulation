//
// Created by alexw on 12/2/2020.
//

#ifndef MEMORYHIERARCHYSIMULATION_CACHE_H
#define MEMORYHIERARCHYSIMULATION_CACHE_H

#include <cmath>

class DataCache
{
private:

    unsigned int * cache;
    unsigned int num_entries;
    unsigned int set_size;
    unsigned int line_size;
    unsigned int address_size; //In bits
    bool write_through_no_write_allocate;
    int total_tag_bits;
    int total_index_bits;
    int total_offset_bits;

    void checkCache(unsigned int physicalAddress)
    {
        unsigned int index = physicalAddress;
        unsigned int bitManager = 0xFFFFFFFF;
        index = index >> total_offset_bits;                 // Right shift so that the index is the LSB
        bitManager = bitManager >> (32 - total_index_bits); // Right shift so that only the index bits are left
        index = index & bitManager;                         // Gets the index in the cache
        index = index * num_entries;                        // Gets the offset for that cache index

    }


public:

    DataCache(int numEntries,int setSize,int lineSize, bool writeThroughNoWriteAllocate, int addressSize)
    {
        num_entries = numEntries;
        set_size = setSize;
        line_size = lineSize;
        write_through_no_write_allocate = writeThroughNoWriteAllocate;
        address_size = addressSize;

        total_index_bits = log2(set_size);
        total_offset_bits = log2(line_size);
        total_tag_bits = address_size - total_offset_bits - total_index_bits;

        cache = static_cast<unsigned int *>(malloc((set_size * num_entries) * line_size));
    }

    void read(unsigned int physicalAddress)
    {

    }

    void wright(unsigned int physicalAddress)
    {

    }

    void freeDataCache()
    {
        free(cache);
    }
};
#endif //MEMORYHIERARCHYSIMULATION_CACHE_H
