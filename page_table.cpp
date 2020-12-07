/////////////////////////////////////////////////////////////
//  Authors: Alec Hamaker, Morgan McNabb, Alexander Wittman
//  File: page_table.cpp
//  Class: CSCI-4727-940 Operating Systems
//  Due Date: December 9, 2020
//  Start Date: December 2, 2020
//  Last Updated: December 3, 2020
///////////////////////////////////////////////////////////////
#include "page_table.h"

page_table::page_table(int _virt_page_count, int _phys_frame_count, int _page_size)
{
    virtual_page_count = _virt_page_count;
    phys_frame_count = _phys_frame_count;
    page_size = _page_size;
    deck.clear();

    for(int i = 0; i < virtual_page_count; i++)
    {
        pte tmp;
        tmp.phys_frame_num = -1;
        tmp.valid_bit = false;
        tmp.dirty_bit = false;
        entries.push_back(tmp);
    }
}

//returns -1 on a miss, returns -2 if out of range
int page_table::lookup(int virtual_page)
{
    if(virtual_page > virtual_page_count-1)//if virtual page out of bounds
        return -2;

    if(entries[virtual_page].valid_bit)
    { 
        bool res = update_pte(entries[virtual_page]);//move to front of queue
        if(res)
            printf("success\n");
        else
            printf("Fuck\n");
        return entries[virtual_page].phys_frame_num;
    }
    else//if the physical frame number isnt valid...
    {
        if(valid_pte_count < phys_frame_count)//there is a free spot in memory.
        {
            entries[virtual_page].phys_frame_num = phys_frame_index;
            phys_frame_index++;
            entries[virtual_page].valid_bit = true;
            valid_pte_count++;
            
            if(deck.size() < (unsigned int)virtual_page_count)
            {
                deck.push_front(entries[virtual_page]);
                printf("free: pushing: %d\n", entries[virtual_page].phys_frame_num);
            }
            else
            {
                deck.pop_back();
                deck.push_front(entries[virtual_page]);
                printf("free: pushing: %d\n", entries[virtual_page].phys_frame_num);
            }
            //entries[virtual_page].time_accessed = time(0);
            //update_pte(entries[virtual_page]);//move to front of queue
        }
        else//we have to evict here
        {
            for (auto it = deck.begin(); it != deck.end(); ++it)          
                cout << ' ' << (*it).phys_frame_num; 
            cout << endl;
            pte tenant = deck.back();
            deck.pop_back();

            //tenant.valid_bit = false;//evict the least recently used pte
            for(int i = 0; i < virtual_page_count; i++)
            {
                if(entries[i] == tenant)//find the pte to evict
                    entries[i].valid_bit = false;
            }
            entries[virtual_page].valid_bit = true;
            entries[virtual_page].phys_frame_num = 
                tenant.phys_frame_num;
            
            //entries[virtual_page].time_accessed = time(0); 
            bool res = update_pte(entries[virtual_page]);//move to front of queue
            if(res)
                printf("1success\n");
            else
                printf("1Fuck\n");
        }
        return -1;//we missed, but we have the entry now...
    }
}

bool page_table::update_pte(pte entry) 
{
    deque<pte>::iterator deck_index = deck.begin();
    while(*deck_index != entry)//until we find the pte...
    {
        deck_index++;
    }
    if(deck_index == deck.end())
        return false;
    else
    {
        deck.erase(deck_index);//remove the pte
        printf("pushing: %d\n", entry.phys_frame_num);
        deck.push_front(entry);
        return true;
    }
}

int page_table::translate(int virtual_page, int offset)
{
    int phys_frame = lookup(virtual_page);
    int phys_addr;
    if(phys_frame < 0)//we missed..
    {
        //here we know that the virtual page is valid
        //but we will check anyway.. (:
        if(entries[virtual_page].valid_bit)
            phys_frame = entries[virtual_page].phys_frame_num;
        else
            phys_frame = lookup(virtual_page);//if we missed the first time,
            //we wont miss this time...
    }//if we hit, then the phys frame is valid.. so we can calc the addr
    phys_addr = page_size * phys_frame + offset;
    return phys_addr;
}

