//---------------------------------------------------//
// Program: PID.c                                    //  
// Coded by: HL;                                     //
// Date: April 1, 2015;                              //
// Status: tested.                                   // 
// Note: this program implements PID control of a    //
//       simulated vehicle.                          //  
// Compilation and built:                            //
// gcc -Wall -W -Werror Dcontrol.c -o Dcontrol       //  
//                                                   //  
// History: Feb 14, 2017                      
// HL  
// 1. Add gearbox
// 2. Change F_pwm to 50 Hz Max
// 3. 
//---------------------------------------------------//

#include <stdio.h>
#include <math.h>

#define ARRAY_LEN 50    //define array length 
#define KERNEL_LEN 7    //define kernel length 
#define ERROR_LEN 50    //define error length 
#define pi 3.1415926    //   

#define alpha_p 1   //coefficient for proporation controller for later intelligent control   
#define alpha_i 1   //coefficient for integral controller for later intelligent control   
#define alpha_d 1   //coefficient for derivative controller for later intelligent control   

#define f_pwm   1500  //PWM frequency for stepper motor steering which gives 
                      //66.67 uSec sampling time interval  
 
#define microStepFull 1.8    //full  
#define microStepHalf 0.9    //half  
#define microStepQ    0.45   //quarter  
#define microStep8    0.225  //one eighth  

#define speedVe    5000  //5 Km per hour, or 1.389 meter per second   
//
// add reduction ratio 
// 

void  getDerive( float error0, float error2, float central_error );    //central difference   
void  getIntegral( float *Error, int n4_time, float iDError );    //step ??  

//--------------------------------------------//
// main module                                //
//--------------------------------------------//

