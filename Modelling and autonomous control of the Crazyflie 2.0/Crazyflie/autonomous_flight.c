/*
    Autonomous Program II
    crazyflie-firmware/src/modules/src

Author : Mr Luke Beumont Barret

One of the examples given by M. Barret in his master thesis we have implemented.
*/

// INCLUDES GO HERE *****************************
#include "FreeRTOS.h"
#include "commander.h"
#include "system.h"
#include "autotest.h"
#include "task.h"
#include "led.h"

#include "crtp_localization_service.h"
#include "vl5310x.h"
#include "position_estimator.h"

#include "timers.h"


static xTimerHandle timer;

//Setpoint conditional check
void coordinateCondition(float SP, float SP_prev, int Fastindex, float *TrajectoryOUT)
{
    float dCoord;
    float TrajectorySP;

    if (SP>SP_prev){
        dCoord=(SP-SP_prev)/100;
        TrajectorySP = SP_prev + ((Fastindex) * (dCoord));
    }
    else if(SP<SP_prev){
        TrajectorySP = SP_prev - ((Fastindex)*(dCoord));
    }
    else{
        TrajectorySP=SP;
    }
    *TrajectoryOUT = TrajectorySP;
}

//Set the mode - Create the array & write the setpoint
void setpointbuilder(setpoint_t* setpoint, int indexFast, int indexSlow){
    setpoint_t tempsetpoint;

    //Flight Path Array
    float XCoord[13]={1.55,1.55,1.55,1.55,1.55,2.50,2.50,2.50,1.50,0.50,0.50,1.55,2.50};
    float YCoord[13]={1.55,1.55,1.55,1.55,1.55,1.55,2.00,1.00,0.50,1.00,2.00,2.50,2.00};
    float ZCoord[13]={0.00,0.15,0.50,0.75,1.00,1.15,1.15,1.15,1.15,1.15,1.15,1.15,1.15};

    //Dynamic Setpoint
    float SPx = XCoord[indexSlow];
    float SPy = YCoord[indexSlow];
    float SPz = ZCoord[indexSlow];

    float SPx_prev = 0.0;
    float SPy_prev = 0.0;
    float SPz_prev = 0.0;

    float TjxSP = 0;
    float TjySP = 0;
    float TjzSP = 0;

    if (indexSlow==0){
        SPx_prev = 1.55;
        SPy_prev = 1.55;
        SPz_prev = 0;
    }
    else if (indexSlow > 0){
        SPx_prev = XCoord[indexSlow - 1];
        SPy_prev = YCoord[indexSlow - 1];
        SPz_prev = ZCoord[indexSlow - 1];
    }
    coordinateCondition(SPx, SPx_prev, indexFast, &TjxSP);
    coordinateCondition(SPy, SPy_prev, indexFast, &TjySP);
    coordinateCondition(SPz, SPz_prev, indexFast, &TjzSP);

    tempsetpoint.mode.x = modeAbs;
    tempsetpoint.mode.y = modeAbs;
    tempsetpoint.mode.z = modeAbs;

    tempsetpoint.mode.yaw = modeVelocity;

    tempsetpoint.mode.x = TjxSP;
    tempsetpoint.mode.y = TjySP;
    tempsetpoint.mode.z = TjzSP;

    tempsetpoint.attitudeRate.yaw = 0;

    //Write tempsetpoint to setpoint function call variable structure
    *setpoint=tempsetpoint;
}

extern int fpathindex;
extern int Coordindex;
extern bool togglebit;

void autoTimerCallback(TimerHandle_t timer){
    setpoint_t autosetpoint;
    setpointbuilder(&autosetpoint,fpathindex,Coordindex);

    commanderSetSetpoint(&autosetpoint, AUTOTEST_TASK_PRI);

    fpathindex++;

    if(fpathindex > 100){
        fpathindex=1;
        Coordindex++;
        togglebit = !togglebit;
        if(Coordindex>12){
            Coordindex=7;
        }
    }
    ledSet(LINK_LED, togglebit);
}

void autotestInit(){
    timer=xTimerCreate("LukesTimer", pdMS_TO_TICKS(100), pdTRUE, NULL, autoTimerCallback);
    xTimerStart(timer, 100);
}
