#define FALSE 0  
#define TRUE  1

#define L_BUTTON 2
#define R_BUTTON 3
#define L_LED 4
#define R_LED 5
#define BRAKE_LED 6
#define US_TRIGGER 8
#define US_ECHO 9
#define BPS_PWM 11
#define BPS_SIGNAL A5

byte L_BUTTON_Previous_State;
byte R_BUTTON_Previous_State;
unsigned long timestamp_LED;
byte L_BUTTON_State;
byte R_BUTTON_State;
int L_LED_State;
int R_LED_State;
int B_LED_State;
int L_BLINK;
int R_BLINK;
int B_BLINK;


int US_SENSOR_run;
int BRAKE_PEDAL_SENSOR_run;

int BRAKE_BLINK_SHEDULE;

int B_State; //pedálszögszenzor állása

float us_distance;

unsigned int BRAKE_Sensor_MAX=945; //TOLERANCE -10  mért értékek(90% 850) a maxot vettem alapnak 945  //min TOLERANCE +10 mért érték(10% 80)

int US_State;

int BLINK_FREK=500;
int BLINK_BORDER=0;
int BLINK_TOLERANCE=30; //Mivel az US_Sensor miatt késünk 21 ms-t, az arduino pedig alapból késik 2-5 ms et
unsigned long Previous_LED_timestamp=0;

void trigger_measurement(void)
{
  digitalWrite(US_TRIGGER,HIGH); //Applying a pulse of 11 microseconds. This is the trigger signal.
  delayMicroseconds(11);
  digitalWrite(US_TRIGGER,LOW);
}
#define LIMIT   11600   //~200 cm. Range limit.
#define DISTANCE_DIVIDER      58    //The ECHO time in microseconds shall be divided by 58 to get the distance in centimeters.
byte actual_level = LOW;    //Variables for catching rising and falling edges.
byte previous_level = LOW;
//unsigned long timestamp;    //Self-explanatory.
byte measurement_running = FALSE; //A flag to signalize that the measurement is running.
byte sensor_triggered = FALSE;    //A flag to signalize that the sensor was triggered.



//////////////////////Sheduler

#define NUMBER_OF_PERIODIC_TASKS   5

typedef struct TASK_CONTROL
{
  void (*task)(void);
  int period;
  int true_period;
};



TASK_CONTROL periodic_tasks[NUMBER_OF_PERIODIC_TASKS] =
    {
      { Brake_Pedal_Sensor, 40, 40 }, //Pedal szögszenzor ütemezése
      { US_Sensor, 60, 60 },  //Ultrahangos távolságmérő ütemezése
      { Blink, 500, 500 },  //Index frekvencia ütemezése  2Hz
      { BRAKE_BLINK_2, 500, 500 }, //Féklámpa 2. fokozat kisebb frekvenciás villogás 2Hz
      { BRAKE_BLINK_1, 250, 250 } //Féklámpa 1. fokozat nagyfrekvenciás villogás 4Hz
      
    };

unsigned long timestamp[NUMBER_OF_PERIODIC_TASKS];

/////////////////////End sheduler


