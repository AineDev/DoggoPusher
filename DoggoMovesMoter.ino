#include <WiFi101.h>
#include <Servo.h>

//#include <Adafruit_GFX.h>
//#include <Adafruit_PCD8544.h>

char ssid[] = "Aine's iPhone";
char pass[] = "12345678";
int status = WL_IDLE_STATUS;
WiFiClient client;
Servo myservo;
char server[] = "api.twitter.com";

const int button = 6;
const int redLED = 8;
const int greenLED = 7;

const long debounceDelay = 50;
int stateButton = 0;
int lastStateButton = 0;
int readingButton = 0;
int delays = 0;
long lastDebounceTimeButton = 0;

bool printOn = false;
int countUntilTweet = 0;
bool printCheck = true;
int printedCharacterCount = 0;

// the newest tweet we found...
String newID = "";
String acc = "";
String id = "";
String currYear = "";
String currDate = "";
String currMonth = "";
String currDay = "";
String currTime = "";
int currHour;
int currMin;
int currSec;
bool inResponse = false;

void setup() {
  Serial.begin(9600);
//  pinMode(button, INPUT);
  pinMode(9, OUTPUT);

  delay(1000);

  while (status != WL_CONNECTED)
  {
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }
  myservo.attach(9);
  Serial.println("hello, please press '?'");
}

void loop() {
  CheckButton();
  //  Serial.println("Truely entered loop");
  char end   = '\n';
  char comma = ',';
  char colon = ':';
  char leftbrace = '{';
  char rightbracket = ']';

  bool inID = false;

  // int l = 14; // length of string
  while (client.available()) {
    // read a new character
    char c = client.read();

    // go until we find a left brace
    if (c == leftbrace) {
      inResponse = true;
    }

    //  start looking for date tweeted
    if (inResponse) {
      if (c == rightbracket) {          //no tweets are there
      }
      if (c == comma) {                 //some response
        if (acc.startsWith("\"since_id_str\":\"0\"}}{\"statuses\":[{\"created_at")) {
          acc = acc.substring(20);
        }
        if (acc.startsWith("{\"statuses\":[{\"created_at") || acc.startsWith("\"since_id_str\":\"0\"}}{\"statuses\":[{\"created_at")) { // find the time it was created
          if (isLater(acc)) {                                 // true if found date is strictly later than the previously seen tweet
//            digitalWrite(9, HIGH);   // turn the LED on (HIGH is the voltage level)
//            delay(1000);                       // wait for a second
//            digitalWrite(9, LOW);    // turn the LED off by making the voltage LOW
//            delay(1000);

              int counter = 0;
              int pos = 0;
              delay(1000);
              for (pos = 0; pos <= 360; pos += 1) { // goes from 180 degrees to 0 degrees
                 
                  myservo.write(pos);              // tell servo to go to position in variable 'pos'
               
                  delay(15);                       // waits 15ms for the servo to reach the position
                }
              pos = 0;
              delay(1000);
              Serial.println("found tweet!");
          }
          acc = "";
          break;
        } else {
          acc = "";
        }
      } else {
        acc = acc + c;
      }
    }

  }
  // if we found a new tweet, save the ID for later
  if (id.length() > 0) {
    newID = id;
  }

}
// PRODUCE TRUE AND REPLACE DATE IF THERE IS NO DATE SHOWN
bool isLater(String newDate) {
  acc = acc.substring(28);
  if (currDate == "") {
    setDate(newDate);
    currDate = "found";
    return true;
  }
  String newYear = acc.substring(26, 30);
  String newMonth = acc.substring(4, 7);
  String newDay = acc.substring(8, 10);
  String newTime = acc.substring(11, 19);
  if (laterYear(newYear)) {
    setDate(acc);
    return true;
  } else if (sameYear(newYear)) {
    if (laterMonth(newMonth)) {
      setDate(acc);
      return true;
    } else if (sameMonth(newMonth)) {
      if (laterDay(newDay)) {
        setDate(acc);
        return true;
      } else if (sameDay(newDay) && laterTime(newTime)) {
        setDate(acc);
        return true;
      }
    }
  }
  return false;
}

