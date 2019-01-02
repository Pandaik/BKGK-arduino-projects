#include <RCSwitch.h>
#include <DS3231.h>

//Define constants
const int NumButtonsOnController = 5;   // Number of buttons on the 
const int ElevenFiftyNinePM = 1439;     // 11:59 PM converted to an integer
const int Midnight = 0;                 // Midnight converted to an integer

// Sniff your button codes and put the values for each button here
const long Button1RFOnCode = 4461875;
const long Button1RFOffCode = 4461884;
const long Button2RFOnCode = 4462019;
const long Button2RFOffCode = 4462028;
const long Button3RFOnCode = 4462339;
const long Button3RFOffCode = 4462348;
const long Button4RFOnCode = 4463875;
const long Button4RFOffCode = 4463884;
const long Button5RFOnCode = 4470019;
const long Button5RFOffCode = 4470028;

// Put the on and off time for each timer
const String Switch1OnTime = "17:00";
const String Switch1OffTime = "23:30";
const String Switch2OnTime = "17:00";
const String Switch2OffTime = "21:30";
const String Switch3OnTime = "00:00";
const String Switch3OffTime = "00:00";
const String Switch4OnTime = "00:00";
const String Switch4OffTime = "00:00";
const String Switch5OnTime = "00:00";
const String Switch5OffTime = "00:00";

String onTimes[] = {Switch1OnTime, Switch2OnTime, Switch3OnTime, Switch4OnTime, Switch5OnTime};
String offTimes[] = {Switch1OffTime, Switch2OffTime, Switch3OffTime, Switch4OffTime, Switch5OffTime};

long onCode[] = {Button1RFOnCode, Button2RFOnCode, Button3RFOnCode, Button4RFOnCode, Button5RFOnCode};
long offCode[] = {Button1RFOffCode, Button2RFOffCode, Button3RFOffCode, Button4RFOffCode, Button5RFOffCode};

bool isOn[NumButtonsOnController];
int tOn[NumButtonsOnController];
int tOff[NumButtonsOnController];
int CurrentTime;

DS3231 rtc(SDA, SCL);

RCSwitch mySwitch = RCSwitch();

Time t;

void setup() {
   
  for (int i = 0; i < NumButtonsOnController; i++) {
    tOn[i] = timeToInt(parseHour(onTimes[i]), parseMin(onTimes[i]));
    tOff[i] = timeToInt(parseHour(offTimes[i]), parseMin(offTimes[i]));
    isOn[i] = false;
  }

  // CurrentTime = timeToInt(t.hour, t.min);

  mySwitch.enableTransmit(10);
  mySwitch.setPulseLength(183);

  rtc.begin();
}

void loop() {
  // Get the time from the RTC
  t = rtc.getTime();

  // Convert the time to an integer
  CurrentTime = timeToInt(t.hour, t.min);
  
  for (int i = 0; i < NumButtonsOnController; i++) {
    if (onTimes[i] != offTimes[i]) {

      // Use case when the on time is less than the off time
      if (tOn[i] < tOff[i]) {

        //  Turn the switch on when the current time is greater than the on time and less than the off time and the switch is off
        if (CurrentTime >= tOn[i] && !isOn[i] && CurrentTime < tOff[i]) {
          mySwitch.send(onCode[i], 24);
          isOn[i] = true;
          delay(100);
        }

        // Turn the swtich off when the current time is greater than the off time and the switch is off
        if (CurrentTime >= tOff[i] && isOn[i]) {
          mySwitch.send(offCode[i], 24);
          isOn[i] = false;
          delay(100);
        }
      } else {
        if (((CurrentTime >= tOn[i] && CurrentTime < ElevenFiftyNinePM) || (CurrentTime >= Midnight && CurrentTime < tOff[i])) && !isOn[i]) {
          mySwitch.send(onCode[i], 24);
          isOn[i] = true;
          delay(100);
        }
        if (CurrentTime >= tOff[i] && CurrentTime < tOn[i] && isOn[i]) {
          mySwitch.send(offCode[i], 24);
          isOn[i] = false;
          delay(100);
        }
      }
    }
  }
  delay(100);
}

int parseHour(String timeString) {
  return timeString.substring(0, 2).toInt();
}

int parseMin(String timeString) {
  return timeString.substring(3, 5).toInt();
}

int timeToInt(int h, int m) {
  return (h * 60) + m;
}