void setup() {
  byte task_index;
  for (task_index = 0; task_index < NUMBER_OF_PERIODIC_TASKS; task_index++)
  {
    timestamp[task_index] = millis();
  }
  
  pinMode(L_BUTTON,INPUT);
  pinMode(R_BUTTON,INPUT);
  pinMode(L_LED,OUTPUT);
  pinMode(R_LED,OUTPUT);
  pinMode(BRAKE_LED,OUTPUT);

  pinMode(BPS_SIGNAL,INPUT);  //Sefining the signal's port as INPUT.


  pinMode(US_TRIGGER,OUTPUT);
  pinMode(US_ECHO,INPUT);

  Serial.begin(38400);    //Initializing the serial port with 38400 baud.
  while (!Serial);

  L_BUTTON_Previous_State=HIGH;
  R_BUTTON_Previous_State=HIGH;
  L_BUTTON_State=HIGH;
  R_BUTTON_State=HIGH;

  L_LED_State=0;
  R_LED_State=0;

  digitalWrite(R_LED,HIGH);
  digitalWrite(L_LED,HIGH);
  digitalWrite(BRAKE_LED,HIGH);

  L_BLINK=0;
  R_BLINK=0;
  B_BLINK=0;

  us_distance=0;
  US_State=0;
  B_State=1;

  US_SENSOR_run=0;
  BRAKE_PEDAL_SENSOR_run=0;
  BRAKE_BLINK_SHEDULE=4;
}

  
void Brake_Pedal_Sensor(){
  unsigned int ADC_value;   //Variable of a conversion value.
  unsigned long int cumulative = 0;   //Variable for cumulating the results of several measurements.
                                      //It shall be zeroed, otherwise the RAM trash is picked up.
  unsigned int num;

    
  for (num = 0; num < 155; num++)    //The period of the PWM signal was measured to be 1800 microseconds.
  {                                 //The AD conversion takes 116 microseconds. 10 complete periods
                                    //mean 10×1800/116 = 155 samples.
    ADC_value = analogRead(BPS_SIGNAL);   //Reading in the conversion value.
    cumulative += ADC_value;          //Cumulating the results.
  }
 
  ADC_value = cumulative / 155;    //Forming the average.
  Serial.println(ADC_value,DEC);  //Sending the result to the USB. ///ITT KOMMENTELTEM
  double percent=((double)ADC_value/(double)BRAKE_Sensor_MAX*100); //Pedálszögszenzor % os kitöltési tényezője (azért adtam meg nagyobb és kisebb tűrést, mert a mért értékek ingadozhatnak)
  if(percent>=70 && percent<=95){
    B_State=4;
  }else if(percent>=50 && percent<70){
    B_State=3;
  }else if(percent>=20 && percent<50){
    B_State=2;
  }else if(percent>=5 && percent<20){
    B_State=1;
  }else if(percent<5){
    Serial.println("Szakadás!");
    B_State=0;
  }else if(percent>95){
    Serial.println("Rövidzár!");
    B_State=5;
  }
  BRAKE_PEDAL_SENSOR_run=1;
}


