#include "mbed.h"
#include "LM75B.h"
#include "C12832.h"
#include "rtos.h"

// add objects for LCD
C12832 lcd(p5, p7, p6, p8, p11);
LM75B sensor(p28,p27);

Serial pc(USBTX,USBRX);

Thread thread;

// Create the local filesystem under the name "local" on mbed
LocalFileSystem local("local");  

// function to write temperature values to file
void write_value() {
    // open file for writing
    FILE *fp = fopen("/local/temperature.txt", "w");

    for (int i = 0; i < 10; i++) {
        // read temperature from sensor
        float temp = sensor.read();

        // write temperature to file
        fprintf(fp, "%.2f\n", temp);

        // wait for 7 minutes = 420 seconds
        wait(420);
    }

    // close file
    fclose(fp);
}

// function to read temperature values from file
void read_file() {
    // create variables for temperature readings
    float minTemp = 100.0;
    float maxTemp = -100.0;
    float totalTemp = 0;
    int numReadings = 0;

    // open file for reading
    FILE *fp = fopen("/local/temperature.txt", "r");

    // read temperature values from file
    while (!feof(fp)) {
        float temp;
        fscanf(fp, "%f", &temp);

        // update statistics
        if (temp < minTemp) {
            minTemp = temp;
        }
        if (temp > maxTemp) {
            maxTemp = temp;
        }
        totalTemp += temp;
        numReadings++;
    }

    // close file
    fclose(fp);

    // calculate average temperature
    float avgTemp = totalTemp / numReadings;

    // display values on LCD
    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("Min Temp: %.2f C", minTemp);
    lcd.locate(0,8);
    lcd.printf("Max Temp: %.2f C", maxTemp);
    lcd.locate(0,16);
    lcd.printf("Avg Temp: %.2f C", avgTemp);
}

// function to display temperature values on LCD
void display_values() {
    while (1) {
        // read temperature from sensor
        float temp = sensor.read();

        // display temperature on the LCD
        lcd.cls();
        lcd.locate(0,3);
        lcd.printf("Temperature: %.2f C", temp);

        // wait for 1 second
        wait(1);
    }
}

int main() {
    // create threads for each function
    Thread write_thread;
    Thread read_thread;
    Thread display_thread;

    // start each thread
    write_thread.start(&write_value);
    read_thread.start(&read_file);
    display_thread.start(&display_values);

    // wait for threads to finish
    write_thread.join();
    read_thread.join();
    display_thread.join();
}
