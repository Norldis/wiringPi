/*
 * isr.c:
 *	Wait for Interrupt test program - ISR method
 *
 *	How to test:
 *	  Use the SoC's pull-up and pull down resistors that are avalable
 *	on input pins. So compile & run this program (via sudo), then
 *	in another terminal:
 *		gpio mode 0 up
 *		gpio mode 0 down
 *	at which point it should trigger an interrupt. Toggle the pin
 *	up/down to generate more interrupts to test.
 *
 * Copyright (c) 2013 Gordon Henderson.
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://github.com/WiringPi/WiringPi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <signal.h>

//**********************************************************************************************************************

// globalCounter:
//	Global variable to count interrupts
//	Should be declared volatile to make sure the compiler doesn't cache it.
static volatile int globalCounter [8] ;

static int terminate_process = 0;

//**********************************************************************************************************************

static void Signal_handler(int sig);

//**********************************************************************************************************************

/*
 * myInterrupt:
 *********************************************************************************
 */

void myInterrupt0 (int pin) { ++globalCounter [0] ; }
void myInterrupt1 (int pin) { ++globalCounter [1] ; }
void myInterrupt2 (int pin) { ++globalCounter [2] ; }
void myInterrupt3 (int pin) { ++globalCounter [3] ; }
void myInterrupt4 (int pin) { ++globalCounter [4] ; }
void myInterrupt5 (int pin) { ++globalCounter [5] ; }
void myInterrupt6 (int pin) { ++globalCounter [6] ; }
void myInterrupt7 (int pin) { ++globalCounter [7] ; }


/*
 *********************************************************************************
 * main
 *********************************************************************************
 */

int main (void)
{
  int gotOne, pin ;
  int myCounter [8] ;

  for (pin = 0 ; pin < 8 ; ++pin)
  {
    globalCounter [pin] = myCounter [pin] = 0 ;
  }

  if (wiringPiSetup() < 0)
  {
    fprintf(stderr, "Unable to setup wiringPi: %s\n", strerror(errno));
    return 1;
  }


  wiringPiISR (0, INT_EDGE_FALLING, &myInterrupt0) ;
  wiringPiISR (1, INT_EDGE_FALLING, &myInterrupt1) ;
  wiringPiISR (2, INT_EDGE_FALLING, &myInterrupt2) ;
  wiringPiISR (3, INT_EDGE_FALLING, &myInterrupt3) ;
  wiringPiISR (4, INT_EDGE_FALLING, &myInterrupt4) ;
  wiringPiISR (5, INT_EDGE_FALLING, &myInterrupt5) ;
  wiringPiISR (6, INT_EDGE_FALLING, &myInterrupt6) ;
  wiringPiISR (7, INT_EDGE_FALLING, &myInterrupt7) ;

  // Set the handler for SIGTERM (15)
  signal(SIGTERM, Signal_handler);
  signal(SIGHUP, Signal_handler);
  signal(SIGINT, Signal_handler);
  signal(SIGQUIT, Signal_handler);
  signal(SIGTRAP, Signal_handler);
  signal(SIGABRT, Signal_handler);
  signal(SIGALRM, Signal_handler);
  signal(SIGUSR1, Signal_handler);
  signal(SIGUSR2, Signal_handler);

  while (!terminate_process)
  {
    gotOne = 0 ;
    printf("Waiting ... ");
    fflush(stdout);

    while (!terminate_process)
    {
      for (pin = 0 ; pin < 8 ; ++pin)
      {
        if (globalCounter [pin] != myCounter [pin])
        {
          printf (" Int on pin %d: Counter: %5d\n", pin, globalCounter [pin]) ;
          myCounter [pin] = globalCounter [pin] ;
          ++gotOne ;
        }
      }
      if (gotOne != 0)
        break ;
    }
  }

  return 0;
}

//**********************************************************************************************************************

/**
 * Intercepts and handles signals from QNX
 * This function is called when the SIGTERM signal is raised by QNX
 */
void Signal_handler(int sig) {
  printf("Received signal %d\n", sig);

  // Signal process to exit.
  terminate_process = 1;
}

//**********************************************************************************************************************
