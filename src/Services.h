/////////////////////////////////////////////////////
//  Authors: Alec Hamaker, Morgan McNabb, Alex Wittman
//  File: Services.h
//  Class: CSCI 4727-940 Operating Systems
//  Due Date: December 9, 2020
//  Creation Date: December 5, 2020
//  Last Updated: December 7, 2020
////////////////////////////////////////////////////
#ifndef SERVICES_H
#define SERVICES_H
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <string.h>
#include <vector>
#include "TLBuffer.h"
#include "page_table.h"
#include "cache.h"

struct DTLB_config
{
    unsigned int num_entries;
};

struct page_table_config
{
    unsigned int num_virtual_pages;
    unsigned int num_physical_pages;
    unsigned int page_size;
};

struct data_cache_config
{   
    unsigned int num_entries;
    unsigned int set_size;
    unsigned int line_size;
    bool write_through_no_write_allocate;
    bool virtual_addresses;
    bool TLB;
};

struct trace
{
    std::string access_type;
    unsigned int input_address;
    unsigned int page_offset;
    unsigned int page_number;
};

DTLB_config TLB_c;
page_table_config page_table_c;
data_cache_config cache_c;
std::vector<trace> traces;
int TLB_hit_count = 0;
int TLB_miss_count = 0;
int PT_hit_count = 0;
int PT_miss_count = 0;
int DC_hit_count = 0;
int DC_miss_count = 0;
int read_count = 0;
int write_count = 0;
int main_mem_ref_count = 0;
int PT_ref_count = 0;
int disk_ref_count = 0;
bool TLB_enabled;
bool virtual_addresses_enabled;

int getTotalIndexBits_cache()
{
    int numberOfIndexBits = log2(cache_c.num_entries);
    if (numberOfIndexBits == 0)
        return 1;
    else
        return numberOfIndexBits;
}

int getTotalOffsetBits_cache()
{
    int numberOfOffsetBits = log2(cache_c.line_size);
    if(numberOfOffsetBits == 0)
        return 1;
    else
        return numberOfOffsetBits;
}

int getTotalOfTagBits_cache()
{
    int numberOfPhysicalPageBits = log2(page_table_c.num_physical_pages);
    int numberOfPageSizeBits = log2(page_table_c.page_size);
    if (numberOfPageSizeBits == 0)
        numberOfPageSizeBits = 1;
    if (numberOfPhysicalPageBits == 0)
        numberOfPhysicalPageBits = 1;
    return (numberOfPhysicalPageBits + numberOfPageSizeBits - getTotalOffsetBits_cache() - getTotalIndexBits_cache());
}


int convert_config_data(std::string line)
{
    // find the delimiter and remove all spaces
    line = line.substr(line.find(':') + 1);
    line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());

    // if the config data is a 'bool'(y or n)
    if((line.compare("y")) != 0 && (line.compare("n")) != 0)
        return stoi(line);
    else
        return line.at(0);
}

bool check_config_validity(int config_value, int lower_val, int upper_val, bool check_power_of_two, std::string type)
{
    if(upper_val > 0)
    {
        if(config_value < lower_val || config_value > upper_val)
        {
            std::cout << "Error: " << type << " must be between " << lower_val 
                << " and " << upper_val << ". Received " << config_value
                << ". Exiting..." << std::endl;
            //exit(-1);
            return false;
        }
    }
    else
    {
        if(config_value < lower_val)
        {
            std::cout << "Error: " << type << " must be larger " << lower_val
                << ". Received " << config_value << ". Exiting..." << std::endl;
            //exit(-1);
            return false;
        }
    }
    if(check_power_of_two)
    {
        if((config_value & (config_value - 1)) != 0)
        {
            std::cout << "Error: " << type << " must be a power of two. Received "
                << config_value << ". Exiting..." << std::endl;
            //exit(-1);
            return false;
        }
    }

    return true;
}

