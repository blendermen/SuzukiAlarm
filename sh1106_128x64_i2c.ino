
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SH1106.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>
#include <MemoryFree.h>;
#include <pgmStrToRAM.h>; // not needed for new way. but good to have for reference.

//alarm trigger
byte triggeredAlarmCount = 0;
unsigned long IntervalForTriggerAlarm = 0;
unsigned long previousMillisForTriggerAlarm = 0;


//temperatura:
// Data wire is plugged into port 2 on the Arduino
const byte oneWireBus = 4;
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
DeviceAddress Wortin   = { 0x28, 0x99, 0xBE, 0x76, 0xE0, 0x01, 0x3C, 0x6B };
// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillisForTemperature = 0;        // will store last time LED was updated
// constants won't change:
long intervalForTemperature;          // interval at which to blink (milliseconds)
float temperatureC;
char temperatureCText[5];
//JOY
#define joyX A0
#define joyY A1


//sterowanie
int xValue, yValue; //z joypada od 0-1024 analogowe
boolean pressedLeft = false, pressedRight = false, pressedUp = false, pressedDown = false; //do sterowania joyem (zeby nie powtarzac 100 razy lewo,lewo,lewo) - tylko pomocnicze. Oznaczaja ze cos zostalo nacisniete ale jeszcze nie puszczone - bo dopiero jak na przyklad skręce w lewo i puszcze joystinc do pozycji zero to uznajemy na wybrór/ruch.
boolean up, down, left, right = false; //stan po nacisnieciu i wroceniu do pozycji 0 na joy - czyli faktyczne warunki ktore sprawdzamy co zostalo nacisniete
boolean select = 1, lastStatusOfSelect = 1; //klik joya
byte pozx, pozy = 0;

//menu
byte displayItem = 1; //zaznaczony element
byte displayPage = 2; //dana strona glowna
byte PageNumber = 1; //numer wewnetrznej strony o autorze
char *displayItemText = "Alarm";
//char  *myStrings[]  = {"Alarm1", "Alarm2", "Alarm3"};
//
//  const char string_0[] PROGMEM = "String 0"; // "String 0" etc are strings to store - change to suit.
//const char string_1[] PROGMEM = "String 1";
//const char string_2[] PROGMEM = "String 2";
//const char string_3[] PROGMEM = "String 3";
//const char string_4[] PROGMEM = "String 4";
//const char string_5[] PROGMEM = "String 5";
//char *optionsMenu2[]  = {string_1};

char *optionsMenu[]  = {"Alarm", "GPS", "Temperatura", "Czujnik ruchu", "O Autorze",};
short iter = 0, pozycja_startowa = 0;


//opcje
boolean alarmEnabled;
boolean gps = true;


//detekor ruchu
int sensitivity;

char howManyShocksToTriggerAlarm = 4;
char howManyShocksAlreadyTrigger = 0;

unsigned short shakeSensorAnalog = 0;
char shakeSensorAnalogText[4]; //just for display purposes 23 = 0023
byte howManyShockHasBeenTriggered = 0;  //increases any time when vibration is greater than sensitivity setting
byte howManyShocksToTriggerAlarmx;   //value to set how many triggers are needed to perform an alarm.
//byte sensitivity;             //vibration sensitivity. It increases an howManyShockHasBeenTriggered only if shakeSensorAnalog readings (0-1023) >= sensitivity(0-1023)
bool alarmTriggered = false; //global
unsigned long IntervalForVibrationDetector = 5000; //seconds global
//unsigned long currentMillisForVibrationDetector = millis(); loop
unsigned long previousMillisForVibrationDetector = 0;  //global
unsigned long previousMillisForVibrationDetector2 = 0;  //global
byte testtime = 0;


Adafruit_SH1106 display(-1);


