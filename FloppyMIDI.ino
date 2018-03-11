#include <SoftwareSerial.h>
SoftwareSerial midiIn(10, 16); // RX, TX (though we don't use midi out)

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

byte midiByte;
// byte midiChannel;
byte midiCommand;
byte nowPlaying;
// byte volume;

bool isPlaying = false;
int headPosition = 0; // Starts at 0
const int numTracks = 100; // TODO: is this the case?
bool forward = false; // what direction are we going?

void setup() {
    // setup SoftSerial for MIDI control
    midiIn.begin(31250);
    pinMode(directionPin, OUTPUT);
    digitalWrite(directionPin, forward); // TODO: is this the correct starting direction?
    pinMode(stepPin, OUTPUT);
    digitalWrite(stepPin, HIGH);
}

void loop () {

    if (midiIn.available()) {

        midiByte = midiIn.read();
        midiCommand = midiByte & B11110000; // 4 most significant bits
        // midiChannel = midiByte & B00001111; // 4 least significant bits

        if (midiCommand == midiNoteOn) {
          midiByte = midiIn.read();
          if (midiByte > rangeMin && midiByte < rangeMax) {
            isPlaying = true;
            nowPlaying = midiByte;
            // volume = midiIn.read(); // TODO
          }
        }

        if (midiCommand == midiNoteOff) {
          if (midiIn.read() == nowPlaying) // Make sure it's the note we care about
            isPlaying = false;
        }

    }

    if (nowPlaying) {
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
      delayMicroseconds(10);
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(10);
      headPosition += (forward ? 1 : -1);
      
      delayMicroseconds(2*1000000/noteFreqs[midiCommand - 60] - 20); // TODO: remove octave shift?
    }

}
