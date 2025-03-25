/*
    FILE TAKEN FROM CS315 SIMD ASSIGNMENT
*/
#include "bmp.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstring> 
#include <cstdio> 


Bitmap::Bitmap(const char* filename)
{
    file_size = bmp_read(filename);
    size = header.height * header.width;
}

Bitmap::~Bitmap()
{
    delete[] data;
}

void Bitmap::info() const
{
    std::printf("Resolution: (%d,%d) -> Size: %d pixels\n", header.height, header.width, size);
}

/*! Opens a 24 bit true color bmp file and strips its header and its data.
 *  The data starts at location "data", its grouped into 3 layers of "size"
 *  bytes of size and they represent the colors blue, green and red.
 */
unsigned int Bitmap::bmp_read(const char* filename)
{
    std::ifstream ifs(filename, std::ifstream::in | std::ios::binary);

    if (!ifs.is_open())
    {
        std::cerr << "File " << filename << "cannot be opened\n";
        exit(-1);
    }
    ifs.seekg(0, std::ios::end);
    auto length_of_file = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    unsigned char* buffer = new unsigned char[length_of_file];
    ifs.read((char*)buffer, length_of_file);
    memcpy_s(&header, sizeof(bmp_header), buffer, sizeof(bmp_header));

    if (header.bits_per_pixel != 24)
    {
        std::cerr << "Sorry, but can handle only 24-bit true color mode pictures\n";
        exit(-1);
    }

    unsigned int pixel_count = header.width * header.height;
    data = new unsigned char[3 * pixel_count * sizeof(unsigned char)];

    unsigned char* ptr = buffer + header.bmp_data_offset;
    auto bytes_read = 0u;
    for (std::size_t i = 0u; i < pixel_count; i++)
    {
        data[i] = *ptr++;
        data[i + pixel_count] = *ptr++;
        data[i + pixel_count * 2] = *ptr++;
        bytes_read += 3;

        if ((i + 1) % header.width == 0)
        {
            while (bytes_read % 4 != 0)
            {
                ptr++;
                bytes_read++;
            }
        }
    }

    delete[] buffer;
    return static_cast<unsigned>(length_of_file);
}