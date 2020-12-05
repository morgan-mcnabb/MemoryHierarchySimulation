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

void check_config_validity(int config_value, int lower_val, int upper_val, bool check_power_of_two, std::string type)
{
   if(upper_val > 0)
   {
        if(config_value < lower_val || config_value > upper_val)
        {
            std::cout << "Error: " << type << " must be between " << lower_val 
                      << " and " << upper_val << ". Received " << config_value
                      << ". Exiting..." << std::endl;
            exit(-1);
        }
   }
   else
   {
        if(config_value < lower_val)
        {
            std::cout << "Error: " << type << " must be larger " << lower_val
                      << ". Received " << config_value << ". Exiting..." << std::endl;
            exit(-1);
        }
   }

   if(check_power_of_two)
   {
        if((config_value & (config_value - 1)) != 0)
        {
            std::cout << "Error: " << type << " must be a power of two. Received "
                      << config_value << ". Exiting..." << std::endl;
            exit(-1);
        }
   }
}

void parse_config(std::string filename)
{  
    std::string line;
    std::fstream file(filename);
    
    // to know what data we are reading
    int file_line_number = 0;
    while(getline(file, line))
    {
        file_line_number++;
        switch(file_line_number)
        {
            case 2:
                TLB_c.num_entries = convert_config_data(line);
                check_config_validity(TLB_c.num_entries, 1, 256, true, "DTLB number of entries");
                
                break;
            case 5:
                page_table_c.num_virtual_pages = convert_config_data(line);
                check_config_validity(page_table_c.num_virtual_pages, 1, 8192, true, "Page table number of virtual pages");
               
                break;
            case 6:
                page_table_c.num_physical_pages = convert_config_data(line);
                check_config_validity(page_table_c.num_physical_pages, 1, 1024, true, "Page table number of physical pages");
                
                break;
            case 7:
                page_table_c.page_size = convert_config_data(line);
                check_config_validity(page_table_c.page_size, 0, 0, true, "Page table page size");
                
                break;
            case 10:

                cache.num_entries = convert_config_data(line);
                check_config_validity(cache.num_entries, 1, 1024, true, "Cache number of entries");
                
                break;
            case 11:
                cache.set_size = convert_config_data(line);
                check_config_validity(cache.set_size, 1, 8, true, "Cache set size");
                
                break;
            case 12:
                cache.line_size = convert_config_data(line);
                check_config_validity(cache.line_size, 8, 0, true, "Cache line size");
				
                break;
                
            // for the next three cases, 121 is 'y' in ascii
            case 13:
                cache_c.write_through_no_write_allocate = convert_config_data(line) == 121;
                break;
            case 15:
                cache_c.virtual_addresses = convert_config_data(line) == 121;
                break;
            case 16:
                cache_c.TLB = convert_config_data(line) == 121;
                break;
        }
    }
}

void parse_traces(std::string filename)
{
    std::string line;
    const std::string delimiter = ":";
    std::fstream file(filename);
    long hexx;
    int page_offset;
    int page_number;
    int offset_mask = 0xffffffff >> (32 - (int)std::log2(page_table_c.page_size));
    int page_number_mask =  0xffffffff << (int)std::log2(page_table_c.page_size);

    while(getline(file, line))
    {
        std::string access_type(line.begin(), std::find(line.begin(), line.end(), ':'));
        std::string virtual_address(std::find(line.begin(), line.end(), ':') + 1, line.end());
        hexx = std::stol(virtual_address, nullptr, 16);
        page_offset = hexx & offset_mask;
        page_number = hexx & page_number_mask;
        page_number = page_number >> (int)std::log2(page_table_c.page_size);

        trace tmp;
        tmp.page_offset = page_offset;
        tmp.page_number = page_number;
        tmp.access_type = access_type;
        tmp.input_address = hexx;
        traces.push_back(tmp);


        /*
        std::cout << access_type << " and  " << virtual_address << " VA hex = " 
            << hexx << " page_num: " << page_number << " page_offset: " 
            << page_offset <<   std::endl;
            */
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
    std::cout << "Number of bits used for the tag is " + to_string(420) + ".\n";//ALEX WE NEED YOU TO CALL YOUR DATA CACHE STUFF HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    std::cout << "Number of bits used for the index is " + to_string(69) + ".\n";//ALEX WE NEED YOU TO CALL YOUR DATA CACHE STUFF HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    std::cout << "Number of bits used for the offset is " + to_string(1337) + ".\n";//ALEX WE NEED YOU TO CALL YOUR DATA CACHE STUFF HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

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

void track_traces()
{
    printf("%-8s %-6s %-4s %-4s %-4s %-4s %-6s %-3s %-4s\n", "Virtual", "Virt.", "Page", "TLB", "PT", "Phys", "", "DC", "DC"); 
    printf("%-8s %-6s %-4s %-4s %-4s %-4s %-6s %-3s %-4s\n", "Address", "Page #", "Off.", "Res.", "Res.", "Pg #", "DC Tag", "Idx", "Res."); 
    printf("%-8s %-6s %-4s %-4s %-4s %-4s %-6s %-3s %-4s\n", "--------", "------", "----", "----", "----", "----", "------", "---", "----"); 
    //printf("%08x %6x %4x %4s %4s %4x %6x %3x %4s", "
    if(traces.size() > 0)
    {
        TLBuffer TLB(TLB_c.num_entries);
        page_table PT(page_table_c.num_virtual_pages, 
                page_table_c.num_physical_pages, page_table_c.page_size);
        for(int i = 0; i < (int)traces.size(); i++)
        {
            bool TLB_hit = TLB.lookup(traces[i].page_number);
            bool PT_hit_or_miss;
            int phys_page_num;
            if(TLB_hit)
            {
                //just output forehead?
                //figure out what we are supposed to do with the Data cache here
                /*
                std::cout << "TLB: hit. For virt page: " + 
                    to_string(traces[i].page_number) << std::endl;
                    */
            }
            else
            {
                //std::cout << "TLB: miss.\t";
                //lookup the page in the PT here..
                int PT_hit = PT.lookup(traces[i].page_number);
                if(PT_hit < 0)//if PT misses
                {
                    PT_hit_or_miss = false;
                    //int phys_frame_num = PT.lookup(traces[i].page_number);
                    phys_page_num = PT.lookup(traces[i].page_number);
                    //std::cout << "Inserting phys frame: " + to_string(phys_frame_num) << "\t";
                    TLB.insert(traces[i].page_number, phys_page_num);
                    //std::cout << "Page Table: miss.\t";
                    //figure out what we are supposed to do with the Data cache 
                    //here
                    
                }
                else
                {
                    PT_hit_or_miss = true;
                    phys_page_num = PT_hit;
                    //std::cout << "Inserting phys frame: " + to_string(PT_hit) << "\t";
                    TLB.insert(traces[i].page_number, PT_hit);
                    //std::cout << "Page Table: hit.\t";
                    //figure out what the data cache is supposed to do
                }

                /*
                std::cout << "For virtual page: " 
                    + to_string(traces[i].page_number) << endl;
                    */
            }
            string TLB_res = (TLB_hit) ? "hit" : "miss";
            string PT_res = (PT_hit_or_miss) ? "hit" : "miss";
            
            printf("%08x %6x %4x %4s %4s %4x %6x %3x %4s\n", traces[i].input_address, traces[i].page_number, traces[i].page_offset, TLB_res.c_str(), PT_res.c_str(), phys_page_num, 0x45, 0x45, "?");
        }
    }
}



#endif
