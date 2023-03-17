'''-------------------------------------------
- Program: pwm.py;                           -
- coded by:  Harry Li, CTI One Corp.         -
- Date:      Dec. 5, 2021;                   - 
- Version: 0x1.0;                            -
- Status:  release                           -
- To run:  $python3 pwm.py;                  -
- Note:    for Jetpack 4.3 with GPIO package -
-          Jetson NANO.                      -
- copyright: CTI One corp.                   -
- Last update: Jan 31, 2023                  -
----------------------------------------------
'''
#start 
import RPi.GPIO as GPIO 
import time #use for delay 

GPIO.cleanup()
GPIO.setmode(GPIO.BOARD)

my_pwm = GPIO.PWM(33,500)  #pin33, 500Hz
my_pwm.start(1) 

time.sleep(1) # sec to wait
my_pwm.ChangeDutyCycle(2) 

time.sleep(30)  
my_pwm = GPIO.PWM(33,500) 
my_pwm.ChangeDutyCycle(4)
my_pwm.start(5) #sec to run  

time.sleep(2)
#my_pwm.stop(my1)
my_pwm.cleanup()
#end
