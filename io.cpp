// io.cpp
// Implementation of hardware I/O operations for LED Display Board

#include "io.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

// Path to the emulated I/O file used for hardware communication simulation
const char* IO_FILE = "C:\\emu8086.io";

// READ_IO_BYTE
// Reads a single byte from the specified I/O port
unsigned char READ_IO_BYTE(long port) {
    unsigned char value = 0;
    
    // Open I/O file in binary read mode
    std::fstream io_file(IO_FILE, std::ios::in | std::ios::binary);

    // Check if file opened successfully
    if (!io_file) {
        throw std::runtime_error("Cannot read I/O file. Please ensure 'C:\\emu8086.io' file exists. "
                               "Make sure that you are running the program with administrator privileges.");
    }

    // Seek to port position and read one byte
    io_file.seekg(port, std::ios::beg);
    io_file.read(reinterpret_cast<char*>(&value), 1);
    io_file.close();
    
    return value;
}


// WRITE_IO_BYTE
// Writes a single byte to the specified I/O port-
void WRITE_IO_BYTE(long port, unsigned char value) {
    // Open I/O file in binary read/write mode
    std::fstream io_file(IO_FILE, std::ios::in | std::ios::out | std::ios::binary);

    // Check if file opened successfully
    if (!io_file) {
       throw std::runtime_error("Cannot write to I/O file. Please ensure 'C:\\emu8086.io' file exists. "
                              "Make sure that you are running the program with administrator privileges.");
    }

    // Seek to port position and write one byte
    io_file.seekp(port, std::ios::beg);
    io_file.write(reinterpret_cast<const char*>(&value), 1);
    io_file.close();
}

// READ_IO_WORD
// Reads a 16-bit word from consecutive I/O ports
short int READ_IO_WORD(long port) {
    // Read low and high bytes separately
    unsigned char low = READ_IO_BYTE(port);
    unsigned char high = READ_IO_BYTE(port + 1);
    
    // Combine bytes into a 16-bit word
    return static_cast<short int>((high << 8) | low);
}


// WRITE_IO_WORD
// Writes a 16-bit word to consecutive I/O ports
void WRITE_IO_WORD(long port, short int value) {
    // Split word into low and high bytes
    unsigned char low = static_cast<unsigned char>(value & 0xFF);
    unsigned char high = static_cast<unsigned char>((value >> 8) & 0xFF);
    
    // Write bytes to consecutive ports
    WRITE_IO_BYTE(port, low);
    WRITE_IO_BYTE(port + 1, high);
}