int main()
{

float x[ARRAY_LEN];       // Reference input  
float Error[ARRAY_LEN];   // error for P  
float iDError =0; // error for I 
float centralDError; // errors for D (central difference)   
float bufferError, errorPrevious;   

float MicroStep;      //angle 

float K_p;      //proportional controller gain  
float K_i;      //integral controller gain  
float K_d;      //derivative controller gain   

float sumPID;                   //sum PID control action  
float speedUnitTime; 

int   upperBound;               //total number of loops for control action 
int   integralBound;            //total number of erro points for integration controller   
int   freqPwm;                  //PWM frequency 
int   dirPwm;                   //PWM frequency 

float angVe[ARRAY_LEN];         //vehicle angle 
float disVe[ARRAY_LEN];         //lateral displacement basd angle and speed  


float h[ARRAY_LEN];             //sensor output (sensing from contl(.))   
float hPrime[ARRAY_LEN];        //convolution output of h(n)  

int iindex; 

//-----------------------------------------
//  Genegrate testing data
//-----------------------------------------

   printf("Please Enter the total number of points for PID testing\n");
   printf("The maximum points should be less than 50K for this program :\n");
   scanf("%d", &upperBound);
   printf("The points for PID computation  = %4d \n", upperBound);

   integralBound = 4;   //time interval for integration controller    
   //printf( "Integral time  = %4d\n", integralBound);   

   speedUnitTime = speedVe/(60*60); // meter per second 
   K_p = 0.1; 
   K_i = 0.1; 
   K_d = 1; 

   for(iindex = 0; iindex <=2 ; iindex++) 
{
   x[iindex] = 1;  // generate test data points  
   //-----------------------------------------------------------
   // check if initial start for n < 2, where n = 0, 1, 2, ... 
   // when n < 2, central difference can not be computed due to 
   // boundary conditions  
   //-----------------------------------------------------------
    disVe[iindex] = 0;  
    hPrime[iindex] = disVe[iindex];  
    
    hPrime[iindex+1] = hPrime[iindex]; // to take care of the first hPrime for index = 3   
                         // for the computation for the rest of the points 
                         // for PID control  
    bufferError = Error[iindex-1]; 
    
    getDerive( Error[iindex-2], bufferError, centralDError );    //central difference   
    //printf( "Main: Central Difference x = %4f\n", centralDError);   

    // find integral of error
    iDError = 0;     //at inital condition or near initial condition, set it 0  

    // find sume of PID 
    sumPID = K_p * Error[iindex] + K_i * iDError + K_d * centralDError;  
    //printf( "sumPID = %4f\n", sumPID);   
 
    // find PWM output 
    freqPwm = f_pwm;     // always at this version 
    //angVe[iindex] = (microStep8 * pi / 180) * sumPID ; // acutation to motor, angle to radius   
    angVe[iindex] = microStep8 * pi / 180 ; // acutation to motor, angle to radius   
    //-------acuate motor-------------------- 
    
        //send direction information to GPIO output here
        //add ARM GPIO output command  
        //printf( "Error >= 0\n");   
    if (errorPrevious < 0) {
        dirPwm = -1; 
        //send direction information to GPIO output here
        //add ARM GPIO output command  
        angVe[iindex] = -1* angVe[iindex];  
        //angVe[iindex] = microStep8 * pi / 180 ; // angle to radius   
        printf( "Error < 0\n");   
    }    

    // printf( "Angle = %4f\n", angVe[iindex]);   
    // find lateral displacement i
    // disVe[iindex] = speedVe * sin(angVe[iindex]);   // 
    disVe[iindex] = speedUnitTime * angVe[iindex];   // for small angle, sin(angle) = angle  

    // find sensor output of the displacement 
    h[iindex] = h[iindex-1] + disVe[iindex];     // sensing at ABSOLUTE coordinate system   

    // simualate random noise additive to h(n) 
    // to be added later 

    // Low Pass Filtering operation via 1D convolution 
    // to find hPrime output  
    // to be added later 

    hPrime[iindex] = h[iindex];  
    Error[iindex] = x[iindex] - hPrime[iindex]; 
    errorPrevious = Error[iindex];  
    printf( "i= %4d x= %4f h'= %4f error= %4f\n", iindex, x[iindex], hPrime[iindex], errorPrevious);   
}//for loop  
   

   for(iindex = 3; iindex <=upperBound ; iindex++) 
{
    x[iindex] = 1;  // generate test data points  
    //Error[iindex] = x[iindex] - hPrime[iindex]; 
    //printf( "Top %4f %4f\n", hPrime[iindex], Error[iindex]);   
    // find derivative of error
    //getDerive(x[iindex-2], x[iindex], &centralDError);    // only turn on when testing derivative on x(n) data  
    bufferError = Error[iindex-1]; 
    
    getDerive( Error[iindex-2], bufferError, centralDError );    //central difference   
    //printf( "Main: Central Difference x = %4f\n", centralDError);   
   

    // find integral of error
    if(iindex >= integralBound ) { 
    getIntegral( &Error, integralBound, iDError );    //central difference   
    //printf( "Main: Integral Error = %4f\n", iDError);   
    } 

    // find sume of PID 
    sumPID = K_p * Error[iindex] + K_i * iDError + K_d * centralDError;  
    //printf( "sumPID = %4f\n", sumPID);   
 
    // find PWM output 
    freqPwm = f_pwm;     // always at this version 
    //angVe[iindex] = (microStep8 * pi / 180) * sumPID ; // acutation to motor, angle to radius   
    angVe[iindex] = microStep8 * pi / 180 ; // acutation to motor, angle to radius   
    //-------acuate motor-------------------- 
    
    if (errorPrevious >= 0)  { 
        dirPwm = 1; 
        //send direction information to GPIO output here
        //add ARM GPIO output command  
        printf( "Error >= 0\n");   
    } 
    else if (errorPrevious < 0) {
        dirPwm = -1; 
        //send direction information to GPIO output here
        //add ARM GPIO output command  
        angVe[iindex] = -1* angVe[iindex];  
        //angVe[iindex] = microStep8 * pi / 180 ; // angle to radius   
        printf( "Error < 0\n");   
    }    

    // find lateral displacement i
    // disVe[iindex] = speedVe * sin(angVe[iindex]);   // 
    disVe[iindex] = speedUnitTime * angVe[iindex];   // for small angle, sin(angle) = angle  

    // find sensor output of the displacement 
    h[iindex] = h[iindex-1] + disVe[iindex];     // sensing at ABSOLUTE coordinate system   

    // simualate random noise additive to h(n) 
    // to be added later 

    // Low Pass Filtering operation via 1D convolution 
    // to find hPrime output  
    // to be added later 

    hPrime[iindex] = h[iindex];  
    Error[iindex] = x[iindex] - hPrime[iindex]; 
    errorPrevious = Error[iindex];  
    printf( "i= %4d x= %4f h'= %4f error= %4f\n", iindex, x[iindex], hPrime[iindex], errorPrevious);   
}//for loop  

return 0;
}// main  

/*-----------------------------------------------
  Compute Derivatives 
-------------------------------------------------*/ 
void  getDerive(float error0, float error2, float c_difference)     //central difference   
{

//printf( "sub: derivative error0= %4f error2= %4f \n", error0, error2);

/*----------------------------------------------
 derivative computation, just need n-2 and n to compute 
 central difference, with n as the current points, e.g., 
 error1, error2, error3 as inputs  
 forward difference: 
 x[n+1]-x[n] = error3 - error2 
 backward difference: 
 x[n]-x[n-1] = error2 - error1 
 central difference = (error3 - error1)/2    
----------------------------------------------------------------*/ 
 c_difference = (error2 - error0)/2; // central difference   
 //printf( "sub: central difference %4f \n", *c_difference);
 return;
} // subroutine  

/*---------------------------------------------
 Integral
----------------------------------------------*/

void  getIntegral( float *Error, int numTime, float iDError )     //integral
{
int i; 
 for (i=0; i <= numTime; i++)
 { 
 iDError = iDError + Error[i]*Error[i];
 }  
 //printf( "sub: integral error %4f \n", iDError);

}   
