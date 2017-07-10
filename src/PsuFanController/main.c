/*
* Power Supply Unit Fan Controller 
* Copyright (c) 2017 bbtinkerer
*
* Simple ATtiny85 fan controller to turn a fan on/off based on temperature. 
* Includes an LED as a temperature indicator. LED is dim at start of fan on
* temperature and blinks when above a max temperature. Fan is not PWM 
* controlled since I am using a small 5V fan which is quiet running at 100%. 
* The controller is in sleep state while the temperature is below the minimum 
* threshold and wakes up every ~8 seconds to recheck the temperature.
* When temperature is above minimum threshold, the controller will stay
* awake checking every second till the temperature falls below the minimum
* threshold. Uses ds18b20 library by Davide Gironi.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "ds18b20.h"

#define PWM_FULL 255
#define FAN_ON_TEMPERATURE 35
#define FAN_OFF_TEMPERATURE 30
#define MAX_TEMPERATURE 45

int fanOn = 0;

void enableSleep();

int main(void)
{
   double currentTemperature = 0;
   int dutyCycle = 0;
   // Set ports to output
   // PB0 - PWM LED indicator
   // PB4 - Fan
   // not shown PB3, set in ds18b20.h for temperature sensor
   DDRB = (1 << PB4) | (1 << PB0);
   PORTB = (1 << PB0);
   
   // interweb said unused ports should not be floating
   // PB1 and PB2 unused, set to input (default)
   // and enable the internal pull-up resistor
   PORTB |= (1 << PB1) | (1 << PB2);
   
   // turn off ADC since not using it
   ADCSRA &= ~(1 << ADEN);
   // datasheet said analog comparator can be turned off, set ACD to 1
   // said to turn off interrupt before turning ACD off
   ACSR &= ~(1 << ACIE);
   ACSR |= (1 << ACD);
   
   // disable interrupts
   cli();
   
   // MCU Status Register - clear watchdog timer reset flag
   MCUSR &= ~(1 << WDRF);
   // Watchdog Timer Control Register - watchdog change enable, watchdog enable
   WDTCR |= ((1 << WDCE) | (1 << WDE));
   // Watchdog Timer Control Register - enable interrupt, set to 8s
   WDTCR = ((1 << WDIE) | (1 << WDP3) | (1 << WDP0));// | (1 << WDP1) | (1 << WDP0));
   
   while (1)
   {
      currentTemperature = ds18b20_gettemp();

      if(currentTemperature >= FAN_OFF_TEMPERATURE)
      {
         if(currentTemperature >= FAN_ON_TEMPERATURE && !fanOn)
         {
            // turn fan on full, only 5v and pretty quiet so no need to PWM
            PORTB |= (1 << PB4);
            fanOn = 1;
         }
         
         if(fanOn)
         {
            if(currentTemperature >= MAX_TEMPERATURE)
            {
               // Fast PWM reset, set ports to normal operation
               TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0));
               // turn off PWM
               TCCR0B &= ~(1 << CS00);
               // flash LED indicator
               PORTB ^= (1 << PB0);
            }
            else
            {
               // turn on LED as temperature indicator by brightness
               // scale dutyCycle from range from ON to MAX temp
               dutyCycle = (currentTemperature-FAN_OFF_TEMPERATURE) / (MAX_TEMPERATURE-FAN_OFF_TEMPERATURE) * PWM_FULL;
               if(dutyCycle > MAX_TEMPERATURE)
               {
                  dutyCycle = MAX_TEMPERATURE;
               }
               OCR0A = dutyCycle;
               
               // turn the PWM back on
               TCCR0A |= (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
               TCCR0B |= (1 << CS00);
            }
            
            // take a reading again after a second
            _delay_ms(1000);
         }
      }
      else
      {
         fanOn = 0;
      }
      
      if(!fanOn)
      {
         // Fast PWM reset, set ports to normal operation
         TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0));
         // set ports low/off
         TCCR0B &= ~(1 << CS00);
         PORTB &= ~((1 << PB0) | (1 << PB4));
         enableSleep();
      }
   }
}

ISR(WDT_vect)
{
   // disable sleep on wakeup
   sleep_disable();
   
   // disable interrupts
   cli();
}

void enableSleep()
{
   set_sleep_mode(SLEEP_MODE_PWR_DOWN);
   sleep_bod_disable();
   sleep_enable();
   //enable interrupts
   sei();
   sleep_cpu();
}