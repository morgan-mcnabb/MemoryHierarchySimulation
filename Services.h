#ifndef SERVICES_H
#define SERVICES_H
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <string.h>

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
page_table_config page_table_c;
data_cache_config cache;



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
                TLB.num_entries = convert_config_data(line);
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
        std::cout << access_type << " and  " << virtual_address << " VA hex = " << hexx << " page_num: " << page_number << " page_offset: " << page_offset <<   std::endl;


    }
}

#endif
