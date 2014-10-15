//ROBOTS ARE BETTER THEN GIRLFREINDS
#define LED_N_SIDE 2
#define LED_P_SIDE 3
#define LAMP 13
#define LSF 9
#define LSR 115

#define RSF 114
#define RSR 9

#define LSP 10
#define RSP 9
#define BOUSTT_T 1000  //aproxomet time for a 100 gradian tank turn
#define BOUSTF_T 1000  //aproxomet time to travel width of robot mop
#define servopower 11

#include <Servo.h>
Servo ls,rs;
int p,ver=1;
int lr=10; //dynamic range for light sensor, higher values mean less resolution
int bos1=0;
void help(char c);

void
servostop()
{
  ls.detach();
  rs.detach();
}
void
servoforward()
{
  ls.attach(LSP);
  rs.attach(RSP);
  ls.write(LSF);
  rs.write(RSF);
}
void
servoleft()
{
  rs.attach(RSP);
  ls.attach(LSP);
  ls.write(LSR);
  rs.write(RSF);
}
void
servoboust()
{
  if(bos1){
    servoleft();
  }
  else{
    servoright();
  }
  delay(BOUSTT_T);
  servoforward();
  delay(BOUSTF_T);
  if(bos1){
    servoleft();
    bos1=0;
  }
  else{
    servoright();
    bos1=1;
  }
  delay(BOUSTT_T);
  servostop();
}
void
servoright()
{
  ls.attach(LSP);
  rs.attach(RSP);
  rs.write(RSR);
  ls.write(LSF);
}
unsigned int
getlight()
{
   unsigned int j;
   digitalWrite(LAMP,HIGH);
      // Apply reverse voltage, charge up the pin and led capacitance
    pinMode(LED_N_SIDE,OUTPUT);
    pinMode(LED_P_SIDE,OUTPUT);
    digitalWrite(LED_N_SIDE,HIGH);
    digitalWrite(LED_P_SIDE,LOW);

    // Isolate the pin 2 end of the diode
    pinMode(LED_N_SIDE,INPUT);
    digitalWrite(LED_N_SIDE,LOW);  // turn off internal pull-up resistor

    // Count how long it takes the diode to bleed back down to a logic zero
    for ( j = 0; j < 30000; j++) {
      delayMicroseconds(lr);
      if ( digitalRead(LED_N_SIDE)==0) break;
    }
     digitalWrite(LAMP,LOW);
    return j;
}
int
getdist()
{
  return analogRead(0);
}
void setup()
{
  pinMode(LAMP,OUTPUT);
    digitalWrite(servopower,LOW);
  pinMode(servopower,OUTPUT);
      digitalWrite(servopower,LOW);
  digitalWrite(LAMP,LOW);
  servoforward();
  servostop();
  Serial.begin(9600);
}
void
servos(char t)
{
      Serial.print("SERVO");
      switch(t){
        case 's': servostop();break;
        case 'f': servoforward();break;
        case 'l': servoleft();break;
        case 'r': servoright();break;
        case 'b': servoboust();break;
        default: Serial.print("\ns -> stop\nf -> forward\nl -> left\nr -> right\nSERVO");
      }
}
void 
exec(char c, char b)
{
    switch(c){
      case '?': help(b);break;
      case 's': servos(b);break;
      case 'd': Serial.println(getdist());Serial.print("SENSOR");break;
      case 'l': Serial.println(getlight());Serial.print("SENSOR");break;
      default: Serial.print("?");
    }
}
void
help(char c)
{
  Serial.print("\nHELP\n");
  Serial.print("?[c] help [for command] | get command description\n");
  Serial.print("s[t] servo [tweak]      | control robot movement\n");
  Serial.print("l photodiode discharge T| print light sensor value (time)\n");
  Serial.print("d IR distance d         | print range sensor value (10bit adc)\n");
  exec(c,0);
  Serial.print(" HELP");
}
void
prompt()
{
  char buf[3];
  Serial.setTimeout(1);
  if(Serial.available())
    Serial.readBytesUntil('\n', buf, 10);
  Serial.setTimeout(1000*100000);
  Serial.print("MONITOR OK MAX=3\n");
  Serial.print("SEND \\n\\n TO TERMINATE\n");
  while(Serial.readBytesUntil('\n', buf, 3)){
    exec(buf[0],buf[1]);
    buf[0]=0;
    buf[1]=0;
    Serial.print(" ONLINE DEBUG OK\n");
  }
  Serial.print("END ONLINE DEBUG OK DONE\n");
}
void
phase(int phase,int uc){
  Serial.print("\nPHASE:");Serial.println(phase);
  for(p=0;p<phase;p++){
    Serial.print("P");
    if(uc & Serial.available())
      break;
  delay(1000);
  }
  Serial.print("\nPHASE COMPLETE\n");
}
void
post()
{
  Serial.print("\npost\n");
  Serial.print("SENSOR:");
  Serial.print(getlight());
  Serial.print("\t\t[DONE]\n");
  Serial.print("SERVOS:");
  digitalWrite(servopower, HIGH);
  Serial.print("\t\t[DONE]\n");
  delay(1000);
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
  Serial.print("POST\t\tPASS\n");
  Serial.print("PRESS ANY KEY FOR MONITOR\n");
  Serial.print("\nPOWER UP ");
  phase(3,1);
}

void loop()
{
  unsigned int j;
  Serial.print("\n\n\n------------------------------\nTest ROBOT FWver:\t");
  Serial.println(ver);
  post();
  while(1){
    if(Serial.available())
      prompt();
   //if(getlight()<12000)
   if(getdist()<410)
      servoforward();
    else
      servoboust();
    Serial.println(j);
  }
}
