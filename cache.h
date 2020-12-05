//
// Created by alexw on 12/2/2020.
//

#ifndef MEMORYHIERARCHYSIMULATION_CACHE_H
#define MEMORYHIERARCHYSIMULATION_CACHE_H

#include <cmath>


        //Add valid and dirty bits to the physical address
        uint64_t addAddress = 0x2;
        addAddress = addAddress << (address_size);
        physicalAddress = physicalAddress | addAddress;

        // Loop through number of entries for that given index
        for (int i = 0; i < num_entries; i++)
        {
            //Get cache entry from the cache
            uint64_t cacheEntry = cache[index];
            unsigned int cacheTag = getTag(cacheEntry);

            //Determine if anything is in the given cache position
            if (cacheEntry == NULL)
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
                        for (int j = 0; j < num_entries; j++)
                        {
                            // set the index to the second to last entry
                            index += num_entries - 2;

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
            else if(cacheTag == tag)
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
            else if (i == (num_entries - 1))
            {
                //Cache does not have the same tag as the physical address
                //Insert physical address into the cache and remove the last entry, if not write through && with write
                if(!write_through_no_write_allocate && !isWrite)
                {
                    //Move each entry to the next entry, remove the last entry as it is LRU
                    for (int j = 0; j < (num_entries - 1); j++)
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
        std::cout << "\n" << checkCache(physicalAddress, false);
    }

    void write(unsigned int physicalAddress)
    {
        checkCache(physicalAddress, true);
    }

    void freeDataCache()
    {
        free(cache);
    }
};
#endif //MEMORYHIERARCHYSIMULATION_CACHE_H
