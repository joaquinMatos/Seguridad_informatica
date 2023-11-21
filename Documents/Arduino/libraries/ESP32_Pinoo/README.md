# ESP32-Pinoo
An Arduino library to use ESP32 cards over Wifi on ESP32 boards.


Quick start
-----------

```
#include "ESP32_Pinoo.h"      

Network *network;

void initNetwork(){ /* OUR LIBRARY CONNECTS TO WI-FI WITH THE CODES INSIDE  */
  network = new Network();
  network->initWiFi();
}

void setup(){
  Serial.begin(115200);
  Serial.println();
  
  initNetwork();
}
```

Check the website [here](https://www.pinoo.io/) to view all available Pinoo Cards.

Future work
-----------

- Add some more examples
- Add documentation, with Doxygen ?
- Add keywords.txt for Arduino IDE

- Add support for realtime messages ?
- Add debugging messages in BLEMidiServer ?
- Better debug function

Message to users
----------------
I would be glad to see your projects! Don't hesitate to drop me an e-mail. (dnzayan@hotmail.com)