//'Suzuki_Logo_3', 128x64px
const unsigned char epd_bitmap_Suzuki_Logo_3 [] PROGMEM = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x01, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x03, 0x80, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x01, 0xe0, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x07, 0x80, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xf0, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x7c, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x3f, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x0f, 0x80, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x01, 0xe0, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x07, 0x80, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0xe0, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x78, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x18, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xfc, 0x1c, 0xf2, 0x00, 0x9e, 0x67, 0x19, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xf1, 0x8c, 0xf2, 0x00, 0x9e, 0x66, 0x39, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xf1, 0xfc, 0xf3, 0xe1, 0x9e, 0x64, 0x79, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xf0, 0x1c, 0xf3, 0xc3, 0x9e, 0x60, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x04, 0xf3, 0x87, 0x9e, 0x60, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xc4, 0xf3, 0x0f, 0x8e, 0x64, 0x79, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xf0, 0x0c, 0x02, 0x00, 0x80, 0x66, 0x39, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xfc, 0x3e, 0x06, 0x00, 0xe0, 0xe7, 0x19, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

void setup()   {
  Serial.begin(9600);
  // Serial.print("start seriala");

  temperatureC = 0;

  //Mapa pamieci EEPROM:
  //  address         zmienna
  //  0-1             sensitivity
  //  2               howManyShocksToTriggerAlarmx
  //  3               alarmEnabled


  //zapis defaultowych danych do pamieci. jezeli juz jakas wartosc tam istnieje to nie robie nic.

  //dla sensitivity(int = 2 bajty, a więc adres= 0-1.
  writeDefaultIntValuesIntoEEPROM(0, 100); //SAVE  zapis defaultowej wartosci w komorkach od 0 do 1. Jezeli  pod tym adresem jest juz jakas wartosc zapisana(!255), to nie robie nic.
  sensitivity = readIntFromEEPROM(0); //READ  funkcja wie, że ma odczytac adresy od 0-1 (bo int = 2 bajty, a więc adres= 0-1).

  //dla howManyShocksToTriggerAlarmx (1 bajt, a wiec jeden adres)
  writeDefaultByteValueIntoEEPROM(2, (byte)5); //SAVE   w adresie numer 2, zapisuje defaulotwy numer 5 - jest to wersja dla jednego bajta.  Jezeli  pod tym adresem jest juz jakas wartosc zapisana(!255), to nie robie nic.
  howManyShocksToTriggerAlarmx = readByteFromEEPROM(2); // READ

  //dla alarmEnabled
  writeDefaultByteValueIntoEEPROM(3, (byte)0);
  alarmEnabled = readByteFromEEPROM(3); // READ


  Serial.println(readIntFromEEPROM(0));
  Serial.println(howManyShocksToTriggerAlarmx);
  Serial.println(alarmEnabled);
  Serial.println("----");



  //joy
  pinMode(2, INPUT_PULLUP);  //do przycisku joypada
  pinMode(5, INPUT);  //do czujnika ruchu
  pinMode(17, INPUT);  //do czujnika ruchu

  //syrena
  pinMode(3, OUTPUT); //do sterowania tranzystorem syreny

  //ekran
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done

  //
  //  for(int x=0;x<100;x++){
  //     digitalWrite(3, HIGH); // sets the digital pin 13 on
  //  delay(10);            // waits for a second
  //  digitalWrite(3, LOW);  // sets the digital pin 13 off
  //  delay(10);
  //    }

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.

  display.clearDisplay();

  //display.drawBitmap(0, 0,  epd_bitmap_Suzuki_Logo_3, 128, 64, WHITE);
  display.display();
  //// Display Text
  //display.clearDisplay();
  //display.setTextSize(2);
  //display.setTextColor(WHITE);
  //display.setCursor(0,0);
  //display.println("PATRYSKA");
  //display.display();

  delay(100);
  //temperatura
  // Start up the library

  intervalForTemperature = 1000;
  sensors.setWaitForConversion(false);
  sensors.setResolution(Wortin, 9);  //This should change the resolution to 9-bit
  sensors.begin();

  delay(100);
  Serial.println(freeMemory());










}


