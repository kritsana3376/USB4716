/*******************************************************************************

*
* Instructions for Running:
*    1. Set the 'deviceDescription' for opening the device.
*	  2. Set the 'profilePath' to save the profile path of being initialized device. 
*    3. Set the 'startChannel' as the first channel for scan analog samples
*    4. Set the 'channelCount' to decide how many sequential channels to scan analog samples.
*
* I/O Connections Overview:
*    Please refer to your hardware reference manual.
*
******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include "C:/Advantech/DAQNavi/Inc/bdaqctrl.h"
#include "C:/Advantech/DAQNavi/Examples/ANSI_C/inc/compatibility.h"
using namespace Automation::BDaq;
using namespace std;
//-----------------------------------------------------------------------------------
// Configure the following parameters before running the demo
//-----------------------------------------------------------------------------------
#define        deviceDescription  L"USB-4716,BID#0"
const wchar_t* profilePath = L"C:/Advantech/DAQNavi/NavigatorPlugin/Explorer/Profile/USB4716_1.xml";

int32          startChannel = 0;
const int32    channelCount = 1;
int32       channelStart = 0;
int32       channelCount2 = 1;


inline void waitAnyKey()
{
   do{SLEEP(1);} while(!kbhit());
} 
int main(int argc, char* argv[])
{
   ErrorCode        ret = Success;
   ErrorCode        ret2 = Success;
   //ValueRange       valRange;
   
   clock_t begin, end;
   long double elapTicks;
   double elapMilli;
   begin = clock() * CLK_TCK; //START
   
   // Step 1: Create a 'InstantAoCtrl' for Static AO function.
   InstantAoCtrl* instantAoCtrl = InstantAoCtrl::Create();

   // Step 1: Create a 'instantAiCtrl' for InstantAI function.
   InstantAiCtrl * instantAiCtrl = InstantAiCtrl::Create();
   

   do
   {
      // Step 2: Select a device by device number or device description and specify the access mode.
      // in this example we use ModeWrite mode so that we can fully control the device, including configuring, sampling, etc.
      DeviceInformation devInfo(deviceDescription);
      ret = instantAiCtrl->setSelectedDevice(devInfo);
      CHK_RESULT(ret);
      ret = instantAiCtrl->LoadProfile(profilePath);//Loads a profile to initialize the device.
      CHK_RESULT(ret);
     
      //DeviceInformation devInfo(deviceDescription);
      ret2 = instantAoCtrl->setSelectedDevice(devInfo);
      CHK_RESULT(ret2);
      ret2 = instantAoCtrl->LoadProfile(profilePath);//Loads a profile to initialize the device.
      CHK_RESULT(ret2);

      // Step 3: Read samples and do post-process, we show data here.
      printf("Acquisition is in progress, any key to quit!\n\n");
      double scaledData[channelCount] = {};//the count of elements in this array should not be less than the value of the variable channelCount
      int32 channelCountMax = instantAiCtrl->getFeatures()->getChannelCountMax();
      int32 channelCountMax2 = instantAoCtrl->getFeatures()->getChannelCountMax();
      

      //-------------------------------------------------------------------------------------------------------------------------
     
      int32    i = 0;
      MathInterval  ranges[64];
      ValueRange valRange;
      for (i = 0; i < channelCountMax2; i++)
      {
          valRange = instantAoCtrl->getChannels()->getItem(i).getValueRange();
          if (V_ExternalRefBipolar == valRange || valRange == V_ExternalRefUnipolar)
          {
              if (instantAoCtrl->getFeatures()->getExternalRefAntiPolar())
              {
                  double referenceValue;

                  if (valRange == V_ExternalRefBipolar)
                  {
                      referenceValue = instantAoCtrl->getChannels()->getItem(i).getExtRefBipolar();
                      if (referenceValue >= 0) {
                          ranges[i].Max = referenceValue;
                          ranges[i].Min = 0 - referenceValue;
                      }
                      else {
                          ranges[i].Max = 0 - referenceValue;
                          ranges[i].Min = referenceValue;
                      }
                  }
                  else
                  {
                      referenceValue = instantAoCtrl->getChannels()->getItem(i).getExtRefUnipolar();
                      if (referenceValue >= 0) {
                          ranges[i].Max = 0;
                          ranges[i].Min = 0 - referenceValue;
                      }
                      else {
                          ranges[i].Max = 0 - referenceValue;
                          ranges[i].Min = 0;
                      }
                  }
              }
              else
              {
                  double referenceValue;

                  if (valRange == V_ExternalRefBipolar)
                  {
                      referenceValue = instantAoCtrl->getChannels()->getItem(i).getExtRefBipolar();
                      if (referenceValue >= 0) {
                          ranges[i].Max = referenceValue;
                          ranges[i].Min = 0 - referenceValue;
                      }
                      else {
                          ranges[i].Max = 0 - referenceValue;
                          ranges[i].Min = referenceValue;
                      }
                  }
                  else
                  {
                      referenceValue = instantAoCtrl->getChannels()->getItem(i).getExtRefUnipolar();
                      if (referenceValue >= 0) {
                          ranges[i].Max = referenceValue;
                          ranges[i].Min = 0;
                      }
                      else {
                          ranges[i].Max = 0;
                          ranges[i].Min = referenceValue;
                      }
                  }
              }
          }
          else {
              ret2 = AdxGetValueRangeInformation(valRange, 0, NULL, &ranges[i], NULL);
              if (BioFailed(ret2))
              {
                  return ret2;
              }
          }
      }
      //-------------------------------------------------------------------------------------------------------------------------


      do
      {

         //read samples and save to buffer 'scaledData'.
         ret = instantAiCtrl->Read(startChannel, channelCount, scaledData);
         CHK_RESULT(ret);
         ret2 = instantAoCtrl->Write(channelStart, channelCount2, scaledData);
         
         CHK_RESULT(ret2);
         if (scaledData[0] < 0) {
             printf("%f\t", scaledData[0]);
             cout << typeid(scaledData).name();
         }
         else{
             printf("%f\t", scaledData[0]);
             cout << typeid(scaledData).name();
         };

         // process the acquired data. only show data here.
         for (int i = startChannel; i< startChannel+channelCount;++i)
         {
             //end = clock() * CLK_TCK;        //stop the timer

             //elapTicks = end - begin;        //the number of ticks from Begin to End
             //elapMilli = elapTicks / 1000;     //milliseconds from Begin to End
            printf("%d\t", i - startChannel);
            printf("Channel %d data: %10.6f\t", i % channelCountMax, scaledData[i-startChannel]);
            printf("Channel %d data: %f\n\n", i % channelCountMax, scaledData[0]);
           
         }
         //SLEEP(1);
      } while(!kbhit());
   }while(false);

	// Step 4 : Close device and release any allocated resource.
	instantAiCtrl->Dispose();

	// If something wrong in this execution, print the error code on screen for tracking.
	if(BioFailed(ret))
	{
      wchar_t enumString[256];
      AdxEnumToString(L"ErrorCode", (int32)ret, 256, enumString);
      printf("Some error occurred. And the last error code is 0x%X. [%ls]\n", ret, enumString);
		waitAnyKey();// wait any key to quit!
	}
   return 0;
}



