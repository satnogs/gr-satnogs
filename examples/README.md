# Examples
This directory contains a set of useful examples and flowgraphs
for testing and demonstrating the performance of the different components of this
OOT module

## Files
* `morse_ref.wav `: This is a reference file containing the `HELLO WORLD` message.
The file is sampled at 8 KHz and the audio frequency is 700 Hz. The Morse code
Words per Minute (WPM) is about 20.

## Flowgraphs
* `test_matched_filter.grc`: Demonstrates the performance of the implemented
 matched filter for CW decoding.
 
 * `morse_decoding_flowgraph.grc`: This flowgraph decodes a CW signal and 
 prints the corresponding message at the `stdout`. To demonstrate the 
 capabilities of the decoder, signal and noise power GUI sliders are provided
 for easy testing and experimenting. 
 