void loop() {
  //Serial.println(freeMemory());
  unsigned long currentMillis = millis(); //czas odkad program wystartowal
  //unsigned long currentMillisForVibrationDetector = millis();
  //Serial.println(currentMillis);
  xValue = analogRead(joyX); //analogowe dane z joysticka 0-1023
  yValue = analogRead(joyY);
  //Serial.println(String("x: ") + xValue);
  //Serial.println(String("y: ") + yValue);
  //Serial.println("--");

  // int select2 = digitalRead(5);
  //Serial.println(select2);
  //pretend to multiply 1,1,1,0,0,0,0 - push button
  if (digitalRead(2) != lastStatusOfSelect) {
    select = digitalRead(2);
    lastStatusOfSelect = select;
  }

  //prawo i lewo - zabezpieczenie by nie powielac lewo,lewo,lewo,lewo przez caly czas trzymania joya. Srodek joypada do okolo 500, lewo 0, prawo 1200cos
  if (xValue < 200 || pressedLeft )
  {
    if (~pressedLeft) pressedLeft = true;
    if (pressedLeft && xValue > 200) {
      pressedLeft = false;
      left = true;
      Serial.println("lewo");
      pozx--;
      // Serial.print("\t");
    }
  } else if (xValue > 800 || pressedRight)
  {
    if (~pressedRight) pressedRight = true;
    if (pressedRight && xValue < 800) {
      pressedRight = false;
      right = true;
      Serial.println("prawo");
      pozx++;
      Serial.print("\t");
    }
  }

  //gora  i dol - zabezpieczenie by nie powielac lewo,lewo,lewo,lewo przez caly czas trzymania joya
  if (yValue > 800 || pressedDown )
  {
    if (~pressedDown) pressedDown = true;
    if (pressedDown && yValue < 800) {
      pressedDown = false;
      down = true;
      Serial.println("dol");
      pozy++;
      //Serial.print("\t");
    }
  } else if (yValue < 200 || pressedUp)
  {
    if (~pressedUp) pressedUp = true;
    if (pressedUp && yValue > 200) {
      pressedUp = false;
      up = true;
      Serial.println("gora");
      pozy--;
      //Serial.print("\t");
    }
  }


  // Display Text
  //display.clearDisplay();
  //display.setTextSize(2);
  //display.setTextColor(WHITE);
  //display.setCursor(pozx,pozy);
  //display.println("DUPA");
  //display.display();
  //delay(100);

  //display.clearDisplay();
  //display.setTextSize(2);
  //display.setTextColor(WHITE);
  //display.setCursor(map(xValue, 0, 1024, 0, 128),map(yValue, 0, 1024, 0, 64));
  //display.println("DUPA");
  //display.display();
  //delay(100);

  VibrationDetector(currentMillis);
  getTemperature(currentMillis);
  triggerAlarm(currentMillis);
  displayMenu(currentMillis);
  // Serial.println(temperatureC);

  //delay(20);

}

void VibrationDetector(unsigned long currentMillis)
{
  shakeSensorAnalog = analogRead(17); //czujnik ruchu podlaczony do pinu A3 0-1023


  //jezeli wykryjemy wstrzas większy niż ustawione sensitivity to zaliczamy howManyShockHasBeenTriggered
  if (shakeSensorAnalog >= sensitivity )
  {
    howManyShockHasBeenTriggered++;
  }

  //jezeli liczba zaliczonych wstrzasow = zadeklarowanej liczby wstrzasow by uruchomic alarm to uruchamiamy alarm.
  if (howManyShockHasBeenTriggered == howManyShocksToTriggerAlarmx )
  {
    alarmTriggered = true;
  }

  //zerujemy liczbe zarejestrowanych triggerow ruchu po czasie 5s. Dzieki temu operujemy tylko na czasie 5s, czyli w ciagu 5 sekund sumujemy triggery ruchu i po tym czasie wszystko zerujemy i od nowa czekamy na nowe triggery
  //az nie spelni sie warunek i nie uruchomimy alarmu
  if (currentMillis - previousMillisForVibrationDetector >= IntervalForVibrationDetector)
  {
    previousMillisForVibrationDetector = currentMillis;
    howManyShockHasBeenTriggered = 0;
    testtime = 0;
    //alarmTriggered = false; //teraz alarmTriggered jest kasowany w funkcji triggerAlarm, zaraz po syrenie alamowej
  }

  //test
  if (currentMillis - previousMillisForVibrationDetector2 >= 1000)
  {
    previousMillisForVibrationDetector2 = currentMillis;
    testtime++;
  }


}


