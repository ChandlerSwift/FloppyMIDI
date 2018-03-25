# USBMIDI -- MIDI resources

# Instructions
For testing, we use [`sendmidi`](https://github.com/gbevin/SendMIDI/) to send
MIDI to the device. To install:
```bash
wget https://github.com/gbevin/SendMIDI/releases/download/1.0.13/sendmidi-linux-1.0.13.tar.gz
tar xf sendmidi-linux-1.0.13.tar.gz
rm sendmidi-linux-1.0.13.tar.gz
```

# Files
`mary.txt`: Mary Had a Little Lamb, for one drive

# MIDI Passthrough

```bash
(echo "dev \"SparkFun Pro Micro MIDI 1\""; ./receivemidi-linux-1.0.5/receivemidi dev "USB-MIDI MIDI 1") | ./sendmidi-linux-1.0.13/sendmidi --
```
