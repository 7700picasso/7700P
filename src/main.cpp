/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       student                                                   */
/*    Created:      9/9/2024, 5:11:30 PM                                      */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include "vex.h"

using namespace vex;

// A global instance of competition
competition Competition;

// define your global instances of motors and other devices here
brain Brain; 
controller Controller1;

motor LF (PORT3, ratio6_1, true);
motor LM (PORT5, ratio6_1, true);
motor LB (PORT17, ratio6_1, true);

motor RF (PORT12, ratio6_1, false);
motor RM (PORT6, ratio6_1, false);
motor RB (PORT13, ratio6_1, false);

motor Intake (PORT14, ratio18_1, false);
motor Lifter (PORT18, ratio6_1, true);

digital_out Clampy = digital_out(Brain.ThreeWirePort.A);
digital_out Doinker = digital_out(Brain.ThreeWirePort.B);

inertial Gyro (PORT20); 

float G  = 0.75;
float D = 3.25;
float PI = 3.14;
int toggle = 0; 
/*---------------------------------------------------------------------------*/

void drive (int rspeed, int lspeed, int wt){                      
  LF.spin(fwd, lspeed, pct);
  LM.spin(fwd, lspeed, pct);  
  LB.spin(fwd, lspeed, pct);

  RF.spin(fwd, rspeed, pct);  
  RM.spin(fwd, rspeed, pct);  
  RB.spin(fwd, rspeed, pct);

  wait (wt, msec);                                                   
}

void driveBrake () {
  LF.stop(brake); 
  LM.stop(brake); 
  LB.stop(brake); 

  RF.stop(brake); 
  RM.stop(brake); 
  RB.stop(brake); 

}

void olGyroTurn(float target, int speed){
	float heading20 = 0;
  Gyro.setRotation(0, degrees);

	while (heading20<=target){
    heading20=Gyro.rotation(degrees);
    drive(speed, -speed, 10);
    wait(10,msec);
	}
	
	drive(0, 0, 0);
}

void gyroTurn(float target)
{   
  float heading=0.0; //initialize a variable for heading
  float accuracy=2.0; //how accurate to make the turn in degrees
  float error=target-heading;
  float kp=0.45;
  float speed=kp*error;
  Gyro.setRotation(0.0, degrees);  //reset Gyro to zero degrees
  
  while(fabs(error)>=accuracy){
    speed=kp*error;
    drive(-speed, speed, 10); //turn right at speed
    heading=Gyro.rotation();  //measure the heading of the robot
    error=target-heading;  //calculate error
  }
    drive(0, 0, 0);  //stope the drive
}

void inchDriveP(float target){
  float x=0;
  float error=target;
  float kp=3.0;
  float speed =kp*error;
  float accuracy=1.0;
  LF.setPosition(0.0, rev);

  while(fabs(error)>accuracy){
    drive(speed,speed,10);
    x=LF.position(rev)*PI*D*G;
    error=target-x;
    speed=kp*error;
  }
  drive(0,0,9);
}


double YOFFSET = 20; //offset for the display
//Writes a line for the diagnostics of a motor on the Brain
void MotorDisplay(double y, double curr, double temp)
{
  Brain.Screen.setFillColor(transparent);
  Brain.Screen.printAt(5, YOFFSET + y, "Current: %.1fA", curr);
  
  if (curr < 1)
    Brain.Screen.setFillColor(green);
  else if (curr >= 1 && curr  <= 2.5)
    Brain.Screen.setFillColor(yellow);
  else
    Brain.Screen.setFillColor(red);
    Brain.Screen.drawRectangle(140, YOFFSET + y - 15, 15, 15);

  Brain.Screen.setFillColor(transparent);
  Brain.Screen.printAt(160, YOFFSET + y, "Temp: %.1fC", temp);
  
  if (temp < 45)
    Brain.Screen.setFillColor(green);
  else if (temp <= 50 && temp  >= 45)
    // TRUE and TRUE --> True
    // TRUE and FALSE --> False
    // FALSE and FALSE --> False
    Brain.Screen.setFillColor(yellow);
  else
    Brain.Screen.setFillColor(red);
    Brain.Screen.drawRectangle(275, YOFFSET + y - 15, 15, 15);
    Brain.Screen.setFillColor(transparent);
}


