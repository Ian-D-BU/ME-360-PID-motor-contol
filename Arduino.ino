#define clock_wise 6
#define counter_clock_wise 5


void setup() {
  // put your setup code here, to run once:
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(clock_wise, OUTPUT);
  pinMode(counter_clock_wise, OUTPUT);
  attachInterrupt (digitalPinToInterrupt (2),myFunction,RISING);
  Serial.begin(9600);
}


// count is 240 for one rotation
volatile long int count=0;
void loop() {
  float feet = 5;
  travel_dis(feet);
}


void myFunction(){
  if (digitalRead(3))
    count++;
  else
    count--;
}


double getSpeed() {
  long int count0, count1;
  double theta0, theta1, w;


  count0=count;
  theta0=1.5*count0; //degrees
  delay(100);


  count1=count;
  theta1=1.5*count1;
  w = (theta1-theta0)/(0.1); //degrees per second
  return((w*60)/360);//RPM
}


void cw(float speed){// turning clock wise function
  analogWrite(clock_wise,speed);
  analogWrite(counter_clock_wise,0);
}


void ccw(float speed){// turning counter clock wise function
  analogWrite(clock_wise,0);
  analogWrite(counter_clock_wise,speed);
}


void travel_dis(float distance) {
  float a = 13;  // acceleration step 22 sideways 16 not
  float half_dis_inch = (distance*12)/2.6; // convert to inches then devide by two
  float active_RPM = 0;
  int delta_t = 100;  // time between RPM adjustments
  Serial.println(half_dis_inch);
  // Reset count at start
  count = 0;
 
  // Acceleration phase
  setRPM(active_RPM, delta_t,1);
  while ((count/240)*1.23*2*3.1415 < half_dis_inch) {
    active_RPM -= a;
    setRPM(active_RPM, delta_t,0);
    Serial.print(" Dist: ");
    Serial.println((count/240)*1.23*2*3.1415);
  }
 
  // Deceleration phase
  while (active_RPM < 0) {
    active_RPM += a;
    setRPM(active_RPM, delta_t,0);
    Serial.print(" Dist:");
    Serial.println((count/240)*1.23*2*3.1415);
  }
  Serial.println(active_RPM);


  // Accelerating back
  while ((count/240)*1.23*2*3.1415 > half_dis_inch*1.6) {
    active_RPM += a;
    setRPM(active_RPM, delta_t,0);
    Serial.print(" Dist:");
    Serial.println((count/240)*1.23*2*3.1415);
  }
    // Decelerating back
  while (active_RPM > 0) {
    active_RPM -= a;
    setRPM(active_RPM, delta_t,0);
    Serial.print(" Dist:");
    Serial.println((count/240)*1.23*2*3.1415);
  }
  delay(5000);// Stop motor
  cw(0);
  ccw(0);
}


volatile float speed = 0;
void setRPM(int RPM,unsigned long duration, int start){ // set start < 0 for starting going backward > 0 for forward and 0 for not starting
  float kp = 0.4;
  RPM = -RPM;
  if(start>0){
    speed = 30;
  }
  else if(start<0){
    speed = -30;
  }
  else{
    unsigned long startTime = millis();  // Record the start time
    while (millis() - startTime < duration) { // Run for 'duration' milliseconds
      if (RPM > 0){
        speed = speed + (RPM - getSpeed())*kp;
        if (speed > 254){
          speed = 255;
        }
        ccw(speed);
        Serial.print(" PWM:");
        Serial.print(speed);
        Serial.print(" RPM:-");
        Serial.print(getSpeed());
      }
      else if (RPM < 0){
        speed = speed - (RPM - getSpeed())*kp;
        if (speed > 254){
          speed = 255;
        }
        cw(speed);    
        Serial.print("PWM:");
        Serial.print(speed);
        Serial.print(" RPM:");
        Serial.print(getSpeed());  
      }
      else {
        speed = 0;
        analogWrite(clock_wise,speed);
        analogWrite(counter_clock_wise,speed);  
        Serial.println("PWM:");
        Serial.print(speed);
        Serial.print(" RPM:");
        Serial.print(getSpeed());  
      }
    }
  }
}
