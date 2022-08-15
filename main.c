/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC16F1827
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/


/*
 * jmB
 0.1    July 15th 2022  first version to be tested, implemented simple PTT event trigger and basic sequencing
 *      Missing the switch monitoring feature
 *      Missing the PS monitoring (basic thus adaptative)
 *      Missing PTT OFF when PTT ON sequence not finished
 *      Fully missing TX power monitoring
 *      Starting event_list for forthcoming state event matrix processing 
 */
#include <xc.h>
#include "mcc_generated_files/mcc.h"


#if defined(__XC8__)
// 
#define FIRMWARE_VERSION 0x0001
__IDLOC7(FIRMWARE_VERSION & 0xFF, FIRMWARE_VERSION<<8,0,0);
//#pragma config IDL0C1 =0, IDL0C2=0, IDL0C3=0, IDLOC4=1;
#endif

#define TRUE    1
#define OK      1
#define FALSE   0
#define ERROR   0

#define HIGH    1
#define LOW     0

#define SEQ1_2  15
#define SEQ2_3  15
#define SEQ3_4  15
#define SEQ4_3  15
#define SEQ3_2  15
#define SEQ2_1  15

#define LOWVOLTAGE  11.0     // battery voltage where operation must be shut down when lower
#define FULLSCALE   15.0



enum switch_type {SWITCH_RX_LEFT, SWITCH_RX_RIGHT, SWITCH_NOCOPY};

enum event_list {NO_EVENT,PTT_ON,PTT_OFF,BATT_LOW,MAX_EVENT};

enum state_list {IDLE, EOTXSWITCH, EORXSWITCH, EOSEQ1, EOSEQ2, EOSEQ3, EOSEQ4};


const char program_name[]="Sequencer";
const char program_version[]="0.1";
const char program_date[]="2022/07/15";


// functions prototypes
int SEQ1(int);
int SEQ2(int);
int SEQ3(int);
int SEQ4(int);

float read_battery_voltage();

enum switch_type get_switch_type();
enum event_list get_event();


// global vars
int newevent=0,event=0;
int PTT_current_value;

//------------------------------------------------------------------------------


float read_battery_voltage()
{
    unsigned int    convertedValue;
    
    ADC_Initialize();
    ADC_SelectChannel(IN_BATT_MON);
    ADC_StartConversion();
    convertedValue = ADC_GetConversionResult();
    
    return ( FULLSCALE * convertedValue/1024.0);
}


enum switch_type get_switch_type()
{
    enum switch_type switch_status=SWITCH_NOCOPY;
    
    int port1=0,port2=0;
    
    OUT_REL_TEST_SetHigh();             // set output high to test switch monitoring state pins
    
    port1=IN_REL_POS1_GetValue();       // get return switch pos1
    port2=IN_REL_POS2_GetValue();       // get return switch pos2
    
    if (port1 && !port2) switch_status= SWITCH_RX_LEFT;
    if (port2 && !port1) switch_status= SWITCH_RX_RIGHT;
    
    
    return(switch_status);
}


int SEQ1(int set)
{
    int exit_code = OK;
    
    switch (set) {
        case HIGH :
            OUT_SEQ1_SetHigh();
            break;
            
        case LOW :
            OUT_SEQ1_SetLow();
            break;
            
        default :
            exit_code = ERROR;
            
    }
    
    return(exit_code);
}

int SEQ2(int set)
{
    int exit_code = OK;
    
    switch (set) {
        case HIGH :
            OUT_SEQ2_SetHigh();
            break;
            
        case LOW :
            OUT_SEQ2_SetLow();
            break;
            
        default :
            exit_code = ERROR;
            
    }
    
    return(exit_code);
}

int SEQ3(int set)
{
    int exit_code = OK;
    
    switch (set) {
        case HIGH :
            OUT_SEQ3_SetHigh();
            break;
            
        case LOW :
            OUT_SEQ3_SetLow();
            break;
            
        default :
            exit_code = ERROR;
            
    }
    
    return(exit_code);
}

int SEQ4(int set)
{
    int exit_code = OK;
    
    switch (set) {
        case HIGH :
            OUT_SEQ4_SetHigh();
            break;
            
        case LOW :
            OUT_SEQ4_SetLow();
            break;
            
        default :
            exit_code = ERROR;
            
    }
    
    return(exit_code);
}

enum event_list get_event()
{
    enum event_list local_event;
    
    
    
    // BATT low
    if (read_battery_voltage() < LOWVOLTAGE )
    {
        newevent=TRUE;
        local_event = BATT_LOW;
    }
    
    else
    {
        //PTT change ?
        if (PTT_current_value != IN_PTT_IN_GetValue())      // PTT change ?
        {
            newevent=TRUE;
            if (PTT_current_value == PTT_ON)
                local_event = PTT_OFF;
            else
                local_event = PTT_ON;
            
            
        }
    }
    
   
    return (local_event);
}
/*
                         Main application
 */
int main(void)
{
    enum switch_type sw_type;
    
    // initialize the device
    SYSTEM_Initialize();
    
    OUT_ALARM_SetHigh();
    
    sw_type = get_switch_type();
    PTT_current_value = IN_PTT_IN_GetValue();
    
    while( (PTT_current_value=IN_PTT_IN_GetValue())==LOW);             // wait in alarm state until PTT is released
    
    //PTT_current_value = HIGH;                      // now, fortunately PTT is no more engaged we can proceed with the seq routine

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    //INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();

    OUT_ALARM_SetLow();                             // Alarm unset prior to sequencer "forever"
    
    while (1)
    {
        
        if (get_event())                           // Any new event ?
        {
            switch (event)
            {
                case PTT_ON :
                    SEQ1(HIGH);
                    
                    __delay_ms(SEQ1_2);
                    SEQ2(HIGH);
                    __delay_ms(SEQ2_3);
                    SEQ3(HIGH);
                    __delay_ms(SEQ3_4);
                    SEQ4(HIGH);
                    break;
                    
                case PTT_OFF :    
                    SEQ4(LOW);
                    __delay_ms(SEQ4_3);
                    SEQ3(LOW);
                    __delay_ms(SEQ3_2);
                    SEQ2(LOW);
                    __delay_ms(SEQ2_1);
                    SEQ1(LOW);
                    break;
                    
                case BATT_LOW :
                    OUT_ALARM_SetHigh();
                    SEQ4(LOW);
                    __delay_ms(SEQ4_3);
                    SEQ3(LOW);
                    __delay_ms(SEQ3_2);
                    SEQ2(LOW);
                    __delay_ms(SEQ2_1);
                    SEQ1(LOW);
                    break;
                    
                default :
                    break;
                    
            newevent = FALSE;    
                    
            }
        }
    }
    return(-1);
}
/**
 End of File
*/