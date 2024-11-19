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

void print_bits(unsigned char* array, unsigned int size){
    for (unsigned int i = 0; i < size * 8; i++)
    {
        if (get_bit(array, i))
            printf("1");
        else
            printf("0");
    }
}