This is a modification of Kris Winer's code.
Changes: 
- Instead of wire.h the library uses I2c: https://www.i2cdevlib.com/usage
- cleaned up the LCD output stuff
- organised the code in classes

The example shows how to read two MPUs. However, the sample rate drops substantially when doing that. I suppose the reason is in the way that the magnetometer is read. Turning the passthrough mode on and off is not a very efficient solution. It should be possible to configure the MPU to read the magnetomoter periodically and then serve the readings to the arduino, but haven't looked into it. Any attempt to solve this would be very welcome ;)