void getTemperature(unsigned long currentMillis)
{
  if (currentMillis - previousMillisForTemperature >= intervalForTemperature)
  {
    previousMillisForTemperature = currentMillis;
    sensors.requestTemperaturesByAddress(Wortin); //lub sensors.requestTemperatures();
    if (sensors.getTempCByIndex(0) != DEVICE_DISCONNECTED_C)
    {
      temperatureC = sensors.getTempCByIndex(0);
      //Serial.println(temperatureC);           -loop
    } else
    {
      temperatureC = 0;
      //Serial.println(temperatureC);
    }
  }
}

//za pierwszym triggerem ruchu uruchamiam alarm na 2s,
//za drugim triggerem ruchu uruchamiam alarm na 5s,
//za trzecim triggerem ruchu uruchamiam alarm na 8s
void triggerAlarm(unsigned long currentMillis)
{
  if (alarmEnabled)
  {
    if (!alarmTriggered) {
      previousMillisForTriggerAlarm = currentMillis;
    }
    if (alarmTriggered)
    {

      switch (triggeredAlarmCount)
      {
        case 0:
          IntervalForTriggerAlarm = 2000;
          break;
        case 1:
          IntervalForTriggerAlarm = 5000;
          break;
        case 2:
          IntervalForTriggerAlarm = 8000;
          break;
        default:
          triggeredAlarmCount = 0;
          break;

      } //end switch

      Serial.println("ALLARM!");
      if (currentMillis - previousMillisForTriggerAlarm >= IntervalForTriggerAlarm)  //po danym czasie IntervalForTriggerAlarm=2,5,8s zarrzymuje alarm i eskaluje czas alarmu wyzej 2->5->8 i moze dalej?
      {
        previousMillisForTriggerAlarm = currentMillis;
        alarmTriggered = false;
        Serial.println("ALLARM STOP!");
        Serial.println(triggeredAlarmCount);
        triggeredAlarmCount++;
      }

    } //alarmTriggered
  } //alarmEnabled
} //triggerAlarm()