void setDate(String newDate) {
  currYear = acc.substring(26, 30);
  currMonth = acc.substring(4, 7);
  currDay = acc.substring(8, 10);
  currTime = acc.substring(11, 19);
  currHour = convertTime(currTime, 0, 2);
  currMin = convertTime(currTime, 3, 5);
  currSec = convertTime(currTime, 6, 8);
}

int convertMonth(String month) {
  if (month == "Jan") {
    return 1;
  } else if (month == "Jan") {
    return 2;
  } else if (month == "Feb") {
    return 3;
  } else if (month == "Mar") {
    return 4;
  } else if (month == "Apr") {
    return 5;
  } else if (month == "May") {
    return 6;
  } else if (month == "Jun") {
    return 7;
  } else if (month == "Jul") {
    return 8;
  } else if (month == "Aug") {
    return 9;
  } else if (month == "Sep") {
    return 10;
  } else if (month == "Oct") {
    return 11;
  } else if (month == "Nov") {
    return 12;
  } else if (month == "Dec") {
    return 13;
  }
}

bool laterYear(String newYear) {
  return newYear.toInt() > currYear.toInt();
}
bool sameYear(String newYear) {
  return newYear.toInt() == currYear.toInt();
}
bool laterMonth(String newMonth) {
  return convertMonth(newMonth) > convertMonth(currMonth);
}
bool sameMonth(String newMonth) {
  return convertMonth(newMonth) == convertMonth(currMonth);
}
bool laterDay(String newDay) {
  return newDay.toInt() > currDay.toInt();
}
bool sameDay(String newDay) {
  return newDay.toInt() == currDay.toInt();
}
bool laterTime(String newTime) {
  int newHour = convertTime(newTime, 0, 2);
  int newMin = convertTime(newTime, 3, 5);
  int newSec = convertTime(newTime, 6, 8);
  if (newHour > currHour) {
    return true;
  } if (newHour == currHour) {
    if (newMin > currMin) {
      return true;
    } else if (newMin == currMin) {
      if (newSec > currSec) {
        return true;
      }
    }
  }
  return false;
}

int convertTime(String time, int a, int b) {
  String result = time.substring(a, b);
  return result.toInt();
}


void CheckButton() {                    //WANT TO DELAY F0R delay (12000); BEFORE CHECKING FOR NEW TWEETS

  String checkString = Serial.readString();
  //Serial.println("delay");
  //delay(10000);
  delays++;
  Serial.println(delays);
  if (delays >= 3) {
  //if (checkString == "?"){
    Serial.println("delaying");
    delay (12000); 
    Button();
    Serial.println("Searching...");
    inResponse = false;
    delays = 0;
  }
}

void Button() {

  client.stop();
  printCheck = true;
  countUntilTweet = 0;
  printedCharacterCount = 0;
  delay(100);
  if (client.connectSSL(server, 443))
  {

    // break up string to read easier
    //Merry%20
    String getPrefix = "GET /1.1/search/tweets.json?";
    String getQuery = "q=%40\"Christmas\"";
    String getPostfix = "&lang=en&result_type=recent&count=1 HTTP/1.1";

    // check if we have a newID
    if (newID.length() > 0) {
      client.println(getPrefix + getQuery + "&since_id=" + newID + getPostfix);
    } else {
      client.println(getPrefix + getQuery + getPostfix);
    }

    client.println("Host: api.twitter.com");
    client.println("Authorization: Bearer AAAAAAAAAAAAAAAAAAAAAODMywAAAAAAfvZVgnQgq5UapDrh9ahPe1XrXU0%3DKhcrzTjg1IkyYYETQeuBfCgcAPpsdgSv4uhq9CC5L5f57H2Lw2");
    client.println("Connection: close");
    client.println();
  }
  else
  {
    Serial.println("Not connected to server.");
  }
}
