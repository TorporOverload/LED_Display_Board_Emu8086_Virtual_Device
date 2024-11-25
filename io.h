// io.h
// I/O interface for LED Display Board

#pragma once

// Port Definitions\\ Unneccesary
//const long STATUS_PORT = 0x00;
//const long SPEED_PORT = 0x01;
//const long DATA_PORT_START = 0x02;
//const unsigned char DATA_TERMINATOR = '\0';
//const unsigned char EXIT_STATUS = 0xFF;

/**
 * @brief Reads a single byte from specified I/O port
 * @param port Port address to read from
 * @return Byte value read from port
 */
unsigned char READ_IO_BYTE(long port);

/**
 * @brief Reads a 16-bit word from specified I/O port
 * @param port Starting port address (uses port and port+1)
 * @return Word value read from ports
 */
short int READ_IO_WORD(long port);

/**
 * @brief Writes a single byte to specified I/O port
 * @param port Port address to write to
 * @param value Byte value to write
 */
void WRITE_IO_BYTE(long port, unsigned char value);

/**
 * @brief Writes a 16-bit word to specified I/O port
 * @param port Starting port address (uses port and port+1)
 * @param value Word value to write
 */
void WRITE_IO_WORD(long port, short int value);