void displayMenu(unsigned long currentMillis)
{

  //OPCJE - mechanizm wyboru odpowiedniej opcji, przechodzi do poczatku listy jak jestesmy juz na koncu,
  //     if (displayPage == 2 && up)
  //       {
  //        up = false;
  //        displayItem++;
  //          if (displayItem == 4)
  //          {
  //            displayItem = 1;
  //          }
  //        }else if(displayPage == 2 && down)
  //        {
  //          down = false;
  //          displayItem--;
  //          if (displayItem == 0)
  //          {
  //            displayItem = 3;
  //          }
  //        }


  //Opcje->Temperatura - wejscie do ekranu z temperatura
  if (displayPage == 2 && displayItemText == "Temperatura") {
    if (select == LOW) {
      display.clearDisplay();
      displayItemText = "Temperatura" ;
      displayPage = 6;
    }
  }
  if (displayPage == 6 && left) {
    left = false;
    displayPage = 2;
    display.clearDisplay();
    displayItemText = "Temperatura";
  }

  //OPCJE->O Autorze - wejscie do ekranu  O Autorze
  if (displayPage == 2 && displayItemText == "O Autorze") {
    Serial.println("ok");
    if (select == LOW) {
      Serial.println("LOW");
      delay(400);
      display.clearDisplay();
      displayItemText = "O Autorze" ;
      displayPage = 3;
    }
  }
  //Opcje->O autorze->numer strony - zmiana strony opisu
  if (displayPage == 3) { //wejscie do opcji o Autorze
    if (right) {
      PageNumber++;
      right = false;
    }
    if (left) {
      PageNumber--;
      left = false;
    }
    if (PageNumber >= 4) {
      PageNumber = 1;
    }
    if (PageNumber <= 0) {
      PageNumber = 1;
      displayPage = 2;
      display.clearDisplay();
      displayItemText = "O Autorze";
    }
  }




  //Opcje->Alarm - wejscie do ekranu Alarm
  if (displayPage == 2 && displayItemText == "Alarm") {
    if (select == LOW) {
      display.clearDisplay();
      displayItem = 1;
      displayPage = 4;
    }
  }
  //Opcje->Alarm->alarm on/off
  if (displayPage == 4 && displayItem == 1) {
    if (right) {
      alarmEnabled = false;
      right = false;
      digitalWrite(3, LOW);
    }
    if (left) {
      alarmEnabled = true;
      left = false;
      digitalWrite(3, HIGH);
    }
  }
  //Opcje->Alarm->wstecz
  if (displayPage == 4 && displayItem == 3) { //wejscie do opcji o Autorze
    if (select == LOW) {
      writeUniqueByteValueIntoEEPROM(3, (byte)alarmEnabled);
      delay(100);
      displayItemText = "Alarm";
      displayPage = 2;
    }
  }




  //Opcje->czujnik ruchu wejscie do ekranu
  if (displayPage == 2 && displayItemText == "Czujnik ruchu") {
    if (select == LOW) {
      delay(500);
      display.clearDisplay();
      displayItemText = "Czujnik ruchu" ;
      displayPage = 5;
      displayItem = 1;
      Serial.println("FF");
      select = HIGH;
    }
  }
  //Opcje->czujnik - ustawienie sensitivity
  if (displayItem == 1 && displayPage == 5)
  {
    if (pressedLeft && sensitivity >= 0) {
      sensitivity--;
      delay(50);
    }
    if ( pressedRight && sensitivity <= 1025) {
      sensitivity++;
      delay(50);
    }
    if (sensitivity == 1025) {
      sensitivity = 1;
    }
    else if (sensitivity == 0) {
      sensitivity = 1024;
    }
    if (select == LOW  && sensitivity <= 999) {
      sensitivity += 25;
      delay(50);
    }
  }
  //opcje->czujnik ustawienie howManyShocksToTriggerAlarmx
  if (displayPage == 5 && displayItem == 2) { //wejscie do opcji o Autorze
    if (pressedLeft && howManyShocksToTriggerAlarmx >= 0) {
      howManyShocksToTriggerAlarmx--;
      delay(50);
    }
    if ( pressedRight && howManyShocksToTriggerAlarmx <= 21) {
      howManyShocksToTriggerAlarmx++;
      delay(50);
    }
    if (howManyShocksToTriggerAlarmx == 21) {
      howManyShocksToTriggerAlarmx = 1;
    }
    else if (howManyShocksToTriggerAlarmx == 0) {
      howManyShocksToTriggerAlarmx = 20;
    }
  }
  //Opcje->czujnik ruchu->wstecz
  if (displayPage == 5 && displayItem == 3) { //wejscie do opcji o Autorze
    if (select == LOW)
    {
      writeUniqueIntIntoEEPROM(0, sensitivity);
      writeUniqueByteValueIntoEEPROM(2, (byte)howManyShocksToTriggerAlarmx);
      delay(100);
      displayPage = 2;
      Serial.println("wst");
    }
  }


  //OPCJE - samo wyswietlania dostepnych opcji do wybotu
  if (displayPage == 2)
  {

    screenHeader(49, " OPCJE ");



    int number_of_elements = sizeof(optionsMenu) / sizeof(char *);
    selectSwitcher2(number_of_elements, optionsMenu);
    for (int x = 0; x < number_of_elements; x++) {


      //
      //         Serial.println("jesterm ");
      //                Serial.print(displayItemText);
      //                  Serial.print("  ");
      //                        Serial.println(myStrings[x]);
      //                            Serial.println(sizeof(myStrings) / sizeof(char *));
      display.setCursor(0, (15 + x * 10));
      if (displayItemText == optionsMenu[x])
      {

        display.setTextColor(BLACK, WHITE);
      }
      else
      {
        display.setTextColor(WHITE, BLACK);
      }

      display.print(optionsMenu[x]);

    }
    //rysuj wszystko na ekran
    display.display();
  }
  else if (displayPage == 3)  //Strona OPCJE -> o Autorze
  {
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor(BLACK, WHITE);
    display.setCursor(25, 0);
    display.print("O Autorze " );
    display.print(PageNumber);
    display.print("/3");
    display.drawFastHLine(0, 10, 124, WHITE);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 15);
    switch (PageNumber) {
      case 1:

        display.println(F("Autor projektu"));
        display.println(F("Marcin Mazur"));
        display.println(F("Bialaczow 2021"));
        display.println(F("\n\n<-Wstecz      Dalej->"));
        break;
      case 2:
        ;
        display.println(F("Alarm/GPS dla"));
        display.println(F("Suzuki GZ Marauder"));
        display.println(F("\n\n             Dalej->"));

        break;
      case 3:

        display.println(F("Rozpoczecie projektu"));
        display.println(F("06/07/2021"));
        display.println(F("\n\n             Dalej->"));
        break;
      default:
        // statements
        break;
    }
    display.display();
    Serial.println(freeMemory());

  }
  else if (displayPage == 4) { //Opcje->Alarm
    selectSwitcher(3);
    screenHeader(49, " Alarm ");


    if (displayItem == 1)
    {
      display.setTextColor(BLACK, WHITE);
    }
    else
    {
      display.setTextColor(WHITE, BLACK);
    }

    display.print(F("Alarm"));
    display.setTextColor(WHITE, BLACK); display.print("   ");

    if (alarmEnabled) {
      display.setTextColor(BLACK, WHITE);
      display.print(F(" ON"));
      display.setTextColor(WHITE, BLACK);
      display.print(F(" OFF"));
      //display.SH1106_command(SH1106_DISPLAYON);
    } else {
      display.setTextColor(WHITE, BLACK);
      display.print(F(" ON"));
      display.setTextColor(BLACK, WHITE);
      display.print(F(" OFF"));
      //display.SH1106_command(SH1106_DISPLAYOFF);
    }

    if (displayItem == 2)
    {
      display.setTextColor(BLACK, WHITE);
    }
    else
    {
      display.setTextColor(WHITE, BLACK);
    }
    display.setCursor(0, 25);
    display.print(F("Test"));

    if (displayItem == 3)
    {
      display.setTextColor(BLACK, WHITE);
    }
    else
    {
      display.setTextColor(WHITE, BLACK);
    }
    display.setCursor(0, 35);
    display.print(">Wstecz");

    display.display();
  }
  //Opcje->Czujnik ruchu
  else if (displayPage == 5) {
    selectSwitcher(3);
    // Serial.println(currentMillis);
    screenHeader(10, " Czujnik ruchu   ");

    //     display.print(digitalRead(5));
    //     display.setCursor(0, 25);
    //     unsigned short shakeSensorAnalog = analogRead(17);

    sprintf(shakeSensorAnalogText, "%04d", shakeSensorAnalog); //23 = 0023
    display.print(F("A: ")); display.print(shakeSensorAnalogText); display.print(F(" hmshbt: ")); display.print(howManyShockHasBeenTriggered);
    //
    if (displayItem == 1)
    {
      display.setTextColor(BLACK, WHITE);
    }
    else
    {
      display.setTextColor(WHITE, BLACK);
    }
    //
    display.setCursor(0, 25);
    display.print(F("Sensitivity: "));
    display.print(sensitivity);
    //Serial.println("sensitivity: ");Serial.print(sensitivity);

    //     //todo + przeniesc wykrywanie i obsluge alarmu po wstrzasie do zewnetrzej funkcji. Alarm ma dzialac bez ekranu nawet.
    //     //zapis sensitivity do pamieci/ podzial sensitivity na 4 czesci
    //   //  if (shakeSensorAnalog >= sensitivity ){howManyShocksAlreadyTrigger++;  display.setCursor(0, 45); display.print("TRIGGERED");display.print(howManyShocksAlreadyTrigger);}
    ////howManyShocksToTriggerAlarmx
    ////howManyShocksAlreadyTrigger

    if (displayItem == 2)
    {
      display.setTextColor(BLACK, WHITE);
    }
    else
    {
      display.setTextColor(WHITE, BLACK);
    }

    display.setCursor(0, 35);
    display.print(F("hmstta: ")); display.print(howManyShocksToTriggerAlarmx);
    // Serial.println("howManyShocksToTriggerAlarmx: ");Serial.print(howManyShocksToTriggerAlarmx);


    display.setTextColor(WHITE, BLACK);

    display.setCursor(0, 45);
    display.print(F("alarmTriggered: ")); display.print(alarmTriggered);
    //Serial.println("alarmTriggered: ");Serial.print(alarmTriggered);








    if (displayItem == 3)
    {
      display.setTextColor(BLACK, WHITE);
    }
    else
    {
      display.setTextColor(WHITE, BLACK);
    }
    //
    display.setCursor(0, 55);
    display.print(F("<-Wstecz "));
    //
    display.display();
  }
  //Opcje->Temperatura
  else if (displayPage == 6) {
    screenHeader(20, " Temperatura ");
    //      //if (currentMillis - previousMillis >= interval) {
    //      //unsigned long currentMilliss = millis();
    //          Serial.println(currentMilliss);
    //            Serial.println("----");
    //        //   Serial.println(previousMillis);
    //         //   Serial.println(interval);
    //          // save the last time you blinked the LED
    //         // previousMillis = currentMillis;
    //          sensors.requestTemperaturesByAddress(Wortin);
    //          //Odczyt w&nbsp;stopniach celsjusza
    //
    //          // Check if reading was successful
    //            if(sensors.getTempCByIndex(0) != DEVICE_DISCONNECTED_C)
    //            {
    //
    //                    temperatureC = sensors.getTempCByIndex(0);
    //                     Serial.println(temperatureC);
    //
    //           //  }else{
    //
    //                    //   temperatureC = 0;
    //              }
    //
    //         // }
    display.setTextSize(3);

    /* 4 is mininum width, 2 is precision; float value is copied onto str_temp*/
    char str_temp[5];
    dtostrf(temperatureC, 4, 2, str_temp);


    display.print(str_temp);
    display.print(" C");

    display.setCursor(0, 55);
    display.setTextSize(1);
    display.println(F("<-Wstecz"));
    display.display();
    delay(20);
  }




}//end sisplaymenu()

