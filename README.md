
A Proof of Concept RTL2832u Plugin for the SDR Software Quisk 
-------------------------------------------------------------

0) As a preliminary matter, make sure the rtl-sdr software library from osmocom is installed, that dvd_usb_rtl28xxu is blacklisted, and, optionally, that non-root users can access the RTLSDR hardware device through the appropriate udev rules.

1) To install this plugin, first untar Quisk version 3.6.3.
1.a) verify that quisk will build and run before continuing.

2) Next, replace quisk-3.6.3/setup.py file with the setup.py file provided by this repo.

3) In addition, copy the rtlsdrpkg directory provided by this repo into the quisk-3.6.3 source tree.

4) Also copy configuration file quisk_conf.py to ~/.quisk_conf.py.

5) Then either,

   a) run the command 'python setup.py install' to build and install Quisk, or

   b) to test first, run the command 'python setup.py build' in the quisk-3.6.3 tree, then cd into build/lib.linux-x86_64-2.7/quisk and execute 'python quisk.py' to run Quisk.

Notes
-----

This POC was written for a Ubuntu computer and will 100% fail on Windows. 

The RTLSDR is configured to use a fixed sampling (display) rate of 960000 hertz.  Unfortunately since I could not get Quisk to support a sampling rate greater than 192000 hertz, the raw RTLSDR samples had to be decimated.

KC7NOA: sample rate of RTL2838 is set to the max (3200000hz)-- changed in quisk_hardware.py from XXXX

To support Quisk sampling Display rates of 192000, 96000, and 48000 the raw samples are decimating by 5, 10, and 20 respectively.  By default Quisk uses a 48000 sampling rate.   This can be changed by editing the ~/.quisk_conf.py configuration file.  Support for direct sampling mode is also provided through the configuration file.

No filtering before decimation is performed.  I am not good enough at digital FIR filtering to know the best coefficients or number of taps to use.  I did experiments with a raised cosine filter with 5 taps but the results did not seem acceptable.  Please submit a pull request if you can help.

George Magiros
KC7NOA = KC7NOA at gee male bop com

18 May 2015

Copyright 2015

License
---------------------------
This mod is released under the GNU General Public Licence v3.0


