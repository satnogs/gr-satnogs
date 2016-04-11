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
 
 * `ax25_example.grc`: This flowgraph semonstrates the AX.25 encoder and 
 decoder. It has two different AX.25 payload source blocks. The first one is the
 `Debug Message Source` block that assigns as payload a predefined message,
 whereas the second one is a `UDP Message Source` block that receives the
 AX.25 payload from a UDP packet.
 
 * `test_rigctl.grc`: This particular flowgraph demonstrates the capabilities
 of the module implementing a portion of the `rigctld` functionality.
 It received `rigctl` commands through a `TCP` socket and produces the appropriate
 messages. In the example these messages are received from the Doppler correction
 block. To generate `rigctl` commands you can use the `GPredict` software.
 Note that you have to specify the frequency of the transmitter satellite.
 As this block does not use any satellite signal rather than a simple sine, you
 do not expect any correction at all. However you will be able to see how smoothly
 the sine peak is drifting. This drift in normal situations will be the correction offset.
 