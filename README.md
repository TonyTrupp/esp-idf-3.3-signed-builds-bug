# esp-idf-3.3-signed-builds-bug

The build won't complete with Secure Features - > Require Signed App Images & Verify app signature on update enabled with ESP-IDF 3.3. I need to use 3.3 because Arduino-ESP32 doesn't support any later versions.   

This appears to be similar to the issue described here: 
https://github.com/espressif/esp-idf/issues/3864  

And fixed within this commit within 4.1: 
https://github.com/espressif/esp-idf/commit/75488f18060553d7853e3c93cb6264d112b8eb07
