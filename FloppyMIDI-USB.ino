#include "MIDIUSB.h"
#include <math.h> // for pow
#include "list.h"
const int directionPin = 2;
const int stepPin = 3;
const int floppyChannels[] = {4,5};

// https://www.midi.org/specifications/item/table-1-summary-of-midi-message
const byte midiNoteOff = B10000000;
const byte midiNoteOn  = B10010000;

// https://www.midikits.net/midi_analyser/midi_note_numbers_for_octaves.htm
// C_3 to C_6
const int rangeMin = 48;
const int rangeMax = 84;


midiEventPacket_t midiPacket;
// byte midiChannel;
byte midiCommand;
List<byte> nowPlaying;
// byte volume;

int headPosition [] = {0,0}; // Starts at 0 for each drive
const int numTracks = 80;
bool forward = false; // what direction are we going?


// https://pages.mtu.edu/~suits/NoteFreqCalcs.html
float calcFreq(int note) {
  int n = note - 69; // 69 is A4
  int f0 = 440; // 440 is freq of A4
  float a = pow(2, 1.0 / 12.0);
  return f0 * pow(a, n);
}


List<float> notePeriodsOrig; // This one is held constant as long as the notes remain unchanged
List<float> notePeriods; // This one changes every iteration of loop()

void setup() {
  pinMode(directionPin, OUTPUT);
  digitalWrite(directionPin, forward);

  pinMode(stepPin, OUTPUT);
  digitalWrite(stepPin, HIGH);

  for (int pin: floppyChannels) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH); // Disable by default
  }
  
  Serial.begin(9600);
}

void loop () {
  
  midiPacket = MidiUSB.read();
  if (midiPacket.header != 0) {

    // Serial.println(midiPacket, BIN);
    midiCommand = midiPacket.byte1 & B11110000; // 4 most significant bits
    // midiChannel = midiPacket & B00001111; // 4 least significant bits

    if (midiCommand == midiNoteOn) {
      if (midiPacket.byte2 >= rangeMin && midiPacket.byte2 <= rangeMax) {        
        nowPlaying.append(midiPacket.byte2);
        // Add new note to frequencies
        float period = 1.0/calcFreq(midiPacket.byte2);
        // Append period to peroid lists
        notePeriodsOrig.append(period);
        notePeriods.append(period);
        // volume = midiIn.read(); // TODO
      }
    } else if (midiCommand == midiNoteOff) {
      if (nowPlaying.in(midiPacket.byte2)){ // Make sure it's a note we care about
        int index = nowPlaying.index(midiPacket.byte2);
        // Remove that value from all the parrell lists
        nowPlaying.pop(index);
        notePeriodsOrig.pop(index);
        notePeriods.pop(index);
      }   
    }
  }

  if (nowPlaying.getLength() > 0) {
    // limits
    float minimum = notePeriods.minimum();
    int minIndex = notePeriods.index(minimum);
    digitalWrite(floppyChannels[minIndex], LOW);


    // Calculate which direction to move the head.
    if (headPosition[minIndex] >= numTracks) {
      forward = true;
      digitalWrite(directionPin, LOW);
    }
    else if (headPosition[minIndex] <= 0) {
      forward = false;
      digitalWrite(directionPin, HIGH);
    }
    
    

    for (int i = 0; i < notePeriods.getLength(); i++) {
      notePeriods[i] -= minimum;
    }

    // Reset that period back to full length because we have to wait
    // another full period before it plays again
    notePeriods[minIndex] = notePeriodsOrig[minIndex];

    
    // step
    digitalWrite(stepPin, LOW);
    delayMicroseconds(20);
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(20);
    headPosition[minIndex] += (forward ? -1 : 1);
    digitalWrite(floppyChannels[minIndex], HIGH);
    delayMicroseconds(minimum*1000000*2);
    
  }

}
