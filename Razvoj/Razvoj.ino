#include <IRremote.h>
#include <TimerOne.h>

int RECV_PIN = 7;
char Cod[7];
int brojac3 = 0;

IRrecv irrecv(RECV_PIN);
decode_results results;

const int ledPin =  13; //Dioda L na ploci Status Led
const int AlarmOFF = 2;

//Alarm opste
boolean Alarmstatus = true; //Uklucen/Isklucen Alarm

//Promenive u potprogramu SerialRead
char  saseriala1[200];
int brojac1 = 0;

//Promenjive u potprogramu AlarmDeaktiv
int Ringbrojac = 0;
int Vremedeaktivacije = 0;
int Atime = 0;
int brojac2 = 5;
boolean Adistime = false;

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
 
  pinMode(ledPin, OUTPUT);
  pinMode(AlarmOFF, INPUT);
  digitalWrite(ledPin, LOW);
  
   //Inicijalizacija Timer-a
  Timer1.initialize(500000);
  Timer1.attachInterrupt(callback);
  
  irrecv.enableIRIn(); // Start the receiver
  
   //Inicijalizacija GSM Posesavanje Text moda i CNMi-a
  Serial1.println("ATZ");
  Provera();
  Serial1.println("ATE0");
  Provera();
  Serial1.println("AT+CNMI=2,1");
  Provera();
  Serial1.println("AT+CMGF=1");
  Provera();
  digitalWrite(ledPin, HIGH);
}

void loop() {
  Readserial();
  Alarmdeactiv();
  if (irrecv.decode(&results)) {
      deco(&results);    
      irrecv.resume(); // Receive the next value
  }
}
void Readserial(){
  brojac1 = 0;
  do{
    if(Serial1.available()){
      brojac1++;
      saseriala1[brojac1] = Serial1.read();
      Serial.print("Brojac1: ");
      Serial.print(brojac1);
      Serial.print("  Saseriala1[brojac1]: ");
      Serial.println(saseriala1[brojac1]);
    }
  }while(Serial1.available());
}
void Alarmdeactiv(){
  if(saseriala1[2] == 'R'){
    if(saseriala1[3] == 'I'){
      if(saseriala1[4] == 'N'){
        Ringbrojac++;
        if(Ringbrojac >= 3){
          Ringbrojac = 0;
          brojac2 = 5;
          Alarmstatus = false;
          Adistime = true;
          Serial1.println("ATH");
        }
        for(int i=0;i<=200;i++){
          saseriala1[i] = ' ';
        }
      }
    }
  }
}
void callback(){
//timer0 interrupt 2kHz toggles pin 8
//generates pulse wave of frequency 2kHz/2 = 1kHz (takes two cycles for full wave- toggle high then toggle low)
   Vremedeaktivacije++;
   if(Vremedeaktivacije >= 2){
     Vremedeaktivacije = 0;
     if(Adistime == true){
       Atime++;
       if(digitalRead(AlarmOFF)){
          noTone(11);
          Serial.println("Alarm Isklucen");
          Adistime = false;
          Atime = 0;
       }
       if(Atime >=20){
         tone(11,2000);
         brojac2--;
       }
       if(Atime >=25){
         noTone(11);
         Adistime = false;
         Alarmstatus = true;
         Serial.println("Uklucen Alarm");
         Atime = 0;
       }
     }
   }
}
void Provera(){
  //delay(1000);
  do{
    if(Serial1.available()){
      int ProveraByte = Serial1.read();
      if (ProveraByte = 75){
        break; 
      } 
      if (ProveraByte = 82){
        break; 
      }
    }
  }while(Serial1.available());
}
void deco(decode_results *results){
  switch(results->value)
  {
  case 0x17EF807:  
    Serial.println(" 0              "); 
    brojac3++;
    Cod[brojac3] = '0';
    break;

  case 0x17E6897:  
    Serial.println(" 1              "); 
    brojac3++;
    Cod[brojac3] = '1';
    break;

  case 0x17EE817:  
    Serial.println(" 2              "); 
    brojac3++;
    Cod[brojac3] = '2';
    break;

  case 0x17E18E7:  
    Serial.println(" 3              ");
    brojac3++;
    Cod[brojac3] = '3'; 
    break;

  case 0x17E9867:  
    Serial.println(" 4              "); 
    brojac3++;
    Cod[brojac3] = '4';
    break;

  case 0x17E58A7:  
    Serial.println(" 5              ");
    brojac3++;
    Cod[brojac3] = '5'; 
    break;

  case 0x17ED827:  
    Serial.println(" 6              "); 
    brojac3++;
    Cod[brojac3] = '6';
    break;

  case 0x17E38C7:  
    Serial.println(" 7              "); 
    brojac3++;
    Cod[brojac3] = '7';
    break;

  case 0x17EB847:  
    Serial.println(" 8              "); 
    brojac3++;
    Cod[brojac3] = '8';
    break;

  case 0x17E7887:  
    Serial.println(" 9              "); 
    brojac3++;
    Cod[brojac3] = '9';
    break;
  }
  if(brojac3 >= 6){
    brojac3 = 0;
    Serial.print("Kod je: ( ");
    for(int i=0;i<=7;i++){
      Serial.print(Cod[i]);
    }
    Serial.println(" )");
  }
}
