
#include "MIDIUSB.h"

const int directionPin = 2;
const int stepPin = 3;

// https://www.midi.org/specifications/item/table-1-summary-of-midi-message
const byte midiNoteOff = B10000000;
const byte midiNoteOn  = B10010000;

// https://pages.mtu.edu/~suits/notefreqs.html
// C_4 to C_5
const float noteFreqs[] = {261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.00, 415.30, 440.00, 466.16, 493.88, 523.2};

// https://www.midikits.net/midi_analyser/midi_note_numbers_for_octaves.htm
// C_4 to C_5
const int rangeMin = 60;
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

void setup() {
    // setup SoftSerial for MIDI control
    pinMode(directionPin, OUTPUT);
    digitalWrite(directionPin, forward); // TODO: is this the correct starting direction?
    pinMode(stepPin, OUTPUT);
    digitalWrite(stepPin, HIGH);
    Serial.begin(9600);
}

void loop () {
    midiPacket = MidiUSB.read();
    if (midiPacket.header != 0) {

        // Serial.println(midiPacket, BIN);
        Serial.println("gotem");
        midiCommand = midiPacket.byte1 & B11110000;
        // midiChannel = midiPacket & B00001111; // 4 least significant bits

        if (midiCommand == midiNoteOn) {
          if (midiPacket.byte2 >= rangeMin && midiPacket.byte2 <= rangeMax) {
            isPlaying = true;
            nowPlaying = midiPacket.byte2;
            // volume = midiIn.read(); // TODO
          }
        }

        if (midiCommand == midiNoteOff) {
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
      Serial.println(2*1000000/noteFreqs[nowPlaying - 60]);
      delayMicroseconds(2*1000000/noteFreqs[nowPlaying - 60] - 4000); // TODO: remove octave shift?
    }

}