void screenHeader(int x, char *header)
{
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(BLACK, WHITE);
  display.setCursor(x, 0);
  display.print(header);
  display.drawFastHLine(0, 10, 128, WHITE);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0, 15);
}


//OPCJE - mechanizm wyboru odpowiedniej opcji, przechodzi do poczatku listy jak jestesmy juz na koncu, int wersion
void selectSwitcher(int amountOfItem) {
  if (up) //jezeli joyem do gory
  {
    up = false; //usuwamy od razu stan
    displayItem++; //wystieltamy kolejny element
    if (displayItem == (amountOfItem + 1)) //jezelu dojdziemy do konca listy
    {
      displayItem = 1; //wracamy na poczatek
    }
  } else if (down)
  {
    down = false;
    displayItem--;
    if (displayItem == 0)
    {
      displayItem = amountOfItem;
    }
  }
}



//OPCJE - mechanizm wyboru odpowiedniej opcji, przechodzi do poczatku listy jak jestesmy juz na koncu, string version
void selectSwitcher2(int amountOfItem,   char *myStrings[]) {
  if (up) //jezeli joyem do gory
  {

    up = false; //usuwamy od razu stan
    iter++;

    displayItemText = myStrings[iter]; //wystieltamy kolejny element
    //Serial.println(displayItemText);
    if (iter == (amountOfItem)) //jezelu dojdziemy do konca listy
    {
      iter = 0;
      displayItemText = myStrings[0]; //wracamy na poczatek
      //Serial.println(displayItemText);
    }
  } else if (down)
  {

    down = false;
    iter--;

    displayItemText = myStrings[iter];
    Serial.println(iter);
    if (iter == -1)
    {
      iter = amountOfItem - 1;
      displayItemText = myStrings[amountOfItem - 1];
      //Serial.println(displayItemText);
    }
  }

}
//FUNKCJE ZAPISU
//zapis do pamieci wartosci int(2 bajty) - bo na eepromie w jednej komorce mozna zapisac max 1B      INT save
void writeIntIntoEEPROM(int address, int number)
{
  //01000111 11100100.
  byte byte1 = number >> 8;   //01000111
  byte byte2 = number & 0xFF; //11100100
  EEPROM.write(address, byte1);
  EEPROM.write(address + 1, byte2);
}