//Displays information on the brain
void Display()
{
  double leftFrontCurr = LF.current(amp);
  double leftFrontTemp = LF.temperature(celsius);
  double leftBackCurr = LB.current(amp);
  double leftBackTemp = LB.temperature(celsius);
  double rightFrontCurr = RF.current(amp);
  double rightFrontTemp = RF.temperature(celsius);
  double rightBackCurr = RB.current(amp);
  double rightBackTemp = RB.temperature(celsius);
  double rightMiddleCurr = RM.current(amp);
  double rightMiddleTemp = RM.temperature(celsius);
  double leftMiddleCurr = LM.current(amp);
  double leftMiddleTemp = RM.temperature(celsius);


  if (LF.installed()){
    MotorDisplay(1, leftFrontCurr, leftFrontTemp);
    Brain.Screen.printAt(300, YOFFSET + 1, "LeftFront");
  }else
    Brain.Screen.printAt(5, YOFFSET + 1, "LeftFront Problem");

  if (LB.installed())
  {
  MotorDisplay(31, leftBackCurr, leftBackTemp);
  Brain.Screen.printAt(300, YOFFSET + 31, "LeftBack");
  }

else
Brain.Screen.printAt(5, YOFFSET + 31, "LeftBack Problem");

if (RF.installed())
{
MotorDisplay(61, rightFrontCurr, rightFrontTemp);
Brain.Screen.printAt(300, YOFFSET + 61, "RightFront");
}
else
Brain.Screen.printAt(5, YOFFSET + 61, "RightFront Problem");

if (RB.installed())
{
MotorDisplay(91, rightBackCurr, rightBackTemp);
Brain.Screen.printAt(300, YOFFSET + 91, "RightBack");
}
else
Brain.Screen.printAt(5, YOFFSET + 91, "RightBack Problem");

if (RM.installed())
{
MotorDisplay(121, rightMiddleCurr, rightMiddleTemp);
Brain.Screen.printAt(300, YOFFSET + 121, "RightMiddle");
}
else
Brain.Screen.printAt(5, YOFFSET + 121, "RightMiddle Problem");

if (LM.installed())
{
MotorDisplay(151, leftMiddleCurr, leftMiddleTemp);
Brain.Screen.printAt(300, YOFFSET + 151, "LeftMiddle");
}
else
Brain.Screen.printAt(5, YOFFSET + 151, "LeftMiddle Problem");
}

void clampPush(bool push){
  Clampy.set(push);
}

void doinkPush(bool push){
  Doinker.set(push);
}

void Drawgui(){
  /*Brain.Screen.clearScreen();
  wait(100, msec);*/
  Brain.Screen.setPenColor(red);
  Brain.Screen.setFillColor(red);
  Brain.Screen.drawRectangle(10, 60, 150, 150);
  Brain.Screen.drawRectangle(170, 60, 150, 150);
  Brain.Screen.drawRectangle(330, 60, 150, 150);
  Brain.Screen.setPenColor(black);
  Brain.Screen.printAt(30, 130, "Left match auto");
  Brain.Screen.setFillColor(green);
  Brain.Screen.printAt(190, 130, "Skills");
  Brain.Screen.setFillColor(blue);
  Brain.Screen.printAt(370, 130, "Right match auto");
}

int Case = 0;

void Autonselector(){
  int Xpos = Brain.Screen.xPosition();
  int Ypos = Brain.Screen.yPosition();
  
  if(Xpos > 10 and Xpos < 160 and Ypos > 60 and Ypos < 210){
  Brain.Screen.setPenColor(white);
  Brain.Screen.setFillColor(white);
  Brain.Screen.drawRectangle(10, 60, 150, 150);
  Case = 1;
  }
  else if(Xpos > 170 and Xpos < 320 and Ypos > 60 and Ypos < 210){
  Brain.Screen.setPenColor(white);
  Brain.Screen.setFillColor(white);
  Brain.Screen.drawRectangle(170, 60, 150, 150);
  Case = 2;
  }

  else if(Xpos > 330 and Xpos < 480 and Ypos > 60 and Ypos < 210){
  Brain.Screen.setPenColor(white);
  Brain.Screen.setFillColor(white);
  Brain.Screen.drawRectangle(330, 60, 150, 150);
  Case = 3;
}
}
/*---------------------------------------------------------------------------*/

