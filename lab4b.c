// NAME:    Brian Be
// EMAIL:   bebrian458@gmail.com
// ID:  204612203

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <mraa/aio.h>   //temp sensor
#include <mraa/gpio.h>  //btn
#include <pthread.h>
#include <ctype.h>

const int B = 4275;               // B value of the thermistor
const int R0 = 100000;            // R0 = 100k

// Flags
int opt_period = 1, opt_log = 0;
char opt_scale = 'F';
int logfd;
FILE* logfile;
mraa_gpio_context btn;


void check_period(char *optarg){
    if(!isdigit(*optarg) || opt_period < 0){
        fprintf(stderr, "Invalid option argument for period. Please use an integer greater than 0 or default value of 1 second\n");
        fprintf(stderr, "%d\n", opt_period);
        exit(1);
    }
}

void check_scale(){
    if(opt_scale != 'F' && opt_scale != 'C'){
        fprintf(stderr, "Invalid option argument for scale. Please use C for Celsius or F for Fahrenheit\n");
        exit(1);
    }
}

void check_logfile(){
    if(logfile == NULL){
        fprintf(stderr, "Error creating or opening\n");
        exit(1);
    }
}

void* check_btn(){

    // Initialize grove - button
    btn = mraa_gpio_init(3);             // Grove - Button connect to D3
    mraa_gpio_dir(btn,1);
    
    while(1){

        // Check for button
        if(mraa_gpio_read(btn)){
            fprintf(stderr, "SHUTDOWN\n");
            exit(0);
        }
    }

    return NULL;
}


int main(int argc, char *argv[]){
    
    // Default values
    int opt = 0;

    struct option longopts[] = {
        {"period",  required_argument,  NULL, 'p'},
        {"scale",   required_argument,  NULL, 's'},
        {"log",     required_argument,  NULL, 'l'},
        {0,0,0,0}
    };

    while((opt = getopt_long(argc, argv, "p:s:l:", longopts, NULL)) != -1){
        switch(opt){
            case 'p':

                opt_period = atoi(optarg);
                check_period(optarg);
                break;
            case 's':
                opt_scale = *optarg;
                check_scale();
                break;
            case 'l':
                opt_log = 1;
                logfile =  fopen(optarg, "w");
                check_logfile();
                break;
            default:
                fprintf(stderr, "Invalid Arguments. Usage: ./lab4b --period=time_in_secs --scale=[FP] --log=filename\n");
                exit(1);
                break;
        }
    }


    // Initialize grove - temp sensor
    mraa_aio_context pinTempSensor = mraa_aio_init(0);     // Grove - Temperature Sensor connect to A0

    // Create thread to check button, avoiding problem of sleeping while button is pressed
    pthread_t btn_thread;
    if(pthread_create(&btn_thread, NULL, check_btn, NULL) < 0){
        fprintf(stderr, "Error creating thread for button\n");
        exit(1);
    }

    // Main thread reads temperature and time
    while(1){

        // Read Temperature
        int a = mraa_aio_read(pinTempSensor);
        double R = 1023.0/((double)a)-1.0;
        R = R0*R;

        double celsius = 1.0/(log(R/R0)/B+1/298.15)-273.15; // convert to celsius temperature via datasheet
        double fahrenheit = celsius * 9/5 + 32;

        // Write to stdout
        if(opt_scale == 'C')    
            fprintf(stdout, "%.1f\n", celsius);
        else
            fprintf(stdout, "%.1f\n", fahrenheit);

        // Write to logfile 
        if(opt_log){

            if(opt_scale == 'C')
                fprintf(logfile, "%.1f\n", celsius);
            else
                fprintf(logfile, "%.1f\n", fahrenheit);

            // Flush to make sure it prints to logfile
            fflush(logfile);
        }

        sleep(opt_period);
    }

    mraa_aio_close(pinTempSensor);

}