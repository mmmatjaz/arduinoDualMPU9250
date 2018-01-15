//
// Created by Matjaz on 13/12/2017.
//

#ifndef IMUINO_FUSION_H
#define IMUINO_FUSION_H


#include "gMPU9250.h"

#define Kp 2.0f * 5.0f
#define Ki 0.0f

class Fusion {
public:
    float q[4]= {1.0f, 0.0f, 0.0f, 0.0f};
    float pitch, yaw, roll;

    Fusion(MPU9250&);

    void update();
    void toBytes(float * there);
    int  toChars(char * dest);


private:
    MPU9250& sensor;
    float GyroMeasError = PI * (40.0f / 180.0f);
    float GyroMeasDrift = PI * (0.0f  / 180.0f);
    float beta = sqrt(3.0f / 4.0f) * GyroMeasError;
    float zeta = sqrt(3.0f / 4.0f) * GyroMeasDrift;
    float eInt[3] = {0.0f, 0.0f, 0.0f};

    float deltat = 0.0f;
    uint32_t lastUpdate = 0; // used to calculate integration interval
    uint32_t Now = 0;        // used to calculate integration interval

    void MadgwickQuaternionUpdate();

    void MahonyQuaternionUpdate();

    const float *getQ();
};

#endif //IMUINO_FUSION_H
