#ifndef PTE_H
#define PTE_H

class pte
{
    public:
        int phys_frame_num;
        bool valid_bit;
        bool dirty_bit;

        bool operator!=(const pte& other)
        {
            return other.phys_frame_num != phys_frame_num &&
                other.valid_bit != valid_bit && other.dirty_bit != dirty_bit;
        }

        bool operator==(const pte& other)
        {
            return other.phys_frame_num == phys_frame_num &&
                other.valid_bit == valid_bit && other.dirty_bit == dirty_bit;
        }
};

#endif
