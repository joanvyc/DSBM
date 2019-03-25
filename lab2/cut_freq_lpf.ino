#define PIN_IN A1
#define WIN_SIZE 8 // WIN_SIZE must be multiple of 2
#define WIN_SIZE_log 3

#define MAX_A (310)
#define CUT_A (MAX_A*0.70)

#define DELAY 500
unsigned int ant, cur;
int creixent;
unsigned int last_time,time_val, t;
int index;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  cur = ant = 0;
  for (int i = 0; i < WIN_SIZE; i++) {
    ant += analogRead(PIN_IN);
    delayMicroseconds(DELAY);
  }
  ant >>= WIN_SIZE_log;
  
  last_time = micros();
  creixent = 0;
  index = 0;
  t = 0;
}


void loop() {
  // put your main code here, to run repeatedly:
  if (index == WIN_SIZE) {
    cur >>= WIN_SIZE_log;
    //Serial.print(cur);
    if (cur < ant && creixent) {
      unsigned int dt = t; // - last_time;
      //Serial.print("==> MR");
        Serial.print(ant);
        Serial.print(" => ");
        Serial.print(1600.f/ (float)dt); 
        Serial.print("Hz,");
      if (ant < CUT_A) { 
        Serial.print("TALL");
      }
      Serial.println(".");
      t = 0;
      //last_time = t;
    }
    //Serial.println("");
    creixent = cur == ant ? creixent : cur > ant;
    ant = cur;
    cur = index = 0;
  }

  index++;
  t++;
  cur += analogRead(PIN_IN);
  delayMicroseconds(DELAY);
}
