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
#include <string.h>
#include "cache.h"
#include "TLBuffer.h"
#include "page_table.h"
#include "Services.h"

void clear_screen()
{
    system("clear");
}

void print_menu()
{
    clear_screen();
    std::cout << "1. Enter name of configuration file for the simulation." << std::endl
              << "2. Enter name of the trace file." << std::endl
              << "3. Begin simulation." << std::endl
              << "4. End program." << std::endl;
}

int main()
{
    bool config_entered = false;
    bool trace_entered = false;
    std::string config_filename;
    std::string trace_filename;
    std::string input;
    int choice = -1;

    do
    {
        print_menu();
        std::cin >> choice;
        switch(choice)
        {
            case -1:
                break;

            case 1:
                // flush the buffer
                clear_screen();
                std::cout << "1. Enter name of configuration file for the simulation." << std::endl;
                std::cin.ignore();
                std::getline(std::cin, config_filename);
                config_entered = config_filename.size() > 0;
                clear_screen();
                break;

            case 2:
                // flush the buffer
                clear_screen();
                std::cout<< "2. Enter name of the trace file." << std::endl;
                std::cin.ignore();
                std::getline(std::cin, trace_filename);
                trace_entered = trace_filename.size() > 0;
                clear_screen();
                break;

            case 3:
                clear_screen();
                std::cout << "3. Begin simulation." << std::endl;
                if(config_entered && trace_entered)
                {
                    if(parse_config(config_filename))
                    {
                        display_config_settings();
                        parse_traces(trace_filename);
                        track_traces();
                        print_statistics();
                        std::cout << "Press enter to continue...";
                        std::cin.ignore();
                        std::cin.get();
                    }
                    else
                        std::cout << "There was an error parsing the config." << std::endl;

                    break;
                }
                
                if(!config_entered)
                    std::cout << "Please enter a configuration file before starting the simulation." << std::endl;
                if(!trace_entered)
                    std::cout << "Please enter the trace file before starting the simulation." << std::endl;
                break;

            case 4:
                clear_screen();
                std::cout << "Exiting simulation." << std::endl;
                break;

            default:
                std::cout << "Invalid input. " << std::endl;
                std::cin.ignore();
                break;
        }
    }while(choice != 4);

    return 0;
}
