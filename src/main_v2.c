/*
 * File:   main.c
 * Author: Nicolas
 *
 * Created on 9 octobre 2022, 13:42
 */

#define _XTAL_FREQ 3276800

#include <xc.h>

// BEGIN CONFIG
#pragma config FOSC     = XT    // Oscillator Selection bits
#pragma config WDTE     = OFF   // Watchdog Timer Enable bit
#pragma config PWRTE    = ON    // Power-up Timer Enable bit
#pragma config BOREN    = ON    // Brown-out Reset Enable bit
#pragma config LVP      = OFF   // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB4 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD      = OFF   // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config CP       = OFF   // Flash Program Memory Code Protection bit (Code protection off)
//END CONFIG

void init_IO() {
    
    // Disable comparators
    CMCON = 7;
    
    // Set nixie display outputs
    TRISB0 = 0; //RB0 (bit 1 for digit) as Output PIN
    TRISB1 = 0; //RB1 (bit 2 for digit) as Output PIN
    TRISB2 = 0; //RB2 (bit 3 for digit) as Output PIN
    TRISB3 = 0; //RB3 (bit 4 for digit) as Output PIN
    
    // Set multiplex outputs
    // + : high digit of the number
    // - : lower digit of the number
    TRISA0 = 0; //RA0 (H+) as Output PIN
    TRISA1 = 0; //RA1 (H-) as Output PIN
    TRISA2 = 0; //RA2 (M+) as Output PIN
    TRISA3 = 0; //RA3 (H-) as Output PIN
    
    // Set Neon light output
    TRISB4 = 0; //RB4 as Output PIN
    
    // Set time setters inputs
    TRISB7 = 1; //RB7 (H++) as Input PIN
    TRISB6 = 1; //RB6 (M++) as Input PIN

    // Clock input
    TRISB5 = 1; //RB5 as Input PIN
}

void init_digits() {

    RB4 = 0 ;   // Activate led blink (initial state: on)

    RB0 = 0;    //RB0 (bit 1 for digit) set to 0
    RB1 = 0;    //RB1 (bit 2 for digit) set to 0
    RB2 = 0;    //RB2 (bit 3 for digit) set to 0
    RB3 = 0;    //RB3 (bit 4 for digit) set to 0
    
    RA0 = 1;    //RA0 (H+) disabled
    RA1 = 1;    //RA1 (H-) disabled
    RA2 = 1;    //RA2 (M+) disabled
    RA3 = 1;    //RA3 (H-) disabled
}

// Encode digit on 4-bit output
void set_digit(int num) {
    RB0= (num >> 0) & 1;
    RB1= (num >> 1) & 1;
    RB2= (num >> 2) & 1;
    RB3= (num >> 3) & 1;
}

// Disable all tube and enable one with the selected number
void exclusively_activate_tube_with_digit(int tube, int num) {

    // disable all tubes
    RA0=1;
    RA1=1;
    RA2=1;
    RA3=1;

    // set digit number
    set_digit(num);

    // enable tube 
    switch (tube) {
        case 0:
            RA0=0;
            break;
        case 1:
            RA1=0;
            break;
        case 2:
            RA2=0;
            break;
        case 3:
            RA3=0;
            break;
    }

}

// Test sequence
void digit_demo() {
    
    // cool countdown
    for (int i = 0; i <= 3; i++) {
        exclusively_activate_tube_with_digit(i, 3 - i);
        RB4 = (1-RB4);
        __delay_ms(1000);
    }
    
    // digits multiplex
    int i = 0;
    while (i <= 400) // 400*5ms : 2secs
    {
        // update digit & multiplex 
        exclusively_activate_tube_with_digit(i % 4, i / 40);
        
        // led blink
        RB4 = (i / 100) % 2;
        
        i++;
        __delay_ms(5);
    }
}

void main(void) {
    
    // Init pin states
    init_IO();
    
    // Init digits
    init_digits();
    
    // Launch demo (tubes / neon tests)
    digit_demo();

    // Initialize variables
    int hours = 0;
    int minutes = 0;
    int digit = 0;   // digit (tube) currently enabled

    // Clock state memory
    int clock_pin_state = 0;
    
    // Initialize timers
    int minutes_diff = 0;

    int blink_diff = 0;

    int hours_plus_diff = 0;
    int hours_plus_acc_diff = 0;
    int minutes_plus_diff = 0;
    int minutes_plus_acc_diff = 0;

    while (1) {

        // Multiplexing display (each tube has a refresh rate of 50Hz, the loop refreshs at 200Hz)
        switch (digit) {
            case 0:
                // Hours tens digit on tube #0
                exclusively_activate_tube_with_digit(digit, hours / 10);
                digit++;
                break;
            case 1:
                // Hours ones digit on tube #1
                exclusively_activate_tube_with_digit(digit, hours % 10);
                digit++;
                break;
            case 2:
                // Minutes tens digit on tube #2
                exclusively_activate_tube_with_digit(digit, minutes / 10);
                digit++;
                break;
            case 3:
                // Minutes ones digit on tube #0
                exclusively_activate_tube_with_digit(digit, minutes % 10);
                digit = 0;
                break;
        }

        
        // Neon blink based on the clock input
        RB4 = RB5;


        // Hours addition
        if (RB7 == 0) {
            int cap = 200; // update every seconds

            // divide the update time if the button is pressed long enough
            hours_plus_acc_diff++;
            if (hours_plus_acc_diff >= 800) {

                cap /= 3;

                if (hours_plus_acc_diff >= 1600)
                    cap /= 3;

            }

            if (hours_plus_diff >= cap) {
                hours_plus_diff = 0;

                hours++;
                minutes_diff = 0;

                if (hours >= 24)
                    hours = 0;
                
            } else
                hours_plus_diff++;

        } else {
            hours_plus_diff = 0;
            hours_plus_acc_diff = 0;
        }


        // Minutes additions
        if (RB6 == 0) {

            int cap = 200; // update every seconds

            // divide the update time if the button is pressed long enough
            minutes_plus_acc_diff++;
            if (minutes_plus_acc_diff >= 800) {

                cap /= 3;

                if (minutes_plus_acc_diff >= 1600)
                    cap /= 3;

            }

            if (minutes_plus_diff >= cap) {
                minutes_plus_diff = 0;

                minutes++;
                minutes_diff = 0;

                if (minutes >= 60)
                    minutes = 0;
                
            } else
                minutes_plus_diff++;

        } else {
            minutes_plus_diff = 0;
            minutes_plus_acc_diff = 0;
        }

        // Minute increment based on clock input
        // each time the square signal is in a low state, we count one second
        if (RB5 == 0 && RB5 != clock_pin_state)
        {
            minutes_diff++;
        }

        clock_pin_state = RB5;

        // Time increment
        if (minutes_diff >= 60) {
            minutes_diff = 0;

            minutes++;
            if (minutes >= 60) {
                minutes=0;
                hours++;
                if (hours >= 24)
                    hours = 0;
            }
        }

        __delay_ms(5);
    }
}
