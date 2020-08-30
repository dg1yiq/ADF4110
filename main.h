#ifndef F_CPU
#define F_CPU 1000000
#endif

#ifndef MAIN_H
#define MAIN_H

#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <util/delay.h>
#include <avr/cpufunc.h>
#include <avr/sleep.h>

//Prototypen
void spiwrite(uint32_t byte);
void spiinit(void);
void pllinit(uint32_t freq);

//Definitionen PLL SPI Interface
#define     SPI_SCK_PORT    PORTB
#define     SPI_SCK_DDR     DDRB
#define     SPI_SCK_PIN     PB2
#define     SPI_MOSI_PORT   PORTB
#define     SPI_MOSI_DDR    DDRB
#define     SPI_MOSI_PIN    PB0
#define     SPI_ENA_PORT    PORTB
#define     SPI_ENA_DDR     DDRB
#define     SPI_ENA_PIN     PB3

//Definition LED's
#define     LED_BEAT_PORT   PORTB
#define     LED_BEAT_DDR    DDRB
#define     LED_BEAT_PIN    PB4

//PLL Definitionen
#define     R0_R_Counter        2
#define     R0_Anti_Backlash    16
#define     R0_LDP              20
#define     R0_SYNC             21
#define     R0_DLY              22

#define     R1_A_Counter        2
#define     R1_B_Counter        8
#define     R1_CP_Gain          21

#define     R2_Counter_Reset    2
#define     R2_Power_Down_1     3
#define     R2_MUX              4
#define     R2_PD_Polarity      7
#define     R2_CP_Threestate    8
#define     R2_Fastlock         9
#define     R2_Timer_Counter    11
#define     R2_Current_1        15
#define     R2_Current_2        18
#define     R2_Power_Down_2     21
#define     R2_Prescaler        22

#define     Pres_8              0x0UL
#define     Pres_16             0x1UL
#define     Pres_32             0x2UL
#define     Pres_64             0x3UL

//************************************
//PLL Parameter
//FREQUENZ, KANALRASTER, REFERENZ
//Frequenzen sind in Hz
//************************************

#define     Frequenz            144300000
#define     Referenz            25000000
#define     Step                100000
#define     Prescaler           8
#define     Prescaler_Reg       Pres_8
#define     MuxPin              0b001       //MUX Parameter (010 = N Out, 001 = Digital Lock Detect, 100 = R Out)

#endif
