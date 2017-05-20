// NAME:    Brian Be
// EMAIL:   bebrian458@gmail.com
// ID:  204612203

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <mraa/aio.h>   //temp sensor
#include <mraa/gpio.h>  //btn

const int B = 4275;               // B value of the thermistor
const int R0 = 100000;            // R0 = 100k


int main(int argc, char *argv[]){
    
    // Initialize grove
    mraa_aio_context pinTempSensor = mraa_aio_init(0);     // Grove - Temperature Sensor connect to A0
    mraa_gpio_context btn = mraa_gpio_init(3);             // Grove - Button connect to D3
    mraa_gpio_dir(btn,1);

    while(1){

        // Check for button
        if(mraa_gpio_read(btn)){
            fprintf(stderr, "button read\n");
            exit(0);
        }

        // Read Temperature
        int a = mraa_aio_read(pinTempSensor);
        double R = 1023.0/((double)a)-1.0;
        R = R0*R;

        double celsius = 1.0/(log(R/R0)/B+1/298.15)-273.15; // convert to celsius temperature via datasheet
        double fahrenheit = celsius * 9/5 + 32;

        fprintf(stdout, "%.1f\n", fahrenheit);

        sleep(1);
    }

    mraa_aio_close(pinTempSensor);

}