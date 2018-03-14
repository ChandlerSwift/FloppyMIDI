
#include "MIDIUSB.h"
#include <math.h> // for pow

const int directionPin = 2;
const int stepPin = 3;

// https://www.midi.org/specifications/item/table-1-summary-of-midi-message
const byte midiNoteOff = B10000000;
const byte midiNoteOn  = B10010000;

// https://www.midikits.net/midi_analyser/midi_note_numbers_for_octaves.htm
// C_4 to C_5
const int rangeMin = 44;
const int rangeMax = 72; 

midiEventPacket_t midiPacket;
// byte midiChannel;
byte midiCommand;
byte nowPlaying = 60;
// byte volume;

bool isPlaying = false;
int headPosition = 0; // Starts at 0
const int numTracks = 80; // TODO: is this the case?
bool forward = false; // what direction are we going?

// https://pages.mtu.edu/~suits/NoteFreqCalcs.html
float calcFreq(int note){
  int n = note - 69; // 69 is A4
  int f0 = 440; // 440 is freq of A4
  float a = pow(2, 1.0 / 12.0);
  return f0 * pow(a,n);
}

void setup() {
    pinMode(directionPin, OUTPUT);
    digitalWrite(directionPin, forward);

    pinMode(stepPin, OUTPUT);
    digitalWrite(stepPin, HIGH);

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
            isPlaying = true;
            nowPlaying = midiPacket.byte2;
            // volume = midiIn.read(); // TODO
          }
        } else if (midiCommand == midiNoteOff) {
          if (midiPacket.byte2 == nowPlaying) // Make sure it's the note we care about
            isPlaying = false;
        }

    }

    if (isPlaying) {
      // limits
      if (headPosition >= numTracks) {
        forward = true;
        digitalWrite(directionPin, LOW);
      }
      else if (headPosition <= 0) {
        forward = false;
        digitalWrite(directionPin, HIGH);
      }

      // step
      digitalWrite(stepPin, LOW);
      delayMicroseconds(2000);
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(2000);
      headPosition += (forward ? -1 : 1);
      delayMicroseconds(2*1000000/calcFreq(nowPlaying) - 4000);
    }

}