bool parse_config(std::string filename)
{  
    std::string line;
    std::fstream file(filename);
    if(!file.is_open())
    {
        std::cout << "There was an error opening the file " << filename << ". Exiting..." << std::endl;
        return false;
    }
    bool valid_input = true;

    // to know what data we are reading
    int file_line_number = 0;
    while(getline(file, line))
    {
        file_line_number++;
        switch(file_line_number)
        {
            case 2:
                TLB_c.num_entries = convert_config_data(line);
                valid_input = check_config_validity(TLB_c.num_entries, 1, 256, 
                        true, "DTLB number of entries");

                break;
            case 5:
                page_table_c.num_virtual_pages = convert_config_data(line);
                valid_input = check_config_validity
                    (page_table_c.num_virtual_pages, 1, 8192, true, 
                     "Page table number of virtual pages");

                break;
            case 6:
                page_table_c.num_physical_pages = convert_config_data(line);
                valid_input = check_config_validity
                    (page_table_c.num_physical_pages, 1, 1024, true, 
                     "Page table number of physical pages");

                break;
            case 7:
                page_table_c.page_size = convert_config_data(line);
                valid_input = check_config_validity(page_table_c.page_size, 0,
                        0, true, "Page table page size");

                break;
            case 10:

                cache_c.num_entries = convert_config_data(line);
                valid_input = check_config_validity
                    (cache_c.num_entries, 1, 1024, true, 
                     "Cache number of entries");

                break;
            case 11:
                cache_c.set_size = convert_config_data(line);
                valid_input = check_config_validity(cache_c.set_size, 1, 8,
                        true, "Cache set size");

                break;
            case 12:
                cache_c.line_size = convert_config_data(line);
                valid_input = check_config_validity(cache_c.line_size, 8, 0, 
                        true, "Cache line size");

                break;

                // for the next three cases, 121 is 'y' in ascii
            case 13:
                cache_c.write_through_no_write_allocate =
                    convert_config_data(line) == 121;
                break;
            case 15:
                cache_c.virtual_addresses = convert_config_data(line) == 121;
                virtual_addresses_enabled = convert_config_data(line) == 121;
                break;
            case 16:
                cache_c.TLB = convert_config_data(line) == 121;
                TLB_enabled = convert_config_data(line) == 121;
                break;
        }

        if(!valid_input)
            return false;
    }

    return true;
}

void parse_traces(std::string filename)
{
    std::string line;
    const std::string delimiter = ":";
    std::fstream file(filename);
    unsigned int hexx;
    unsigned int page_offset;
    unsigned int page_number;
    
    while(getline(file, line))
    {
        std::string access_type(line.begin(), std::find(line.begin(), line.end(), ':'));
        std::string virtual_address(std::find(line.begin(), line.end(), ':') + 1, line.end());
        int logical_address_size = virtual_address.size();
        hexx = std::stol(virtual_address, nullptr, 16);

        hexx = hexx << 32 - (int)(std::log2(page_table_c.page_size) + std::log2(page_table_c.num_virtual_pages));
        hexx = hexx >> 32 - (int)(std::log2(page_table_c.page_size) + std::log2(page_table_c.num_virtual_pages));

        page_number = hexx / page_table_c.page_size;
        page_offset = hexx % page_table_c.page_size;
 
        trace tmp;
        tmp.page_offset = page_offset;
        tmp.page_number = page_number;
        tmp.access_type = access_type;
        tmp.input_address = hexx;
        traces.push_back(tmp);

        if(tolower(tmp.access_type.c_str()[0]) == 'r')
            read_count++;
        else
            write_count++;
    }
}

void display_config_settings()
{
    std::cout << "Number of virtual pages is " 
        + to_string(page_table_c.num_virtual_pages) + ".\n";
    std::cout << "Number of physical pages is "
        + to_string(page_table_c.num_physical_pages) + ".\n";
    std::cout << "Each page contains " + to_string(page_table_c.page_size) 
        + " bytes.\n";
    std::cout << "Number of bits used for the page table index is "
        + to_string((int)std::log2(page_table_c.num_virtual_pages)) + ".\n";
    std::cout << "Number of bits used for the page offset is "
        + to_string((int)std::log2(page_table_c.page_size)) + ".\n";

    std::cout << endl;

    std::cout << "Data TLB contains " + to_string(TLB_c.num_entries) 
        + " entries.\n";

    std::cout << endl;

    std::cout << "Data cache contains " + to_string(cache_c.set_size) 
        + " sets.\n";
    std::cout << "Each set contains " + to_string(cache_c.num_entries) 
        + " entries.\n";
    std::cout << "Each line is " + to_string(cache_c.line_size) + " bytes.\n";
    if(cache_c.write_through_no_write_allocate)
        std::cout << "The cache uses a write through cache" << 
            " with no write allocation.\n";
    else
        std::cout << "The cache uses write back cache with write" <<
            " allocation.\n";
    std::cout << "Number of bits used for the tag is " 
        + to_string(getTotalOfTagBits_cache()) + ".\n";
    std::cout << "Number of bits used for the index is " 
        + to_string(getTotalIndexBits_cache()) + ".\n";
    std::cout << "Number of bits used for the offset is " 
        + to_string(getTotalOffsetBits_cache()) + ".\n";

    std::cout << endl;

    if(cache_c.virtual_addresses)
        std::cout << "The addresses read in are virtual addresses.\n";
    else
        std::cout << "The addresses read in are physical addresses.\n";

    if(cache_c.TLB)
        std::cout << "The translation lookaside buffer is enabled.\n";
    else
        std::cout << "The translation lookaside buffer is disabled.\n";

    std::cout << endl;

}

