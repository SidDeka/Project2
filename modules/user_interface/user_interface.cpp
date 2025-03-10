//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "user_interface.h"

#include "code.h"
#include "siren.h"
#include "smart_home_system.h"
#include "fire_alarm.h"
#include "date_and_time.h"
#include "temperature_sensor.h"
#include "gas_sensor.h"
#include "matrix_keypad.h"
#include "display.h"
#include "ServoMotor.h"

//=====[Declaration of private defines]========================================

#define DISPLAY_REFRESH_TIME_MS 1000

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

DigitalOut incorrectCodeLed(LED3);
DigitalOut systemBlockedLed(LED2);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============
char codeSequenceFromUserInterface[CODE_NUMBER_OF_KEYS];
char pressed[5];


//=====[Declaration and initialization of private global variables]============

static bool incorrectCodeState = OFF;
static bool systemBlockedState = OFF;

static bool codeComplete = false;
static int numberOfCodeChars = 0;

//=====[Declarations (prototypes) of private functions]========================

static void userInterfaceMatrixKeypadUpdate();
static void incorrectCodeIndicatorUpdate();
static void systemBlockedIndicatorUpdate();

static void userInterfaceDisplayInit();
static void userInterfaceDisplayUpdate();


//=====[Implementations of public functions]===================================

void userInterfaceInit()
{
    incorrectCodeLed = OFF;
    systemBlockedLed = OFF;
    matrixKeypadInit( SYSTEM_TIME_INCREMENT_MS );
    userInterfaceDisplayInit();
}

void userInterfaceUpdate()
{

    userInterfaceMatrixKeypadUpdate();
    incorrectCodeIndicatorUpdate();
    systemBlockedIndicatorUpdate();
    userInterfaceDisplayUpdate();
}

bool incorrectCodeStateRead()
{
    return incorrectCodeState;
}

void incorrectCodeStateWrite( bool state )
{
    incorrectCodeState = state;
}

bool systemBlockedStateRead()
{
    return systemBlockedState;
}

void systemBlockedStateWrite( bool state )
{
    systemBlockedState = state;
}

bool userInterfaceCodeCompleteRead()
{
    return codeComplete;
}

void userInterfaceCodeCompleteWrite( bool state )
{
    codeComplete = state;
}

//=====[Implementations of private functions]==================================

static void userInterfaceMatrixKeypadUpdate()
{
    static int numberOfHashKeyReleased = 0;
    char keyReleased = matrixKeypadUpdate();

    static int numTries = 3;
    // static int numTries_init = 1;
    static int pos_x = 11;
    static int pressed_pos = 0;
   


    // if(numTries_init > 0)
    // {
    //      char str1[10];
    //      sprintf(str1, "%d", numTries);
    //      displayCharPositionWrite ( 10,1 );
    //      displayStringWrite( str1);
    //      numTries_init = numTries_init - 1;
    // }

    
    char str2[10];
    sprintf(str2, "%d", numTries);
    displayCharPositionWrite( 10,1 );
    displayStringWrite( str2);


    if(numTries <= 0){
        
            clear_screen();
            while(true){
            displayCharPositionWrite ( 0,0 );
            displayStringWrite( "Please reset" );

            displayCharPositionWrite ( 0,1 );
            displayStringWrite("the system");
            }
    }

    if(numTries > 0)
    {
    if( keyReleased != '\0' ) 
    {

  
         pressed[4] = '\0';
         pressed[pressed_pos] = keyReleased;
         pressed_pos++;
  

         displayCharPositionWrite (pos_x, 0 );
         char str3[2];
         str3[0] = keyReleased;
         str3[1] = '\0';
         displayStringWrite(str3);
         pos_x++;
        
         if((new_code_correct(pressed) == true) && (pressed[4] == '#' ))
         {
            clear_screen();
            displayCharPositionWrite ( 0,0 );
            displayStringWrite( "Gate opening" );
            servo_update(pressed);
            userInterfaceDisplayInit();
            numTries = 3;
            pressed_pos = 0;
            pos_x = 11;
            
         }


         if((new_code_correct(pressed) == false) && (pressed[4] != '\0')){
             numTries = numTries -1;
             pos_x = 11;
             pressed_pos = 0;
             char empty[5];
             for(int i = 0; i <5; i++){
                     empty[i] = ' ';
                 }

                 displayCharPositionWrite (11, 0 );
                 displayStringWrite(empty);

                 displayCharPositionWrite (12, 0 );
                 displayStringWrite(empty);

                 displayCharPositionWrite (13, 0 );
                 displayStringWrite(empty);

                 displayCharPositionWrite (14, 0 );
                 displayStringWrite(empty);
         }





        if( sirenStateRead() && !systemBlockedStateRead() ) {
            if( !incorrectCodeStateRead() ) {
                codeSequenceFromUserInterface[numberOfCodeChars] = keyReleased;
                numberOfCodeChars++;
                if ( numberOfCodeChars >= CODE_NUMBER_OF_KEYS ) {
                    codeComplete = true;
                    numberOfCodeChars = 0;
                }
            } else {
                if( keyReleased == '#' ) {
                    numberOfHashKeyReleased++;
                    if( numberOfHashKeyReleased >= 2 ) {
                        numberOfHashKeyReleased = 0;
                        numberOfCodeChars = 0;
                        codeComplete = false;
                        incorrectCodeState = OFF;
                    }
                }
            }
        }
    }
}
}


static void userInterfaceDisplayInit()
{
    displayInit();
 

 

    displayCharPositionWrite ( 0,0 );
    displayStringWrite( "Prss # to submit" );

    displayCharPositionWrite ( 0,1 );
    displayStringWrite( "code" );
   
    delay(3000);

    clear_screen();
    displayCharPositionWrite ( 0,0 );
    displayStringWrite( "Enter code:" );

    displayCharPositionWrite ( 0,1 );
    displayStringWrite( "Num Tries:" );

     
    
    
//     displayCharPositionWrite ( 0,1 );
//     displayStringWrite( "Alarm:" );
 }

static void userInterfaceDisplayUpdate()
{
    static int accumulatedDisplayTime = 0;
    //char temperatureString[3] = "";
  
    
    if( accumulatedDisplayTime >=
        DISPLAY_REFRESH_TIME_MS ) {

        accumulatedDisplayTime = 0;

        // sprintf(temperatureString, "%.0f", temperatureSensorReadCelsius());
        // displayCharPositionWrite ( 4,0 );
        // displayStringWrite( temperatureString );


        

  



        // } else {
        //     displayStringWrite( "ND" );
        // }

        // displayCharPositionWrite ( 6,1 );
        
        // if ( sirenStateRead() ) {
        //     displayStringWrite( "ON " );
        // } else {
        //     displayStringWrite( "OFF" );
        // }

    } else {
        accumulatedDisplayTime =
            accumulatedDisplayTime + SYSTEM_TIME_INCREMENT_MS;        
    } 
}


static void incorrectCodeIndicatorUpdate()
{
    incorrectCodeLed = incorrectCodeStateRead();
}

static void systemBlockedIndicatorUpdate()
{
    systemBlockedLed = systemBlockedState;
}

