# pi-senseHAT-top
Implementing a top interface for a SenseHAT-LED addon for the pi3-B


# conclusions of design phase
We will implement 3 distinct programs. The first will acquire all necessary
information (what exactly has to be decided) and save them in a nice way
(probably just plain text). 

The second program will then read those files and depending on user-input
(joystick) output the desired information on the LED. 

The third program is a web server which allows to read the acquired data over
the network and also manipulating the settings (yet to be discussed, ideally
setting it on/off, timeframes, etc.). 

# Copyright

This project is licensed under the MIT license.

Copyright (c) 2019 

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

