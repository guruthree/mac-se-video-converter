uint16_t lookuptable[256][8] = {
    {0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF},
    {0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF},
    {0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF},
    {0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF},
    {0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF},
    {0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF},
    {0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF},
    {0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF},
    {0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF},
    {0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF},
    {0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF},
    {0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF},
    {0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF},
    {0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF},
    {0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF},
    {0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF},
    {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF},
    {0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF},
    {0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF},
    {0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF},
    {0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF},
    {0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF},
    {0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF},
    {0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF},
    {0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF},
    {0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF},
    {0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF},
    {0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF},
    {0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF},
    {0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF},
    {0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF},
    {0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF},
    {0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF},
    {0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF},
    {0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF},
    {0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF},
    {0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF},
    {0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF},
    {0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF},
    {0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF},
    {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF},
    {0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF},
    {0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF},
    {0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF},
    {0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF},
    {0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF},
    {0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF},
    {0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF},
    {0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF},
    {0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF},
    {0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF},
    {0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF},
    {0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF},
    {0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF},
    {0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF},
    {0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF},
    {0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF},
    {0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF},
    {0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF},
    {0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF},
    {0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF},
    {0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF},
    {0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF},
    {0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF},
    {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000},
    {0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000},
    {0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000},
    {0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000},
    {0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000},
    {0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000},
    {0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000},
    {0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000},
    {0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000},
    {0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000},
    {0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000},
    {0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000},
    {0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000},
    {0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000},
    {0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000},
    {0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000},
    {0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000},
    {0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000},
    {0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000},
    {0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000},
    {0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000},
    {0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000},
    {0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000},
    {0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000},
    {0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000},
    {0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000},
    {0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000},
    {0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000},
    {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000},
    {0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000},
    {0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000},
    {0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000},
    {0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000},
    {0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000},
    {0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000},
    {0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000},
    {0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000},
    {0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000},
    {0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000},
    {0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000},
    {0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000},
    {0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000},
    {0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000},
    {0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000},
    {0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000},
    {0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000},
    {0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000},
    {0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000},
    {0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000},
    {0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000},
    {0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000},
    {0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000},
    {0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000},
    {0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000},
    {0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000},
    {0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000},
    {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000},
    {0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000},
    {0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000},
    {0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000},
    {0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000},
    {0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000},
    {0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000},
    {0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000},
    {0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000},
    {0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000},
    {0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000},
    {0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000},
    {0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000},
    {0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000},
    {0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000},
    {0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000},
    {0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000},
    {0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000},
    {0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000},
    {0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000},
    {0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000},
    {0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000},
    {0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000},
    {0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000},
    {0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000},
    {0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000},
    {0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000},
    {0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000},
    {0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000},
    {0x0000, 0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000},
    {0x7FFF, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000},
    {0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000},
    {0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000},
    {0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000},
    {0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000},
    {0x7FFF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000},
    {0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
    {0x0000, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
    {0x7FFF, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
    {0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
    {0x0000, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
    {0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
    {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
};
