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
    int total_mem_reference;

    /*
     * changes the valid bit
     */
    uint64_t setValidBit(uint64_t physicalAddress, bool isValid) {
        uint64_t bitManager;
        if (isValid)
        {
            bitManager = 0x2;  //Valid bit set, using bitwise or
            //Shift bits to the valid & dirty bit position
            bitManager = bitManager << (address_size);
            physicalAddress = bitManager | physicalAddress;
        }
        else
        {
            bitManager = 0x2;  //Valid bit cleared using bitwise not
            //Shift bits to the valid & dirty bit position
            bitManager = bitManager << (address_size);
            //NOT Operand so that all bits are 1 to AND the bit manager and physical address
            bitManager = ~bitManager;
            physicalAddress = bitManager & physicalAddress;
        }

        // Return the new address with the set or cleared valid bit
        return physicalAddress;
    }

    uint64_t setDirtyBit(uint64_t physicalAddress, bool isDirty)
    {
        uint64_t bitManager;
        if (isDirty)
        {
            bitManager = 0x1;  //Dirty bit set, using bitwise not
            //Shift bits to the valid & dirty bit position
            bitManager = bitManager << (address_size);
            physicalAddress = bitManager | physicalAddress;
        }
        else
        {
            bitManager = 0x1;  //Dirty bit cleared using bitwise not
            //Shift bits to the valid & dirty bit position
            bitManager = bitManager << (address_size);
            //NOT Operand so that all bits are 1 to AND the bit manager and physical address
            bitManager = ~bitManager;
            physicalAddress = bitManager & physicalAddress;
        }

        // Return the new address with the set or cleared valid bit
        return physicalAddress;
    }

    unsigned int getValidBit(uint64_t physicalAddress) {
        physicalAddress = physicalAddress >> (address_size + 1);
        return physicalAddress;
    }

    unsigned int getDirtyBit(uint64_t physicalAddress)
    {
        physicalAddress = physicalAddress >> address_size;
        physicalAddress = physicalAddress & 0x1;
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
        physicalAddress = setValidBit(physicalAddress, true);
        physicalAddress = setDirtyBit(physicalAddress, false);

        // Loop through number of entries for that given index
        for (int i = 0; i < set_size; i++)
        {
            //Get cache entry from the cache
            uint64_t cacheEntry = cache[index];
            unsigned int cacheTag = getTag(cacheEntry);
            unsigned int validBit = getValidBit(cacheEntry);
            unsigned int dirtyBit = getDirtyBit(cacheEntry);

            //Determine if anything is in the given cache position
            if (cacheEntry == 0)
            {
                //No entry in cache, Cache Miss
                if (write_through_no_write_allocate)
                {
                    if(isWrite)
                    {
                        //Write on write through policy, does not save in cache
                        //References to memory
                        total_mem_reference++;
                    }
                    else
                    {
                        //Read on write through policy, save in cache
                        //References to memory
                        total_mem_reference++;
                        if (i == 0)
                        {
                            //Set memory to the cache
                            cache[index] = physicalAddress;
                        }
                        else
                        {
                            //Set memory to cache, move other entries one spot over
                            for (int j = 0; j < i; j++)
                            {
                                cache[index-j] = cache[index - (1+j)];
                            }
                            index -= i;
                            cache[index] = physicalAddress;
                        }
                    }
                }
                else
                {
                    //Read or Write on write-back policy with allocation, save in cache
                    //References to memory and cache is not dirty
                    total_mem_reference++;
                    if (i == 0)
                    {
                        //Set memory to the cache
                        cache[index] = physicalAddress;
                    }
                    else
                    {
                        //Set memory to cache, move other entries one spot over
                        for (int j = 0; j < i; j++)
                        {
                            cache[index-j] = cache[index - (1+j)];
                        }
                        index -= i;
                        cache[index] = physicalAddress;
                    }
                }

                //Cache Missed, return False
                return false;
            }
            else if(cacheTag == tag && validBit == 1)
            {
                // Cache Hit
                if (write_through_no_write_allocate)
                {
                    //Write through no allocate policy
                    if (isWrite)
                    {
                        // Data is being written to the cache and memory
                        total_mem_reference++;
                    }
                    else
                    {
                        // Data is being read from the cache
                    }
                }
                else
                {
                    //Write-back with write allocation
                    if(isWrite)
                    {
                        //Write data to the cache and set cache to dirty
                        cache[index] = setDirtyBit(cache[index], true);
                    }
                    else
                    {
                        // Data is being read from the cache
                    }
                }

                //Cache Hit return true
                return true;
            }
            else if(cacheTag == tag && validBit == 0)
            {
                //Cache miss, invalid entry
                if(write_through_no_write_allocate)
                {
                    //Write Through with no write allocate
                    if(isWrite)
                    {
                        //Write data straight to memory
                        total_mem_reference++;
                    }
                    else
                    {
                        //Eject invalid address and move each entry over 1 spot
                        for(int j = 0; j < i; j++)
                        {
                            cache[index-j] = cache[index - (1+j)];
                        }
                        index -= i;
                        cache[index] = physicalAddress;
                    }
                }
                else
                {
                    //Write-back with write allocation
                    if(isWrite)
                    {
                        //Write new memory location to cache
                        if(dirtyBit == 1)
                        {
                            //Memory has been written to cache, but not saved to memory
                            total_mem_reference ++;
                        }
                        //Eject invalid address and move each entry over 1 spot
                        for(int j = 0; j < i; j++)
                        {
                            cache[index-j] = cache[index - (1+j)];
                        }

                        index -= i;

                        //Get Cache entry from memory
                        total_mem_reference++;
                        cache[index] = physicalAddress;

                        //Set cache entry as dirty
                        cache[index] = setDirtyBit(physicalAddress, true);
                    }
                }
                //Cache miss, return false
                return false;
            }
            else if (i == (set_size - 1))
            {
                //Cache entry not in cache, miss
                if(write_through_no_write_allocate)
                {
                    //Write through no write allocate
                    if (isWrite)
                    {
                        //Write straight to memory
                        total_mem_reference++;
                    }
                    else
                    {
                        //Eject LRU address and move each entry over 1 spot
                        for(int j = 0; j < i; j++)
                        {
                            cache[index-j] = cache[index - (1+j)];
                        }

                        index -= i;

                        //Read from memory to the cache
                        total_mem_reference++;
                        cache[index] = physicalAddress;
                    }
                }
                else
                {
                    if (isWrite)
                    {
                        //Writing to cache
                        //Determine if the ejected cache block is dirty and valid
                        if(dirtyBit == 1 && validBit == 1)
                        {
                            //Store data to memory
                            total_mem_reference++;
                        }

                        //Eject LRU address and move each entry over 1 spot
                        for(int j = 0; j < i; j++)
                        {
                            cache[index-j] = cache[index - (1+j)];
                        }

                        index -= i;

                        //Read from memory to the cache
                        total_mem_reference++;
                        cache[index] = physicalAddress;

                        //Mark entry as dirty
                        cache[index] = setDirtyBit(cache[index], true);
                    }
                    else
                    {
                        //Determine if the ejected cache block is dirty and valid
                        if(dirtyBit == 1 && validBit == 1)
                        {
                            //Store data to memory
                            total_mem_reference++;
                        }

                        //Eject LRU address and move each entry over 1 spot
                        for(int j = 0; j < i; j++)
                        {
                            cache[index-j] = cache[index - (1+j)];
                        }

                        index -= i;

                        //Read from memory to the cache
                        total_mem_reference++;
                        cache[index] = physicalAddress;

                        //Mark entry as dirty
                        cache[index] = setDirtyBit(cache[index], false);
                    }
                }

                //Cache missed, return false
                return false;
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
        total_mem_reference = 0;

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

    int getTotalMemReference()
    {
        return total_mem_reference;
    }
};
#endif //MEMORYHIERARCHYSIMULATION_CACHE_H
