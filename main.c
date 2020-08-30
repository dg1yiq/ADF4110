/*
 
 ATMega8 ADF4110 PLL

 MIT License

 Copyright (c) 2020 Marco Reinke, DG1YIQ

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
*/

#include "main.h"

//Hauptprogramm
int main(void)
{
    LED_BEAT_DDR |= (1 << LED_BEAT_PIN);        // Beat LED Output
    LED_BEAT_PORT &= ~(1 << LED_BEAT_PIN);      //Beat LED aus
    
    //SPI Pins initialisieren
    spiinit();
    
    //PLL initialisieren
    pllinit(Frequenz);
    
    //
    //Hauptschleife
    //
    
    while(1)
    {
        /*
         //Hier tun wir nicht, außer ein bischen geblinke :-)
        LED_BEAT_PORT |= (1 << LED_BEAT_PIN);   //LED an
        _delay_ms(100);
        LED_BEAT_PORT &= ~(1 << LED_BEAT_PIN);  //LED aus
        _delay_ms(100);
        LED_BEAT_PORT|= (1 << LED_BEAT_PIN);    //LED an
        _delay_ms(100);
        LED_BEAT_PORT&= ~(1 << LED_BEAT_PIN);   //LED aus
        _delay_ms(700);
        //ggf den MC in den Sleep Mode versetzen, aus dem er nicht wieder aufwacht, um Störungen durch diesen zu minimieren.
        */
        
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);    //Sleep Mode definieren
        sleep_mode();                           //Sleep Mode aktivieren
    
	}
    return 0;
}

void spiinit(void)
{
    //Alle SPI Pin's als Ausgang definieren
    //Die Zuordnung zu den HW Pins erfolgt in der main.h
    SPI_MOSI_DDR    |= (1 << SPI_MOSI_PIN);
    SPI_SCK_DDR     |= (1 << SPI_SCK_PIN);
    SPI_ENA_DDR     |= (1 << SPI_ENA_PIN);
    
    //Alles Pins Low
    SPI_MOSI_PORT   &= ~(1 << SPI_MOSI_PIN);    //Data Low
    SPI_SCK_PORT    &= ~(1 << SPI_SCK_PIN);     //Clock Low
    SPI_ENA_PORT    &= ~(1 << SPI_ENA_PIN);     //Enable Low
    return;
}

void pllinit(uint32_t freq)
{
    uint32_t    MUX=MuxPin;
    uint32_t    R;
    uint32_t    A;
    uint32_t    B;
    
    //Brechne Register R,A und B anhand der in der main.h festgelegten Parameter
   
    R=(Referenz/Step);
    B=(freq/Step)/Prescaler;
    A=(freq/Step)-(Prescaler*B);
    
    //Kurze Pause
    _delay_ms(1);
    
    //Übertragung der Register an den ADF4110 - Wichtig: zum schreiben der Register muss der ADF im Counter/Reset Modus sein.
    spiwrite((Prescaler_Reg<<R2_Prescaler) | (MUX<<R2_MUX) | (1<<R2_Counter_Reset) | (1<<R2_PD_Polarity) | 0x2UL );     //R2 - Counter Reset = 1
    spiwrite((R<<R0_R_Counter) | 0x0UL );                                                                               //R0 - R-Counter
    spiwrite((B<<R1_B_Counter) | (A<<R1_A_Counter) | 0x1UL );                                                           //R1 - A+B-Counter
    spiwrite((Prescaler_Reg<<R2_Prescaler) | (MUX<<R2_MUX) | (1<<R2_PD_Polarity) | 0x2UL);                              //R2 - Counter Reset = 0
}

void spiwrite(uint32_t byte)
{
    //Software SPI Routine - SPI BitBanging

    uint8_t count;
    
    //Der ADF4110 hat 24bit große Register - Zähler 0 bis 23
    for (count=0; count<24 ; count++)
    {
        //Prüfe ausmaskiertes DB23 ob 1 oder 0
        if (byte & (1UL << 23))
        {
            //Wenn Bit 1 dann den SPI MOSI Pin High setzen
            SPI_MOSI_PORT |= (1 << SPI_MOSI_PIN);
        } else {
            //Wenn Bit 0 dann den SPI MOSI Pin Low setzen
            SPI_MOSI_PORT &= ~(1 << SPI_MOSI_PIN);
        }
        //Am Ende eines Bites Clock High
        SPI_SCK_PORT |= (1 << SPI_SCK_PIN); // Clock High
        //Prüfe ob wir uns am Ende der Übertragung befinden
        if (count == 23)
        {
            //Ende des Bytes/letztes Bit -> kurz SPI ENA Pin HighLow
            //Achtung ist der Mikrocontroller zu schnell kann der Impuls für den ADF zu "kurz" sein... ggf Delay einfügen
            SPI_ENA_PORT |= (1 << SPI_ENA_PIN); // Enable High
            //_delay_us(10);
            SPI_ENA_PORT &= ~(1 << SPI_ENA_PIN); //Enable Low
        }
        //SPI Clock wieder Low
        SPI_SCK_PORT &= ~(1 << SPI_SCK_PIN); //Clock Low
        //Rotiere das zu sendende Byte ein Bit nach Links sodass das nächste zu sendende Bit an die Stelle DB23 kommt
        byte<<=1;
    }
    //Ende der Übertragung - SPI MOSI low
    SPI_MOSI_PORT &= ~(1 << SPI_MOSI_PIN);
    
    return;
}

