/*
    FILE TAKEN FROM CS315 SIMD ASSIGNMENT
*/
#pragma once

#pragma pack(push, 1) // To avoid adding paddings in between
struct bmp_header
{
    char id1;
    char id2;
    unsigned int file_size;
    unsigned int reserved;
    unsigned int bmp_data_offset;
    unsigned int bmp_header_size;
    unsigned int width;
    unsigned int height;
    unsigned short int planes;
    unsigned short int bits_per_pixel;
    unsigned int compression;
    unsigned int bmp_data_size;
    unsigned int h_resolution;
    unsigned int v_resolution;
    unsigned int colors;
    unsigned int important_colors;
};
#pragma pack(pop)

class Bitmap
{
public:
    // Create a bitmap by loading its data from a file
    Bitmap(const char* filename);
    ~Bitmap();

    // display some bitmap metadata
    void info() const;

    // trivial accessors
    unsigned char* raw_data() const { return data; }
    unsigned int bitmap_size() const { return size; }
    unsigned int height() const { return header.height; }
    unsigned int width() const { return header.width; }

private:
    unsigned int bmp_read(const char* filename);

    bmp_header header;
    unsigned char* data;
    unsigned int size;
    unsigned int file_size;
};