void page_table_sim(bool TLB_enabled, int& PT_hit, int& phys_page_num, int& PT_miss_count, int& PT_hit_count, TLBuffer& TLB, page_table& PT, DataCache& cache, trace trace)
{
    if(TLB_enabled)
    {
        PT_hit = PT.lookup(trace.page_number);
        if(PT_hit < 0)//if PT misses
        {
            PT_miss_count++;
            phys_page_num = PT.lookup(trace.page_number);
            TLB.insert(trace.page_number, phys_page_num);
        }
        else
        {
            PT_hit_count++;
            phys_page_num = PT_hit;
            TLB.insert(trace.page_number, PT_hit);
        }
    }
    else
    {
        PT_hit = PT.lookup(trace.page_number);
        if(PT_hit < 0)//if PT misses
        {
            PT_miss_count++;
            phys_page_num = PT.lookup(trace.page_number);
            if(PT_hit == -3)
            {
                TLB.invalidate(trace.page_number);
                int physical_address = page_table_c.page_size * phys_page_num + trace.page_offset;
                cache.invalidateCacheEntry(physical_address);
                // TODO: Do invalidation stuff with the DC here
            }
        }
        else
        {
            PT_hit_count++;
            phys_page_num = PT_hit;
        }
    }
}
void track_traces()
{
    //print the header
    printf("%-8s %-6s %-4s %-4s %-4s %-4s %-6s %-3s %-4s\n", "Virtual", 
            "Virt.", "Page", "TLB", "PT", "Phys", "", "DC", "DC"); 
    printf("%-8s %-6s %-4s %-4s %-4s %-4s %-6s %-3s %-4s\n", "Address", 
            "Page #", "Off.", "Res.", "Res.", "Pg #", "DC Tag", "Idx", "Res."); 
    printf("%-8s %-6s %-4s %-4s %-4s %-4s %-6s %-3s %-4s\n",
            "--------", "------", "----", "----", "----", "----", "------",
            "---", "----"); 

    if(traces.size() > 0)
    {
        // declaration of key simulation variables
        TLBuffer TLB(TLB_c.num_entries);

        page_table PT(page_table_c.num_virtual_pages, 
                page_table_c.num_physical_pages, page_table_c.page_size);

        DataCache cache(cache_c.num_entries, cache_c.set_size, 
                cache_c.line_size, cache_c.write_through_no_write_allocate,
                page_table_c.page_size, page_table_c.num_physical_pages);

        string TLB_res;
        string PT_res;
        bool TLB_hit;
        int PT_hit;
        int phys_page_num;
        int no_virtualization_phys_page_num;

        for(int i = 0; i < (int)traces.size(); i++)
        {
            TLB_res = "";
            PT_res = "";
            if(virtual_addresses_enabled && TLB_enabled)
            {
                TLB_hit = TLB.lookup(traces[i].page_number);
                if(TLB_hit)
                    TLB_hit_count++;
                else
                {
                    TLB_miss_count++;
                    page_table_sim(TLB_enabled, PT_hit, phys_page_num, PT_miss_count, PT_hit_count, TLB, PT, cache, traces[i]);
                }
            }
            else if(!TLB_enabled && virtual_addresses_enabled)
            {
                page_table_sim(TLB_enabled, PT_hit, phys_page_num, PT_miss_count, PT_hit_count, TLB, PT, cache, traces[i]);
            }

            if(TLB_enabled && virtual_addresses_enabled)
                TLB_res = TLB_hit ? "hit" : "miss";

            if(virtual_addresses_enabled && !TLB_hit)
            {
                PT_res = PT_hit >= 0 ? "hit" : "miss";
            }
            if(!virtual_addresses_enabled)
                no_virtualization_phys_page_num = traces[i].input_address >> (int)log2(page_table_c.page_size);


            unsigned int physicalAddress = virtual_addresses_enabled ? PT.translate(traces[i].page_number, traces[i].page_offset)
                : traces[i].input_address;
            unsigned int DCTag = cache.getTag(physicalAddress);
            unsigned int DCIndex = cache.getIndex(physicalAddress);
            string DT_res;
            bool access_type_success;

            access_type_success = traces[i].access_type.compare("R") == 0 ? cache.read(physicalAddress) 
                : cache.write(physicalAddress);

            if(access_type_success)
            {
                DT_res = "hit";
                DC_hit_count++;
            }
            else
            {
                DT_res = "miss";
                DC_miss_count++;
            }

            string formatter = virtual_addresses_enabled ? "%08x %6x %4x %-4s %-4s %4x %6x %3x %-4s\n"
                : "%08s %6s %4x %-4s %-4s %4x %6x %3x %-4s\n";
            char* out_str = (char*)malloc(100);
            if(virtual_addresses_enabled)
            {
                sprintf(out_str, formatter.c_str(), 
                        traces[i].input_address, traces[i].page_number, 
                        traces[i].page_offset, TLB_res.c_str(), PT_res.c_str(), 
                        phys_page_num, DCTag, DCIndex, DT_res.c_str());
            }
            else
            {
                sprintf(out_str, formatter.c_str(), 
                        "", "", 
                        traces[i].page_offset, TLB_res.c_str(), PT_res.c_str(), 
                        no_virtualization_phys_page_num, DCTag, DCIndex, DT_res.c_str());
            }

            cout << out_str;
            free(out_str);
            printf("\n");
            
        }
    }
}

