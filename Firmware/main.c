/*
 * Name: Jackson Cornell
 * Date completed: ?
 * Author: Jackson Cornell
 * Desc: This will take input from CAN to drive a servo motor via PWM
 *
 *       This will control a digital buck converter via SPI using a
 *       hard-coded value
 *
 * Hardware Notes:
 * PA2-5 - SPI
 * PC4 - PWM output
 * PB1 - CAN RX
 * PF3 - CAN TX
 *
 * As of 12/22/2020 this code is not yet functional
 */

/* INCLUDES */
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/can.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/pwm.h"

//MIL includes
#include "MIL/MIL_CLK.h"
#include "MIL/MIL_CAN.h"
#include "MIL/MIL_SPI.h"

/************************VARIABLES******************************/

//Servo Motor voltage
//Must be between 2.5 V and 12.6 V
#define VOLT 7.4
//Calculated resister value
#define R_SET (54900 * 1.25 / (VOLT - 2.5)) - 6490

//Digital Pot SPI values
//Assumes R_ab = 10000
#define WIPER_CODE 127 - (R_SET * 127 / 10000)
//Addresses
const uint32_t WIPER_ADDR = 0;
const uint32_t TCON_ADDR = 4;
const uint32_t TCON_RAB = 14;
const uint32_t SPI_WRITE = 0;
const uint32_t SPI_READ = 3;

//PWM
const uint32_t PWM_PERIOD = 400;
const uint32_t PWM_DUTY_CYC_DEFAULT = 300;

uint8_t ServoPos;
const uint32_t SPI_CLK = 10000;
const uint32_t SPI_DATA_LEN = 8;

/************************FLAGS******************************/

/************************FUNCTION PROTOTYPES******************************/
void PWM_Init();

/************************MAIN******************************/
int main(void)
{

    //VARIABLES
    uint8_t PWM_duty_cyc = PWM_DUTY_CYC_DEFAULT;
    MIL_CAN_MailBox_t mailbox;
    mil_can_status_t mail_recieved;
    mil_can_status_t mail_retrieved;

    //CONFIGURE SYSTEM CLOCK TO INTERNAL 16MHZ
    MIL_ClkSetInt_16MHz();

    //configure CAN mailbox
    mailbox.canid = 0;          //ALL IDs should be 8 bits long
    mailbox.filt_mask = 0;      //bit mask
    mailbox.base = CAN0_BASE;
    mailbox.msg_len = 1;         //values 1 to 8
    mailbox.obj_num = 1;         //values 1 to 32
    mailbox.rx_flag_int = 0;
    mailbox.buffer = &PWM_duty_cyc;

    //initialize CAN
    MIL_InitCAN(MIL_CAN_PORT_F, CAN0_BASE);
    MIL_InitMailBox(&mailbox);

    //initialize SPI
    MIL_SPI_Init(MIL_SPI_PORTA_MOD0, MIL_SPI_MASTER, SPI_CLK,
                  MIL_CS_MOD_CTRL, SPI_DATA_LEN);

    //initializes PWM
    PWM_Init();

    //Configure digital pot
    MIL_SPIDataPut(MIL_SPI_PORTA_MOD0, WIPER_ADDR);
    while(SSIBusy(MIL_SPI_PORTA_MOD0));
    MIL_SPIDataPut(MIL_SPI_PORTA_MOD0, TCON_ADDR);


     while(1){
         //MIL_SPIDataPut(MIL_SPI_PORTA_MOD0, TCON_ADDR);
         //SysCtlDelay(100);
         //MIL_SPIDataPut(MIL_SPI_PORTA_MOD0, (uint16_t) 32);
        // while(SSIBusy(MIL_SPI_PORTA_MOD0));
         MIL_SPIDataPut(MIL_SPI_PORTA_MOD0, (TCON_ADDR<<10) | (SPI_WRITE<<8) | TCON_RAB);
         mail_recieved = MIL_CAN_CheckMail(&mailbox);
         if (mail_recieved == MIL_CAN_OK){
             mail_retrieved = MIL_CAN_GetMail(&mailbox);
             if (mail_retrieved == MIL_CAN_OK){
                 PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, PWM_duty_cyc);
             }
         }

    }

    return 0;
}

/************************FUNCTIONS******************************/
void PWM_Init()
{
    //Enable pins
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

    GPIOPinConfigure(GPIO_PC4_M0PWM6);
    GPIOPinTypePWM(GPIO_PORTC_BASE, GPIO_PIN_4);

    //Enable PWM
    PWMGenConfigure(PWM0_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    //Set PWM
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_3, PWM_PERIOD);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_6, PWM_DUTY_CYC_DEFAULT);

    PWMGenEnable(PWM0_BASE, PWM_GEN_3);

    PWMOutputState(PWM0_BASE, PWM_OUT_6_BIT, true);
}