//odczyt danych z eeprom - dla int        INT read
int readIntFromEEPROM(int address)
{
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address + 1);
  return (byte1 << 8) + byte2;
}

//odczyt danych z eeprom - dla Bajta      BAJT read
byte readByteFromEEPROM(int address)
{
  byte byte1 = EEPROM.read(address);
  return byte1;
}

//zapis defaultowych wartosci przy pierwszym uruchomieniu arduino. jezeli już jakies wartosci są zapisane to nie robie oczywiscie nic. Wersja dla Int (2 bajty)       INT default write
void writeDefaultIntValuesIntoEEPROM(int address, int number)
{
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address + 1);
  //jezeli komorki pamieci sa puste to zapisuje tam defaultowe dane
  if (byte1 == 255 && byte2 == 255)
    //Serial.println("zapisuje defaultowe wartosci dla int");
  {
    writeIntIntoEEPROM(address, number);
  }
}

//zapis defaultowych wartosci przy pierwszym uruchomieniu arduino. jezeli już jakies wartosci są zapisane to nie robie oczywiscie nic. Wersja dla Bajta       BAJT default write
void writeDefaultByteValueIntoEEPROM(int address, byte number)
{
  byte byte1 = EEPROM.read(address);
  //jezeli komorka pamieci jest pusta, to zapisuje tam defaultowe dane
  if (byte1 == 255)
    //Serial.println("zapisuje defaultowe wartosci dla bajta");
  {
    EEPROM.write(address, number);
  }
}

//wrapper na INT save - przed zapisem sprawdza czy istnieje juz taka sama wartosc zapisana - jezeli istnieje to nie nadpisuje niepotrzenie pamieci eeprom/   INT save wrapper checking duplication
void writeUniqueIntIntoEEPROM(int address, int number) {
  int value = readIntFromEEPROM(address);
  if (value != number)
  {
    Serial.println("zapisuje unikalnego inta");
    writeIntIntoEEPROM(address, number);
  }
}

//wrapper na BAJT save - przed zapisem sprawdza czy istnieje juz taka sama wartosc zapisana - jezeli istnieje to nie nadpisuje niepotrzenie pamieci eeprom/   BAJT save wrapper checking duplication
void writeUniqueByteValueIntoEEPROM(int address, byte number) {
  byte value = readByteFromEEPROM(address);
  if (value != number)
  {
    Serial.println("zapisuje unikalnego bajta");
    EEPROM.write(address, number);
  }
}
//FUNKCJE ZAPISU KONIEC