void clear_globals()
{
     TLB_hit_count = 0;
     TLB_miss_count = 0;
     PT_hit_count = 0;
     PT_miss_count = 0;
     DC_hit_count = 0;
     DC_miss_count = 0;
     read_count = 0;
     write_count = 0;
     main_mem_ref_count = 0;
     PT_ref_count = 0;
     disk_ref_count = 0;
     traces.clear();
}

void print_statistics()
{
    printf("Simulation Statistics\n");
    printf("---------------------\n");
    if(TLB_enabled && virtual_addresses_enabled)
    {
        printf("%-25s %-3d\n", "Data TLB hits:", TLB_hit_count);
        printf("%-25s %-3d\n", "Data TLB misses:", TLB_miss_count);
        printf("%-25s %-.6f\n", "Data TLB hit ratio:", (double)TLB_hit_count/ 
                (TLB_hit_count+TLB_miss_count));
        cout << endl;
    }
    if(virtual_addresses_enabled)
    {
        printf("%-25s %-3d\n", "Page table hits:", PT_hit_count);
        printf("%-25s %-3d\n", "Page table faults:", PT_miss_count);
        printf("%-25s %-3.6f\n", "Page table hit ratio:", (double)PT_hit_count/
                (PT_hit_count+PT_miss_count));
        cout << endl;
    }
    printf("%-25s %-3d\n", "Data cache hits:", DC_hit_count);
    printf("%-25s %-3d\n", "Data cache misses:", DC_miss_count);
    printf("%-25s %-.6f\n", "Data cache hit ratio:", (double)DC_hit_count
            /(DC_hit_count+DC_miss_count));
    cout << endl;
    printf("%-25s %-3d\n", "Total reads:", read_count);
    printf("%-25s %-3d\n", "Total writes:", write_count);
    printf("%-25s %-.6f\n", "Ratio of reads:", (double)read_count/
            (read_count+write_count));
    cout << endl;
    printf("%-25s %-3d\n", "Main memory references:", DC_miss_count);
    if(virtual_addresses_enabled)
    {
        printf("%-25s %-3d\n", "Page table references:", PT_hit_count+
                PT_miss_count);
        printf("%-25s %-3d\n", "Disk references:", PT_miss_count);
    }
    else
        printf("%-25s %-3d\n", "Disk references:", read_count+write_count);
    
    clear_globals();
}



#endif