void pre_auton(void) {
  Drawgui();
    
Brain.Screen.pressed(Autonselector);

 
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              Autonomous Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void autonomous(void) {
  // ..........................................................................
  
switch(Case)
{
  case 0: {
    Brain.Screen.clearScreen();
  }
  break;
case 1: {
  clampPush(true);
  inchDriveP(-18);
  gyroTurn(30);
  inchDriveP(-18);
  clampPush(false);
  wait(1000, msec);
  Lifter.spin(fwd, 70, pct);
  Intake.spin(fwd, 100, pct);
  wait(200, msec);
  gyroTurn(50);
  inchDriveP(20);
  wait(1000, msec);
  gyroTurn(-165);
  Lifter.stop();
  Intake.stop();
  wait(10, msec);
  inchDriveP(37);

}
break;
case 2:{
//Scores alliance stake.
Intake.spin(fwd, 100, pct);
Lifter.spin(fwd,50, pct);
wait(1000, msec);
//Gets mobile goal and scores 3 rings.
inchDriveP(15.5);
wait(10, msec);
gyroTurn(87);
wait(10, msec);
clampPush(true);
wait(10, msec);
inchDriveP(-19);
wait(10, msec);
clampPush(false);
wait(10, msec);
gyroTurn(170);
wait(10, msec);
inchDriveP(38);
wait(1000, msec);
drive(100, 100, 1000);
wait(10, msec);
inchDriveP(-12);
Intake.stop();
wait(10, msec);
gyroTurn(-100);
wait(10, msec);
Intake.spin(fwd, 100, pct);
Lifter.spin(fwd, 50, pct);
inchDriveP(10.5);
wait(1500, msec);
gyroTurn(-105);
wait(10, msec);
inchDriveP(-12);
wait(1000, msec);
clampPush(true);
Intake.stop();
Lifter.stop();
wait(10, msec);
inchDriveP(26);
wait(10, msec);
wait(10, msec);
inchDriveP(-60);
wait(10, msec);
clampPush(false);
gyroTurn(170);
Intake.spin(fwd, 100, pct);
Lifter.spin(fwd, 50, pct);
wait(10, msec);
inchDriveP(36);
}
break;
case 3:{
clampPush(true);
  inchDriveP(-18);
  gyroTurn(-45);
  inchDriveP(-17);
  clampPush(false);
  wait(1000, msec);
  Lifter.spin(fwd, 70, pct);
  Intake.spin(fwd, 100, pct);
  wait(200, msec);
  gyroTurn(-55);
  inchDriveP(20);
  wait(1000, msec);
  gyroTurn(170);
  Lifter.stop();
  Intake.stop();
  wait(10, msec);
  inchDriveP(37);
}
}
}
  // ..........................................................................


/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase of */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void usercontrol(void) {
  // User control code here, inside the loop

  while (1) {
    
     Brain.Screen.clearScreen(); 
    Display();
    Brain.Screen.printAt(10, 200, "Toggle= %0.2f", toggle);
    int lspeed = Controller1.Axis3.position(pct);
    int rspeed = Controller1.Axis2.position(pct);

    drive(rspeed, lspeed, 10);

    if (Controller1.ButtonA.pressing()){
    clampPush(true);
    }
    else if (Controller1.ButtonB.pressing()){
    clampPush(false);
    }
   
   
    if (Controller1.ButtonX.pressing()){
      toggle = toggle + 1;
    }
    if (toggle == 0){
    Doinker.set(false);
    wait(100, msec); 
    }
    else if (toggle == 1){
      Doinker.set(true);
    wait(100, msec);
    }
    if(toggle == 2){
      Doinker.set(false);
      toggle = -1;
    }

    if (Controller1.ButtonR1.pressing()){
      Intake.spin(fwd, 100, pct);
      Lifter.spin(fwd,50, pct);
    }
    else if (Controller1.ButtonR2.pressing()){
      Intake.spin(reverse,100, pct);
      Lifter.spin(reverse,50, pct);
    }
    else if (Controller1.ButtonUp.pressing()){
      Intake.stop();
      Lifter.stop();
    }
    // ........................................................................

    wait(20, msec); // Sleep the task for a short amount of time to
                    // prevent wasted resources.
  }
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  // Run the pre-autonomous function.
  pre_auton();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}
