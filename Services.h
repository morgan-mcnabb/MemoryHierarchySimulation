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
    unsigned int page_offset;
    unsigned int page_number;
};

DTLB_config TLB_c;
page_table_config page_table_c;
data_cache_config cache;
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
                break;
            case 5:
                page_table_c.num_virtual_pages = convert_config_data(line);
                break;
            case 6:
                page_table_c.num_physical_pages = convert_config_data(line);
                break;
            case 7:
                page_table_c.page_size = convert_config_data(line);
            case 10:
                cache.num_entries = convert_config_data(line);
                break;
            case 11:
                cache.set_size = convert_config_data(line);
                break;
            case 12:
                cache.line_size = convert_config_data(line);
                break;
                
            // for the next three cases, 121 is 'y' in ascii
            case 13:
                cache.write_through_no_write_allocate = convert_config_data(line) == 121;
                break;
            case 15:
                cache.virtual_addresses = convert_config_data(line) == 121;
                break;
            case 16:
                cache.TLB = convert_config_data(line) == 121;
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
        traces.push_back(tmp);


        std::cout << access_type << " and  " << virtual_address << " VA hex = " 
            << hexx << " page_num: " << page_number << " page_offset: " 
            << page_offset <<   std::endl;
    }
}

void track_traces()
{
    if(traces.size() > 0)
    {
        //start
        TLBuffer TLB(TLB_c.num_entries);
        page_table PT(page_table_c.num_virtual_pages, 
                page_table_c.num_physical_pages, page_table_c.page_size);
        for(int i = 0; i < (int)traces.size(); i++)
        {
            bool TLB_hit = TLB.lookup(traces[i].page_number);
            if(TLB_hit)
            {
                //just output forehead?
                //figure out what we are supposed to do with the Data cache here
                std::cout << "TLB: hit. For virt page: " + 
                    to_string(traces[i].page_number) << std::endl;
            }
            else
            {
                std::cout << "TLB: miss.\t";
                //lookup the page in the PT here..
                int PT_hit = PT.lookup(traces[i].page_number);
                if(PT_hit < 0)//if PT misses
                {
                    int phys_frame_num = PT.lookup(traces[i].page_number);
                    std::cout << "Inserting phys frame: " + to_string(phys_frame_num) << "\t";
                    TLB.insert(traces[i].page_number, phys_frame_num);
                    std::cout << "Page Table: miss.\t";
                    //figure out what we are supposed to do with the Data cache 
                    //here
                    
                }
                else
                {
                    std::cout << "Inserting phys frame: " + to_string(PT_hit) << "\t";
                    TLB.insert(traces[i].page_number, PT_hit);
                    std::cout << "Page Table: hit.\t";
                    //figure out what the data cache is supposed to do
                }

                std::cout << "For virtual page: " 
                    + to_string(traces[i].page_number) << endl;
            }
        }
    }
}



#endif
