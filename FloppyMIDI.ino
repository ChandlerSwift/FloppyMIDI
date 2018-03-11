#include <SoftwareSerial.h>
SoftwareSerial midiIn(10, 16); // RX, TX (though we don't use midi out)

int directionPin = 

// https://www.midi.org/specifications/item/table-1-summary-of-midi-message
const byte midiNoteOff = B10000000;
const byte midiNoteOn  = B10010000;

// https://pages.mtu.edu/~suits/notefreqs.html
// C_4 to C_5
const float noteFreqs = [261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.00, 415.30, 440.00, 466.16, 493.88, 523.25];

// https://www.midikits.net/midi_analyser/midi_note_numbers_for_octaves.htm
const int rangeMin = 60;
const int rangeMax = 72; 

byte midiByte;
// byte midiChannel;
byte midiCommand;
byte nowPlaying;
// byte volume;

bool isPlaying = false;
int headPosition = 0; // Starts at 0
const int tracks = 100; // TODO: is this the case?
bool forward = true;

void setup() {
    // setup SoftSerial for MIDI control
    midiIn.begin(31250);
}

void loop () {

    if (midiIn.available()) {

        midiByte = midiIn.read();
        midiCommand = midiByte & B11110000; // 4 most significant bits
        // midiChannel = midiByte & B00001111; // 4 least significant bits

        if (midiCommand == midiNoteOn) {
          isPlaying = true;
          nowPlaying = midiIn.read();
          // volume = midiIn.read(); // TODO
        }

        if (midiCommand == midiNoteOff) {
          if (midiIn.read() == nowPlaying)
            isPlaying = false;
        }

    }

    if (nowPlaying) {
      delayMicroseconds(1000000/noteFreqs[midiCommand - 60]
    }

}
