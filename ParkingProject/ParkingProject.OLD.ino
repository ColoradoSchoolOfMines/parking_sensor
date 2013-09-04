
#define NOTE_C6  1047
#define NOTE_E6  1319
#define NOTE_G6  1568

// notes in the melody:
int melody[] = {
  NOTE_C6, NOTE_G6};
int noteDurations[] = {
  8,8};
  
  int trigger_value0; // pressure reading threshold for identifying a bike is pressing.
  int trigger_value1;
  int threshold = 12; //change this amount if necessary. tunes sensitivity.
  int the_tally; //total amount of sensings.
  int incomingByte = 0;   // for incoming serial data
  int pressure0;
  int pressure1;
  
  
void setup() {
  pinMode(A0, INPUT); //sensor closest to lot 
  pinMode(A1, INPUT); //Sensor closest to street
  pinMode(2, OUTPUT);
  pinMode(13, OUTPUT); //Music tone from buzzer, attached to pin 13
  Serial.begin(57600);
  

  delay(1000);
  

  
  Serial.println("Hello, Welcome to the Mines ");
  
        // read local air pressure and create offset.
  trigger_value0 = analogRead(A0) + threshold;
  trigger_value1 = analogRead(A1) + threshold;
  
  Serial.println("Developed for ACMx by Roy Stillwell, Colorado School of Mines 2013 ");
  Serial.println("___________________________________________________");
  Serial.println("");
  Serial.print("Local Air Pressure Line 0: ");
  Serial.println(trigger_value0 - threshold);
    Serial.print("Local Air Pressure Line 1: ");
  Serial.println(trigger_value1 - threshold);
  Serial.println("___________________________________________________");
  Serial.println("");

}

void loop() {
  pressure0 = analogRead(A0);
  pressure1 = analogRead(A1);
  
 //1 - TUBE 0 IS PRESSURIZED 
  if (pressure0 > trigger_value0) {
      Serial.print("");
      Serial.println("Sensor 0 Hit!");
      Serial.print("Tube0Pressure|");
      Serial.print(pressure0);
      Serial.print("|");
      Serial.println("");
      make_tone(); //will buzz if buzzer attached, also LED on pin 13 will flash.
    }
 //2 - TUBE 1 IS PRESSURIZED
  if (pressure1 > trigger_value1) {
      Serial.print("");
      Serial.println("Sensor 1 Hit!");
      Serial.print("Tube1Pressure|");
      Serial.print(pressure1);
      Serial.print("|");
      Serial.println("");
      make_tone(); //will buzz if buzzer attached, also LED on pin 13 will flash.
    }
  }
  

void make_tone() {
  for (int thisNote = 0; thisNote < 2; thisNote++) {

    //to calculate the note duration, take one second 
    //divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    tone(13, melody[thisNote],noteDuration);

    //to distinguish the notes, set a minimum time between them.
    //the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    //stop the tone playing:
    noTone(13);
  }
}
