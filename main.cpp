///////////////////////////////////////////////////////////////
//  Authors: Alec Hamaker, Morgan McNabb, Alexander Wittman
//  File: main.cpp
//  Class: CSCI-4727-940 Operating Systems
//  Due Date: December 9, 2020
//  Start Date: November 30, 2020
//  Last Updated: November 30, 2020
///////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <string.h>
#include "TLBuffer.h"

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

DTLB_config TLB;
page_table_config page_table;
data_cache_config cache;

int convert_config_data(std::string line)
{
    line = line.substr(line.find(':') + 1);
    line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
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
                TLB.num_entries = convert_config_data(line);
                break;
            case 5:
                page_table.num_virtual_pages = convert_config_data(line);
                break;
            case 6:
                page_table.num_physical_pages = convert_config_data(line);
                break;
            case 7:
                page_table.page_size = convert_config_data(line);
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

int main()
{
    TLBuffer p(5);  
    
    p.insert(1,1);
    p.insert(2,2);
    p.insert(3,3);
    p.insert(4,4);
    p.insert(5,5);

    std::deque<int> dq = p.lru_entries;
    for(auto it = dq.begin(); it != dq.end(); ++it)
        std::cout << ' ' << *it << std::endl;


    p.lookup(3);
    p.lookup(1);
    p.lookup(5);

    
    dq = p.lru_entries;
    for(auto it = dq.begin(); it != dq.end(); ++it)
        std::cout << ' ' << *it << std::endl;

    return 0;
}