void US_Sensor()
{
  // Clears the trigPin
  digitalWrite(US_TRIGGER, LOW);
  delayMicroseconds(2);
 
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(US_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(US_TRIGGER, LOW);
 
  // Reads the echoPin, returns the sound wave travel time in microseconds
  int duration = pulseIn(US_ECHO, HIGH);
 
  //calculating distance
  us_distance= duration*0.034/2;

  if(us_distance>120){
    US_State=0;
  }else if(us_distance<=120 && us_distance>80){
    US_State=1;
  }else if(us_distance<=80 && us_distance>40){
    US_State=2;
  }else if(us_distance<=40){
    US_State=3;
  }

  US_SENSOR_run=1;
}

void Blink(){
    if(R_LED_State==1 && L_LED_State==0){
      digitalWrite(L_LED,HIGH);
        if(R_BLINK==0){
        digitalWrite(R_LED,LOW);
        R_BLINK=1;
        }else{
        digitalWrite(R_LED,HIGH);
        R_BLINK=0;
        }
  }else if(R_LED_State==0 && L_LED_State==1){
    digitalWrite(R_LED,HIGH);
        if(L_BLINK==0){
        digitalWrite(L_LED,LOW);
        L_BLINK=1;
        }else{
        digitalWrite(L_LED,HIGH);
        L_BLINK=0;
        }
  }else if(R_LED_State==1 && L_LED_State==1){
        if(R_BLINK==0){
        digitalWrite(R_LED,LOW);
        digitalWrite(L_LED,LOW);
        R_BLINK=1;
        L_BLINK=1;
        }else{
        digitalWrite(R_LED,HIGH);
        digitalWrite(L_LED,HIGH);
        R_BLINK=0;
        L_BLINK=0;
        }
  }else if(R_LED_State==0 && L_LED_State==0){
    digitalWrite(R_LED,HIGH);
    digitalWrite(L_LED,HIGH);
  }
}

void ButtonEvent(){
  L_BUTTON_State=digitalRead(L_BUTTON);
  R_BUTTON_State=digitalRead(R_BUTTON);

  if(L_BUTTON_State==LOW && L_BUTTON_Previous_State==HIGH && R_BUTTON_State==HIGH && R_BUTTON_Previous_State==HIGH && (L_LED_State==1 && R_LED_State==1)==FALSE){
    //Serial.println("Left Button is pussed!");
    if(L_LED_State==0){
      L_LED_State=1;
    }else{
      L_LED_State=0;
    }
    R_LED_State=0;
    delay(20); //Prellegés mentesítés (Mivel az arduino loop-ja nem frissít olyan gyorsan, ezért 20ms bőven elég a prellegés mentesítéshez és nm zavar be közbe a rendszernek)
  }

  if(R_BUTTON_State==LOW && R_BUTTON_Previous_State==HIGH && L_BUTTON_State==HIGH && L_BUTTON_Previous_State==HIGH && (L_LED_State==1 && R_LED_State==1)==FALSE){
    //Serial.println("Right Button is pussed!");
    if(R_LED_State==0){
      R_LED_State=1;
    }else{
      R_LED_State=0;
    }
    L_LED_State=0;
    delay(20);  //Prellegés mentesítés (Mivel az arduino loop-ja nem frissít olyan gyorsan, ezért 20ms bőven elég a prellegés mentesítéshez és nm zavar be közbe a rendszernek)
  }

  if(R_BUTTON_State==LOW && L_BUTTON_State==LOW && (R_BUTTON_Previous_State==HIGH || L_BUTTON_Previous_State==HIGH)){
    //Serial.print("Index is pussed!");
    if(L_LED_State==1 && R_LED_State==1){
        //Serial.println(" oFF");
        R_LED_State=0;
        L_LED_State=0;
    }else{
        //Serial.println(" oN");
        R_LED_State=1;
        L_LED_State=1;
    }
    delay(20);  //Prellegés mentesítés (Mivel az arduino loop-ja nem frissít olyan gyorsan, ezért 20ms bőven elég a prellegés mentesítéshez és nm zavar be közbe a rendszernek)
  }

  R_BUTTON_Previous_State=R_BUTTON_State;
  L_BUTTON_Previous_State=L_BUTTON_State;
}

void BRAKE_US_CALC(){
  if(US_State==3 || B_State==4){
    B_LED_State=1;
    BRAKE_BLINK_SHEDULE=1;
  }else if(US_State==2 || B_State==3){
    B_LED_State=1;
    BRAKE_BLINK_SHEDULE=2;
  }else if(US_State==1 || B_State==2){
    B_LED_State=0;
    BRAKE_BLINK_SHEDULE=3;
  }else if(US_State==0 || B_State==1){
    B_LED_State=0;
    BRAKE_BLINK_SHEDULE=4;
  }
}

void BRAKE_BLINK_1(){
  if(BRAKE_BLINK_SHEDULE==1){
  if(B_LED_State==1){
        if(B_BLINK==0){
        digitalWrite(BRAKE_LED,LOW);
        B_BLINK=1;
        }else{
        digitalWrite(BRAKE_LED,HIGH);
        B_BLINK=0;
        }

  }else{
    if(US_State==1 || B_State==2){
      digitalWrite(BRAKE_LED,LOW);
    }else{
      digitalWrite(BRAKE_LED,HIGH);
    }
  }
  }
}

void BRAKE_BLINK_2(){
  if(BRAKE_BLINK_SHEDULE==2){
  if(B_LED_State==1){
        if(B_BLINK==0){
        digitalWrite(BRAKE_LED,LOW);
        B_BLINK=1;
        }else{
        digitalWrite(BRAKE_LED,HIGH);
        B_BLINK=0;
        }
  }else{
    if(US_State==1 || B_State==2){
      digitalWrite(BRAKE_LED,LOW);
    }else{
      digitalWrite(BRAKE_LED,HIGH);
    }
  }
  }
}

void loop() {
  byte task_index;
  US_SENSOR_run=0;
  BRAKE_PEDAL_SENSOR_run=0;
  
  for (task_index = 0; task_index < NUMBER_OF_PERIODIC_TASKS; task_index++)
  {
    if (millis() - timestamp[task_index] >= periodic_tasks[task_index].period)
    {     
      timestamp[task_index] = millis();
      periodic_tasks[task_index].period = periodic_tasks[task_index].true_period;

      periodic_tasks[task_index].task();
    }
    
  }

  if(BRAKE_BLINK_SHEDULE==3){
    digitalWrite(BRAKE_LED,LOW);  //Fékláma 3. fokozat folyamatos vilagítás
  }else if(BRAKE_BLINK_SHEDULE==4){
    digitalWrite(BRAKE_LED,HIGH); //Fékláma 4. fokozat nem világít
  }
  
  timestamp_LED=millis();
  
  ButtonEvent();
  BRAKE_US_CALC();
 
  
}
