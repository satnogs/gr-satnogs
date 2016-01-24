# gr-satnogs: SatNOGS GNU Radio Out-Of-Tree Module
gr-satnogs is an out-of-tree GNU Radio module that provides all the necessary tools
for decoding signals from various scientific and academic sattelites.

## Install

### Requirements
1. GNU Radio ( > 3.7.2 )
2. CMake ( > 3.1)
3. G++ (with C++11 support)
4. VOLK
5. git

### Installation

1. `git clone https://github.com/satnogs/gr-satnogs.git`
2. `cd gr-satnogs`
3. `mkdir build`
4. `cmake ..`
5. `make`
6. `sudo make install`

If this is the first time you are building the gr-satnogs module run
`sudo ldconfig`

## Website
For more indormation about SatNOGS please visit our [site](https://satnogs.org/).

## License

&copy; 2016 [Libre Space Foundation](http://librespacefoundation.org).

Licensed under the [GPLv3](LICENSE).