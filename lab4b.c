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
#include <time.h>

const int B = 4275;               	// B value of the thermistor
const int R0 = 100000;            	// R0 = 100k
const int TIME_DISP_SIZE = 9;		// HH:MM:SS\0

// Flags and default values
int opt_period = 1, opt_log = 0;
char opt_scale = 'F';

// Used to create/open logfile
FILE* logfile;

// Timer variables
time_t timer;
struct tm* converted_time_info;
char *time_disp;


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

    // Initialize Grove - Button connect to D3
    mraa_gpio_context btn = mraa_gpio_init(3);       
    mraa_gpio_dir(btn,1);

    while(1){

        // Constantly read time
	    time(&timer);
	    converted_time_info = localtime(&timer);
	    strftime(time_disp, TIME_DISP_SIZE, "%H:%M:%S", converted_time_info);

		// Constantly check for button
        if(mraa_gpio_read(btn)){
            fprintf(stdout, "%s SHUTDOWN\n", time_disp);
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


    // Initialize Grove - Temperature Sensor connect to A0
    mraa_aio_context pinTempSensor = mraa_aio_init(0);

    // Set time to local time
    setenv("TZ", "PST8PDT", 1);
    tzset();

    // Read time first in case parent executes display before child thread updates time
    time_disp = malloc(sizeof(char) * TIME_DISP_SIZE);
    time(&timer);
    converted_time_info = localtime(&timer);
    strftime(time_disp, TIME_DISP_SIZE, "%H:%M:%S", converted_time_info);

    // Create thread to check button, avoiding problem of sleeping while button is pressed
    // Will also update time to timestamp SHUTDOWN message, even while ain thread is sleeping
    pthread_t btn_thread;
    if(pthread_create(&btn_thread, NULL, check_btn, NULL) < 0){
        fprintf(stderr, "Error creating thread for button\n");
        exit(1);
    }

    // Main thread reads temperature
    while(1){

        // Read Temperature
        int a = mraa_aio_read(pinTempSensor);
        double R = 1023.0/((double)a)-1.0;
        R = R0*R;

        double celsius = 1.0/(log(R/R0)/B+1/298.15)-273.15; // convert to celsius temperature via datasheet
        double fahrenheit = celsius * 9/5 + 32;


        // Generate report to stdout
        if(opt_scale == 'C'){  
            fprintf(stdout, "%s %.1f\n", time_disp, celsius);
        }
        else
            fprintf(stdout, "%s %.1f\n", time_disp, fahrenheit);

        // Generate report to logfile 
        if(opt_log){

            if(opt_scale == 'C')
                fprintf(logfile, "%s %.1f\n", time_disp, celsius);
            else
                fprintf(logfile, "%s %.1f\n", time_disp, fahrenheit);

            // Flush to make sure it prints to logfile
            fflush(logfile);
        }

        sleep(opt_period);
    }

    mraa_aio_close(pinTempSensor);

}