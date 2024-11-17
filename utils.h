unsigned char get_bit(unsigned char* array, unsigned int index)
{
    return array[index/8] & (1 << (index % 8));
}
