#include "math.h"
#include "application.h"
#include "spark_disable_wlan.h"
#include "spark_disable_cloud.h"

//declare variables
const short sampleRate = 1102;
short curArrayIndex = 0; //max of 1102
short errorsInSamp = 0;
//short maxSample = 0;
//short minSample = 0;
//int avgSample = 0;
short tmpForComp = 0;

short micAL1SecSmp[sampleRate]; //use 1/10 second samples, if we sample at 11025, we will fill this array in 100ms.
unsigned long lastSampTime;
unsigned int curSampTimeDiff;
char publishSamples[64];
int curValue = 0;
long counter = 0;
//for debugging

//use this for calculating TDOA.  Basically, figure out the time that the sample arrives at each of the mics
//http://stackoverflow.com/questions/8541776/tdoa-multilateration-to-locate-a-sound-source
const long soundSpdmilliMetersPerSecond = 340290;

//if we get a 'sound' in the input stream, then we will act on it and reset (instead of waiting for the end of)
void setup() {

// analog input for each mic (see https://docs.google.com/drawings/d/12OdPwacGCoI-6NfFAYS2rjWgYxKWuHG0zupCH1tGLEQ/edit?hl=en&forcehl=1)
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    pinMode(A6, INPUT);
    pinMode(A7, INPUT);
    
    //with wifi/spark cloud on, max speed for each loop is around 5ms (on home network)
  //  WiFi.off();
  //  Spark.disconnect();
    Spark.connect();
    Serial.begin(9600); 
    Spark.publish("ConnStat","Definitely Connected!");
// digital input for the piezo
// setups for the cloud?

  // variable name max length is 12 characters long
 // Spark.variable("micAL1SecSmp", &micAL1SecSmp, INT); == only supports double, string, and int

}

void loop() {

//could check every 10 samples or so to prevent slowing down the core too much
//can potentially also speed up code by keeping it all in a 'while 1' inside this loop.  Keeps the processing smooth... then when a 'game' is done, exit.
// e.g. "while game is being played, focus on the mics and processing the audio"

curSampTimeDiff = micros() - lastSampTime;
if (curSampTimeDiff < 90){
    delayMicroseconds(90-curSampTimeDiff);
    //Serial.print("delaying ");
    //Serial.println(curSampTimeDiff);
    counter++;
}

//haven't quantified, but every so often the loop takes closer to 160us to execute.
else{
    errorsInSamp++;
    counter++;
   // Serial.print("Exec time delay!:");
    //Serial.println(curSampTimeDiff);
}

// with wifi/spark off , the loop executes in about 2-3 us (sampling 4 inputs and calling micros())
lastSampTime = micros();
curValue = analogRead(A7);
curValue = analogRead(A6);
curValue = analogRead(A1);
curValue = analogRead(A0);

//fill the current value into the array and normalize it 
if(curArrayIndex < 1102){
    //if (tmpForComp > maxSample){
      //  maxSample = tmpForComp;
    //}
    //else if(tmpForComp <= minSample)
    //{
    //    minSample = tmpForComp;
    //}
  //  avgSample += tmpForComp;
    micAL1SecSmp[curArrayIndex] =(curValue * 1000) / 4095;
    curArrayIndex++;
}
else{
    curArrayIndex=0;
    counter++;
  //  avgSample = avgSample / 1102;
    Serial.print("Array Index:");
    Serial.println(curArrayIndex);
    for (int i = 0; i < arraySize(micAL1SecSmp); i++) {
        Serial.print(micAL1SecSmp[i]);
        Serial.print( " | ");
        if (i % 20 == 1){
            Serial.println(".");
        }
    }
    Serial.print("Errors:");
    Serial.println(errorsInSamp);
    errorsInSamp=0;
}

// every 10 seconds push.
if (counter==100){
    counter = 0;
    //WiFi.on();
    //Spark.connect();
    Serial.print("Connected and pushing!");
    sprintf(publishSamples,"%hi:%hi:%hi",micAL1SecSmp[0],micAL1SecSmp[512],micAL1SecSmp[1000]);
    Spark.publish("Samples",publishSamples);
    //delay(15000);
}


//Serial.println(curValue);


//write the current time to last 
//sample the mics, add value
// if elapsed time is > 0.25 second, process the array regardless of whether it is full or not
//if time 

}

