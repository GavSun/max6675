// =========================================================================================
// Project:   Interfacing the adc MAX6675 Module with RP2350 (i.e. RPi PICO2 board)
// Description: This FW use the RP2350 & it's SPI peripheral to communicate with the MAX6675 module.
//              The main.c file is only for testing the code prepared for the MAX6675 interface.
//              Relevant code & header files are:
//              - max6675.c & max6675.h
// Version:   --
// Date:      5th Aug 2025
// Author:    Sunil Gavali
// Github:    https://github.com/GavSun
//  
// ------------------------------------------------------------------------------------------
// Compilation Information:
// Enviornment: VSCode with Rapberry Pi Pico Project extension
// ==========================================================================================

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

#include "max6675_adc.h"

//-----------------------------------------------
// MAX6675 borad connections with Rpi PICO2 board
//-----------------------------------------------
// Max6675 Board             Rpi PICO2 Board
//-----------------------------------------------
// SO - Data Out      ->    GPIO16 : SPI0 RX
// CS - Chip Select*  <-    GPIO17 : CS*
// SCK - Clock        <-    GPIO18 : Clock
// VCC -              <-    3v3
// GND -              <-    Gnd
//-----------------------------------------------

// Define SPI and assign GPIO pins. (not fixded, Both can be changed)
// Used SPI 0, and allocated following GPIO pins to it.
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

int main()
{
    stdio_init_all();

    // SPI initialisation. Use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, set it to a high state.
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
        
    //Now setup MAX6675
    init_max6675(SPI_PORT, PIN_CS);

    uint16_t adc_data;
    while (true) {
        sleep_ms(1000);

        //Get MAX6675 data
        adc_data = read_max6675();

        //Print on terminal (RP PICO2 USB port is used as terminal)
        printf("Temperature = %d\n",adc_data);
    } //while()
} //main()
