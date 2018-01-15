/* this example either prints raw readouts in a human readable output twice per second or sreams the data in a binary struct 
The mode is determined by the input on a digital pin - see the define statement - but can be hardcoded instead.

*/

#include "gMPU9250.h"
#include "Fusion.h"

#define LED_PIN       13
#define MODE_PIN      12
#define PRINT_PERIOD 500 // miliseconds
#define SERIAL_PRINT digitalRead(MODE_PIN)//true  // Set to true to get Serial output for debugging
// false for stream


MPU9250 imu1(false);
Fusion fus1(imu1);
MPU9250 imu2(true);
Fusion fus2(imu2);

typedef struct {
    uint8_t address;
    uint32_t timeStamp;
    float a[3];
    float g[3];
    float m[3];
    float q[4];
    uint8_t x = '\n';
} Paket;
Paket pak1, pak2;

// periodic print
unsigned long lastPrint = 0;
unsigned long lastRead = 0;
int sampleCount = 0; // sampling freq monitor

void setupMPU(MPU9250 &mpu) {
    mpu.MPU9250SelfTest();
    mpu.calibrateIMU();
    mpu.initMPU9250();

    mpu.togglePassthrough(true);/*
    if (!mpu.pingMAG()) {
        digitalWrite(LED_PIN, HIGH);
        Serial.print("Could not connect to the MAG 0");
        while (1);
    }*/
    mpu.setMagBias(0., 0., 0.);
    mpu.initAK8963(mpu.magCalibration);
    mpu.togglePassthrough(false);
}

void printData(MPU9250 &imu, Fusion &fus) {
    Serial.print(imu.address);

    Serial.print(": acc[mg]= ");
    Serial.print(1000 * imu.ax);
    Serial.print(" ");
    Serial.print(1000 * imu.ay);
    Serial.print(" ");
    Serial.print(1000 * imu.az);

    Serial.print(" rpy= ");
    Serial.print(fus.roll);
    Serial.print(" ");
    Serial.print(fus.pitch);
    Serial.print(" ");
    Serial.print(fus.yaw);

    Serial.println();
}

void sendData(Paket &pak,MPU9250 &imu, Fusion &fus){
    pak.address = imu.address;
    pak.timeStamp = millis();
    pak.a[0] = imu.ax;
    pak.a[1] = imu.ay;
    pak.a[2] = imu.az;
    pak.g[0] = imu.gx;
    pak.g[1] = imu.gy;
    pak.g[2] = imu.gz;
    pak.m[0] = imu.mx;
    pak.m[1] = imu.my;
    pak.m[2] = imu.mz;
    memcpy(pak.q, fus.q, 16);
    Serial.write((const char *) &pak, sizeof(pak));
}

void setup() {
    Serial.begin(38400);
    pinMode(MODE_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    I2c.setSpeed(1);
    I2c.begin();
    
    
    if (!imu1.pingIMU() || !imu2.pingIMU()) {
        digitalWrite(LED_PIN, HIGH);
        Serial.print("Could not connect to MPUs");
        while (1);
    }

    setupMPU(imu1);
    setupMPU(imu2);
    imu1.togglePassthrough(true);
}

void loop() {
    if (imu1.haveNewData()) {
            sampleCount++;
        imu1.readAccelData();// Read the x/y/z adc values
        imu1.readGyroData(); // Read the x/y/z adc values      
        imu1.togglePassthrough(true);
        imu1.readMagData();  // Read the x/y/z adc values
        imu1.togglePassthrough(false);       
        fus1.update();
        if (!SERIAL_PRINT)
            sendData(pak1, imu1, fus1);
    }

    if (imu2.haveNewData()) {
        imu2.readAccelData();// Read the x/y/z adc values
        imu2.readGyroData(); // Read the x/y/z adc values
        imu2.togglePassthrough(true);
        imu2.readMagData();  // Read the x/y/z adc values
        imu2.togglePassthrough(false);

        fus2.update();
        if (!SERIAL_PRINT)
            sendData(pak2, imu2, fus2);
    }

    if (SERIAL_PRINT && (millis() - lastPrint > PRINT_PERIOD)) {
        int freq = sampleCount * (1000 / PRINT_PERIOD);
        sampleCount = 0;
        Serial.print(" f=");
        Serial.print(freq);Serial.print(" | t= ");
        Serial.println();

        lastPrint = millis();
        printData(imu1,fus1);
        printData(imu2,fus2);
    }
}
