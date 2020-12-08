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

    /*
     * changes the valid bit
     */
    uint64_t setValidBit(uint64_t physicalAddress, bool isValid) {
        uint64_t bitManager;
        if (isValid)
        {
            bitManager = 0x0;  //Valid bit set, using bitwise not
        }
        else
        {
            bitManager = 0x2;  //Valid bit cleared using bitwise not
        }

        //Shift bits to the valid & dirty bit position
        bitManager = bitManager << (address_size);
        //NOT Operand so that all bits are 1 to AND the bit manager and physical address
        bitManager = ~bitManager;

        physicalAddress = bitManager & physicalAddress;

        // Return the new address with the set or cleared valid bit
        return physicalAddress;
    }

    unsigned int getValidBit(uint64_t physicalAddress) {
        physicalAddress = physicalAddress >> (address_size + 1);
        return physicalAddress;
    }

    /*
     * Checks the cache to see if address is there or not, adds the address if there is a miss.
     * Returns Hit(true) or Miss(false)
     */
    bool checkCache(uint64_t physicalAddress, bool isWrite) {

        unsigned int index;
        unsigned int tag;

        index = getIndex(physicalAddress) * set_size;        // Gets the offset for that cache index

        tag = getTag(physicalAddress);

        //Add valid and dirty bits to the physical address
        uint64_t addAddress = 0x2;
        addAddress = addAddress << (address_size);
        physicalAddress = physicalAddress | addAddress;

        // Loop through number of entries for that given index
        for (int i = 0; i < set_size; i++)
        {
            //Get cache entry from the cache
            uint64_t cacheEntry = cache[index];
            unsigned int cacheTag = getTag(cacheEntry);
            unsigned int validBit = getValidBit(cacheEntry);

            //Determine if anything is in the given cache position
            if (cacheEntry == 0)
            {
                // No data in cache position, determine if cache needs to be updated
                if(!write_through_no_write_allocate && !isWrite)
                {
                    // Data can be written to the cache
                    // Determine if this is the first entry
                    if (i == 0)
                    {
                        // First Entry, write straight to the cache
                        cache[index] = physicalAddress;
                        // Cache Missed, return false
                        return false;
                    }
                    else if(i > 0)
                    {

                        // set the index back to the first entry
                        index -= i;

                        // Not first entry, shift all entries in set 1 entry over
                        for (int j = 0; j < set_size; j++)
                        {
                            // set the index to the second to last entry
                            index += set_size - 2;

                            // Copy entry address and move it to the next entry
                            cache[index+1] = cache[index];

                            // Move down to the previous entry
                            index--;
                        }

                        // set index back to the first entry
                        index++;

                        // set the physical address to the first entry of the set
                        cache[index] = physicalAddress;

                        // Cache Missed, return false
                        return false;
                    }
                }
                else
                {
                    // No entry in cache, return miss
                    return false;
                }
            }
            else if(cacheTag == tag && validBit == 1)
            {
                //Cache has the same tag as the physical address
                //Get copy of current entry
                uint64_t copyEntry = cache[index];

                //Move the address to the first entry
                for (int j = 0; j < i; j++)
                {
                    //Move the previous entry to the next entry
                    cache[index] = cache[index-1];

                    //Set current index to the previous entry
                    index--;
                }

                // Set the copied entry to the first entry
                cache[index] = copyEntry;

                // Cache Hit, return true
                return true;
            }
            else if(cacheTag == tag && validBit == 0)
            {
                //Cache has the same tag as the physical address
                //Get copy of current entry
                uint64_t copyEntry = cache[index];

                //Move the address to the first entry
                for (int j = 0; j < i; j++)
                {
                    //Move the previous entry to the next entry
                    cache[index] = cache[index-1];

                    //Set current index to the previous entry
                    index--;
                }

                // Set the copied entry to the first entry, set valid bit to true
                cache[index] = setValidBit(copyEntry, true);

                // Cache Hit, return true
                return false;
            }
            else if (i == (set_size - 1))
            {
                //Cache does not have the same tag as the physical address
                //Insert physical address into the cache and remove the last entry, if not write through && with write
                if(!write_through_no_write_allocate && !isWrite)
                {
                    //set address about to be ejected valid bit to 0
                    setValidBit(cache[index], false);

                    //Move each entry to the next entry, remove the last entry as it is LRU
                    for (int j = 0; j < (set_size - 1); j++)
                    {
                        cache[index] = cache[index-1];

                        index--;
                    }
                    cache[index] = physicalAddress;

                    // Miss on cache, return false
                    return false;
                }
                else
                {
                    //Write through, return false for hit
                    return false;
                }
            }
            else
            {
                //Move to the next entry
                index++;
            }
        }
        return false;
    }


public:

    DataCache(int numEntries,int setSize,int lineSize, bool writeThroughNoWriteAllocate, int pageSize, int numberOfPhysicalPages)
    {
        num_entries = numEntries;
        set_size = setSize;
        line_size = lineSize;
        write_through_no_write_allocate = writeThroughNoWriteAllocate;
        address_size = log2(pageSize) + log2(numberOfPhysicalPages);

        total_index_bits = log2(num_entries);
        total_offset_bits = log2(line_size);
        total_tag_bits = address_size - total_offset_bits - total_index_bits;

        cache = static_cast<uint64_t *>(calloc((num_entries * set_size), sizeof(uint64_t)));

    }


    ~DataCache()
    {
        free(cache);
    }


    /*
    * Get the index of a physical address for the cache
    */
    unsigned int getIndex(uint64_t physicalAddress) {
        unsigned int index = physicalAddress;
        unsigned int bitManager = 0xFFFFFFFF;

        // Get index from the physical address
        index = index >> total_offset_bits;                 // Right shift so that the index is the LSB
        bitManager = bitManager >> (32 - total_index_bits); // Right shift so that only the index bits are left
        index = index & bitManager;                         // Gets the index in the cache
        return index;
    }

    /*
     * Get the Tag of a physical address for the cache
     */
    unsigned int getTag(uint64_t physicalAddress) {
        unsigned int bitManager = 0xFFFFFFFF;

        // Get Tag from the physical address
        unsigned int tag = physicalAddress;                     // Copy physical address to get the tag
        bitManager = bitManager >> (32 - total_tag_bits);       // Right shift to equal the number of 1 bits = tag bits
        tag = tag >> (total_offset_bits + total_index_bits);    // Right shift so the tag is the LSB
        tag = tag & bitManager;                                 // And the bit manager and tag to get the tag bits
        return tag;
    }

    bool read(unsigned int physicalAddress)
    {
        return checkCache(physicalAddress, false);
    }

    bool write(unsigned int physicalAddress)
    {
        return checkCache(physicalAddress, true);
    }

    void invalidateCacheEntry(uint64_t physicalAddress)
    {
        unsigned int index = getIndex(physicalAddress);
        unsigned int tag = getTag(physicalAddress);
        for (int i = 0; i < set_size; i++)
        {
            if(tag == getTag(cache[index]))
            {
                cache[index] = setValidBit(cache[index], false);
            }
            index++;
        }
    }
};
#endif //MEMORYHIERARCHYSIMULATION_CACHE_H
