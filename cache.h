//
// Created by alexw on 12/2/2020.
//

#ifndef MEMORYHIERARCHYSIMULATION_CACHE_H
#define MEMORYHIERARCHYSIMULATION_CACHE_H

#include <cmath>

class DataCache
{
private:

    uint64_t * cache;
    unsigned int num_entries;
    unsigned int set_size;
    unsigned int line_size;
    unsigned int address_size; //In bits
    bool write_through_no_write_allocate;
    int total_tag_bits;
    int total_index_bits;
    int total_offset_bits;

    void checkCache(unsigned int physicalAddress) {
        unsigned int index = physicalAddress;
        unsigned int bitManager = 0xFFFFFFFF;

        // Get index from the physical address
        index = index >> total_offset_bits;                 // Right shift so that the index is the LSB
        bitManager = bitManager >> (32 - total_index_bits); // Right shift so that only the index bits are left
        index = index & bitManager;                         // Gets the index in the cache
        index = index * num_entries;                        // Gets the offset for that cache index

        printf("Index: %x\n", index);

        // Get Tag from the physical address
        bitManager = 0xFFFFFFFF;
        unsigned int tag = physicalAddress;                     // Copy physical address to get the tag
        bitManager = bitManager >> (32 - total_tag_bits);       // Right shift to equal the number of 1 bits = tag bits
        tag = tag >> (total_offset_bits + total_index_bits);    // Right shift so the tag is the LSB
        tag = tag & bitManager;                                 // And the bit manager and tag to get the tag bits

        printf("Tag: %x\n", tag);

        uint64_t addAddress = 0x2;
        addAddress = addAddress << (address_size);
        addAddress = physicalAddress | addAddress;

        if (cache[2] == NULL)
            cache[2] = addAddress;
        printf("%jx", cache[2]);

        // Loop through number of entries for that given index
        for (int i = 0; i < num_entries; i++)
        {

        }






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

        cache = static_cast<uint64_t *>(calloc((num_entries * set_size), sizeof(uint64_t)));

    }

    void read(unsigned int physicalAddress)
    {
        checkCache(physicalAddress);
    }

    void write(unsigned int physicalAddress)
    {
        checkCache(physicalAddress);
    }

    void freeDataCache()
    {
        free(cache);
    }
};
#endif //MEMORYHIERARCHYSIMULATION_CACHE_H
