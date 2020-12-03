class pte
{
    public:
        int phys_frame_num;
        bool valid_bit;
        bool dirty_bit;
        int time_accessed;
};
