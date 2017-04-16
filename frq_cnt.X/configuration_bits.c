/******************************************************************************
 * 40m band frequency counter with audio CW output
 * 
 * Gerd Bartelt
 * www.sebulli.com
 *                           
 ******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

/******************************************************************************/
/* Configuration Bits                                                         */
/*                                                                            */
/* Refer to your Hi-Tech User Manual in the PICC installation directory       */
/* /doc folder for more information on filling in configuration bits.         */
/* In addition, configuration bit mnemonics can be found in your              */
/* PICC\version\include\<processor name>.h file for your device.  The XC8     */
/* compiler contains documentation on the configuration bit macros within     */
/* the compiler installation /docs folder in a file called                    */
/* pic18_chipinfo.html.                                                       */
/*                                                                            */
/* For additional information about what the hardware configurations mean in  */
/* terms of device operation, refer to the device datasheet.                  */
/*                                                                            */
/* A feature of MPLAB X is the 'Generate Source Code to Output' utility in    */
/* the Configuration Bits window.  Under Window > PIC Memory Views >          */
/* Configuration Bits, a user controllable configuration bits window is       */
/* available to Generate Configuration Bits source code which the user can    */
/* paste into this project.                                                   */
/*                                                                            */
/******************************************************************************/

#pragma config FOSC = EC    // EC: CLKIN function enabled
#pragma config BOREN = OFF  // Brown-out Reset disabled
#pragma config WDTE = OFF   // WDT disabled
#pragma config MCLRE = OFF  // MCLR pin function is digital input
#pragma config CP = OFF     // This is an open source project!
#pragma config LVP = OFF    // High voltage must be used for programming
#pragma config LPBOR = OFF  // BOR disabled
#pragma config BORV = LO    // low voltage 
#pragma config WRT = OFF    // Write protection off