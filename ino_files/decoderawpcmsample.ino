#include "math.h"
#include "application.h"

//declare variables
const short sampleRate = 11025;
short curArrayIndex = 0; //max of 1102
short errorsInSamp = 0;
//short maxSample = 0;
//short minSample = 0;
//int avgSample = 0;
short tmpForComp = 0;

short micAL1SecSmp[sampleRate/10]; //use 1/10 second samples, analyze the array in 1/10 second intervals(every 100ms, analyze)
unsigned long lastSampTime = 0;
unsigned int curSampTimeDiff;
char publishSamples[64];
int curValue = 0;

//for debugging
long counter = 0;

//use this for calculating TDOA.  Basically, figure out the time that the sample arrives at each of the mics
//http://stackoverflow.com/questions/8541776/tdoa-multilateration-to-locate-a-sound-source
const int soundSpdmilliMetersPerSecond = 340290;

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
    Spark.function("getCount", getCount);
    
    
    
    Spark.publish("Samples","Definitely Connected!");


  // variable name max length is 12 characters long
 // Spark.variable("micAL1SecSmp", &micAL1SecSmp, INT); == only supports double, string, and int

}

int getCount(String command)
{
    //can handle the 'command' conditionally
    return (int) ( counter ); 
}

void loop() {

// every 30 seconds, push diagnostic data.  In the final product, this can happen between volleys.
if (counter>=330750){
    counter = 0;
    WiFi.on();
    Spark.connect();
    Serial.print("Connected and pushing!");
    sprintf(publishSamples,"%hi:%hi:%hi",micAL1SecSmp[0],micAL1SecSmp[512],micAL1SecSmp[1000]);
    Spark.publish("Samples",publishSamples);
    //delay(15000);
   // WiFi.off();
   // Spark.disconnect();
}



curSampTimeDiff = micros() - lastSampTime;

//So 11025Hz comes out to sampling approximately every 90.7 microseconds.  Since samples take around 3us,
//go ahead and do this every 85 microseconds, to be safe.
if (curSampTimeDiff < 85){
    //delay until its time to sample again
    delayMicroseconds(85-curSampTimeDiff);
    // with wifi/spark off , sampling 4 inputs and calling micros() takes about 2-3 us
    lastSampTime = micros();
    curValue = analogRead(A7);
    curValue = analogRead(A6);
    curValue = analogRead(A1);
    curValue = analogRead(A0);
    counter++;
    

    //Keep filling the array until we are full-up on samples.
    if(curArrayIndex < sampleRate){
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

    
    
}

//haven't quantified, but every so often the loop takes closer to 160us to execute.
else{
    errorsInSamp++;
    //Serial.print("Exec time delay!:");
    //Serial.println(curSampTimeDiff);
}

//Serial.println(curValue);



}

