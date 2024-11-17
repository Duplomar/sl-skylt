unsigned char get_bit(unsigned char* array, unsigned int index)
{
    return array[index/8] & (1 << (index % 8));
}

void set_bit(unsigned char* array, unsigned int index, unsigned char val)
{
    if (val)
        array[index/8] = array[index/8] | (1 << (index % 8));
    else
        array[index/8] = array[index/8] & ~(1 << (index % 8));
}