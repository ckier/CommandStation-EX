/* This is an automation example file.
 *  The presence of a file calle "myAutomation.h" brings EX-RAIL code into
 *  the command station.
 *  The auotomation may have multiple concurrent tasks. 
 *  A task may 
 *  - Act as a ROUTE setup macro for a user to drive over 
 *  - drive a loco through an AUTOMATION 
 *  - automate some cosmetic part of the layout without any loco.
 *  
 *  At startup, a single task is created to execute the first 
 *  instruction after E$XRAIL. 
 *  This task may simply follow a route, or may START  
 *  further tasks (thats is.. send a loco out along a route).
 *  
 *  Where the loco id is not known at compile time, a new task 
 *  can be creatd with the command:
 *  </ START [cab] route> 
 *  
 *  A ROUTE, AUTOMATION or SEQUENCE are internally identical in ExRail terms  
 *  but are just represented differently to a Withrottle user:
 *  ROUTE(n,"name") - as Route_n .. to setup a route through a layout
 *  AUTOMATION(n,"name") as Auto_n .. to send the current loco off along an automated journey
 *  SEQUENCE(n) is not visible to Withrottle.
 *  
 */

EXRAIL   // myAutomation must start with the EXRAIL instruction
  // This is the default starting route, AKA SEQUENCE(0)  
  SETLOCO(3)   // set current loco id... 
  START(1)  // send current loco off along route 1
  SETLOCO(10)  // set current loco id... 
  START(2)  // send current loco off along route 2
  DONE     // This just ends the startup thread, leaving 2 others running.

/* SEQUENCE(1) is a simple shuttle between 2 sensors      
 *  S10 and S11 are sensors pre-defined with the <S> command
 *  S10                    S11                   
 *  === START->================
 */
   SEQUENCE(1) 
     DELAY(10000)   // wait 10 seconds
     FON(3)       // Set Loco Function 3, Horn on
     DELAY(1000)    // wait 1 second
     FOFF(3)      // Horn off
     FWD(80)      // Move forward at speed 80
     AT(11)       // until we hit sensor id 11
     STOP         // then stop
     DELAY(5000)    // Wait 5 seconds
     FON(2)       // ring bell
     REV(60)      // reverse at speed 60
     AT(10)       // until we get to S10
     STOP         // then stop
     FOFF(2)      // Bell off 
     FOLLOW(1)    // and follow sequence 1 again
   
/* SEQUENCE(2) is an automation example for a single loco Y shaped journey
 *  S1,S2,S3 are sensors, T4 is a turnout
 *  
 *  S3                      T4                            S1
 *  ===-START->=============================================
 *                          //
 *  S2                     //
 *  ======================//
 *  
 *  Train runs from START to S1, back to S2, again to S1, Back to start.
 */
  SEQUENCE(2)
   FWD(60)     // go forward at DCC speed 60 
   AT(1) STOP  // when we get to sensor 1 
   DELAY(10000)  // wait 10 seconds 
   THROW(4)    // throw turnout for route to S2
   REV(45)     // go backwards at speed 45
   AT(2) STOP  // until we arrive at sensor 2
   DELAY(5000)   // wait 5 seconds
   FWD(50)     // go forwards at speed 50
   AT(1) STOP  // and stop at sensor 1
   DELAY(5000)   // wait 5 seconds 
   CLOSE(4)    // set turnout closed
   REV(50)     // reverse back to S3
   AT(3) STOP
   DELAY(20000)  // wait 20 seconds 
   FOLLOW(2)   // follow sequence 2... ie repeat the process
   
   ENDEXRAIL    // marks the end of the EXRAIL program. 
    
   