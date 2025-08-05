#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

#include "max6675_adc.h"

//---------------------------------------------------------
// Local parameters defined here
//---------------------------------------------------------

// Copy of CS pin number
uint8_t	PIN_CS_MAX6675;
// Local copy of I2C peripheral instance
static spi_inst_t *spi_handle;

// Local variables
uint8_t adc_reading_counter = 0;
int16_t adc_buffer[4];

//---------------------------------------------------------
// Interface functions start here
//---------------------------------------------------------

/**
 * @brief   Returns the temperature measured by the Thermocouple connected to MAX6675 board.
 * @note    The returned value is average of 4 readings (present + Previous 3).
 *          The adc MAX6675 is read only once but for averaging purpose this function maintains 
 *          three readings in local buffer.
 *          Therefore, the temperature measured by this function will initially lag 
 *          behind the tempetaure being sensed by the thermocouple sensor.
 *          
 * @param   None
 * @retval  Temperature in degree Centigrade (from 0 to 1023.75 deg. Cen.). 
 * 
*/
uint16_t read_max6675(void){

    uint16_t adc_average;   //Average of 4 consecutive MAX6675 readings
    int num_bytes_read;
    uint16_t buf[1];        //For temporary storage of MAX6675 ouput
    
    //read temperature from MAX6675 ADC over SPI bus
    //The adc value is stored in buf[]
    gpio_put(PIN_CS_MAX6675, 0);        //SPI CS pin low
    num_bytes_read = spi_read16_blocking(spi_handle, 0, buf, 1);
    gpio_put(PIN_CS_MAX6675, 1);        //SPI CS pin High
    
    // Note - Max6675 ADC ouput is 16 bit wide:
    // Bit 15 = Dummy always 0
    // Bit 14 to Bit 3 = 12 bit Temperature reading
    // Bit 2 = 1 if no thermocoupe connected else 0 always
    // bit 1 = 0 always (device ID?)
    // bit 0 = ignore it as it is tri state ouput
    // Therefore received ADC count needs to be shifted 3 bit to rightside.
    adc_buffer[adc_reading_counter] = buf[0]>>3;

    // In order to reduce the fluctuations present in MAX6675 ADC readings
    // average of 4 consequtive ADC reading is performed
    adc_average = adc_buffer[0] + adc_buffer[1] + adc_buffer[2] + adc_buffer[3];
    adc_average = adc_average >> 2; //Shift right side 2 bits = Divide by 4 = average 4 readings

    //manage the ADC reading counter
    if(adc_reading_counter < 3){
        adc_reading_counter++;
    }else{
        adc_reading_counter = 0;
    }

    // Max6675 ADC Output value Scaling:
    // All bits = zero means the thermocouple reading is 0°C. 
    // All bits = ones means the thermocouple reading is +1023.75°C.
    // as the ADC 12 bit max value = 0xFFF = 4095, 1023.75/4095 = 0.25deg cen./bit
    return((uint16_t)((float)adc_average * 0.25f));
}

/**
 * @brief   Initialises the ADC MAX6675 and relevant variables required for temperature measurement.
 * @param   spi_used    the handle to SPI peripheral which is already set up by the calling function
 * @param   number      The GPIO pin number of SPI_CS pin.
 * @retval  None
 * @note    The Max6675 essentially needs no configuration.
 * 
 */
void init_max6675(spi_inst_t *spi_used, unsigned char number){
    
    // Note - 
    // The SPI peripheral is already setup and the
    // the GPIO pins for MISO, CS, CLK & MOSI are already configured by rhe main()
    
    //Store the received I2C handle & GPIO number of SPI CSpin
	spi_handle = spi_used;
    PIN_CS_MAX6675 = number;
    
   // Set SPI format
    spi_set_format( spi_handle,   // SPI instance
                    16,      // Number of bits per transfer
                     1,      // Polarity (CPOL)
                    1,      // Phase (CPHA)
                    SPI_MSB_FIRST);

    //Clean storage area to avoid ambeguity
    adc_reading_counter = 0;
    adc_buffer[0] =0;
    adc_buffer[1] =0;
    adc_buffer[2] =0;
    adc_buffer[3] =0;
}
