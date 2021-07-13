
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SH1106.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>
#include <MemoryFree.h>;
#include <pgmStrToRAM.h>; // not needed for new way. but good to have for reference.

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
unsigned long previousMillis = 0;        // will store last time LED was updated
// constants won't change:
const long interval PROGMEM = 1000;           // interval at which to blink (milliseconds)
float temperatureC = 0;

//JOY
#define joyX A0
#define joyY A1


//sterowanie
int xValue, yValue; //z joypada od 0-1024 analogowe
boolean pressedLeft=false, pressedRight=false, pressedUp=false, pressedDown=false; //do sterowania joyem (zeby nie powtarzac 100 razy lewo,lewo,lewo) - tylko pomocnicze. Oznaczaja ze cos zostalo nacisniete ale jeszcze nie puszczone - bo dopiero jak na przyklad skręce w lewo i puszcze joystinc do pozycji zero to uznajemy na wybrór/ruch.
boolean up,down,left,right = false; //stan po nacisnieciu i wroceniu do pozycji 0 na joy - czyli faktyczne warunki ktore sprawdzamy co zostalo nacisniete
boolean select=1, lastStatusOfSelect=1; //klik joya
byte pozx, pozy =0;

//menu
byte displayItem = 1; //zaznaczony element
byte displayPage = 2; //dana strona glowna
byte PageNumber = 1; //numer wewnetrznej strony o autorze
char *displayItemText = "Alarm";
  char  *myStrings[]  = {"Alarm1", "Alarm2", "Alarm3"};

  const char string_0[] PROGMEM = "String 0"; // "String 0" etc are strings to store - change to suit.
const char string_1[] PROGMEM = "String 1";
const char string_2[] PROGMEM = "String 2";
const char string_3[] PROGMEM = "String 3";
const char string_4[] PROGMEM = "String 4";
const char string_5[] PROGMEM = "String 5";
char *optionsMenu2[]  = {string_1};

  char *optionsMenu[]  = {"Alarm", "GPS", "Temperatura", "Czujnik ruchu", "O Autorze",};
short iter=0, pozycja_startowa=0;
//opcje
boolean alarm = false;
boolean gps = true;


//detekor ruchu
unsigned short sensitivity = 35;
char howManyShocksToTriggerAlarm = 4;
char howManyShocksAlreadyTrigger = 0;

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
  Serial.print("start seriala");

 
  //joy
  pinMode(2, INPUT_PULLUP);  //do przycisku joypada
  pinMode(5, INPUT);  //do czujnika ruchu
  pinMode(17, INPUT);  //do czujnika ruchu
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
  sensors.begin();
delay(100);
Serial.println(freeMemory());










}


void loop() {

  unsigned long currentMillis = millis(); //czas odkad program wystartowal
         

  xValue = analogRead(joyX); //analogowe dane z joysticka 0-1023
  yValue = analogRead(joyY);
  //Serial.println(String("x: ") + xValue);
  //Serial.println(String("y: ") + yValue);
  //Serial.println("--");
  
// int select2 = digitalRead(5);
 //Serial.println(select2);
  //pretend to multiply 1,1,1,0,0,0,0 - push button   
  if (digitalRead(2) != lastStatusOfSelect){select=digitalRead(2);lastStatusOfSelect=select;}
 
  //prawo i lewo - zabezpieczenie by nie powielac lewo,lewo,lewo,lewo przez caly czas trzymania joya. Srodek joypada do okolo 500, lewo 0, prawo 1200cos
 if (xValue <200 || pressedLeft )
 {
   if (~pressedLeft) pressedLeft=true;
   if (pressedLeft && xValue > 200){
        pressedLeft=false; 
        left=true;
        Serial.println("lewo");
        pozx--; 
       // Serial.print("\t");
        }
 }else if(xValue > 800 || pressedRight)
 {
    if (~pressedRight) pressedRight=true;
    if (pressedRight && xValue < 800){
         pressedRight=false; 
         right=true;
         Serial.println("prawo");
         pozx++; 
         Serial.print("\t");}
 }

  //gora  i dol - zabezpieczenie by nie powielac lewo,lewo,lewo,lewo przez caly czas trzymania joya
 if (yValue > 800 || pressedDown )
 {
   if (~pressedDown) pressedDown=true;
   if (pressedDown && yValue < 800){
        pressedDown=false; 
        down=true;
        Serial.println("dol");
        pozy++; 
        //Serial.print("\t");
        }
 }else if(yValue < 200 || pressedUp)
 {
    if (~pressedUp) pressedUp=true;
    if (pressedUp && yValue > 200){
          pressedUp=false; 
          up=true;
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
 
 displayMenu(currentMillis);
//delay(20);

}

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
         if(displayPage == 2 && displayItemText == "Temperatura"){            
             if(select == LOW){ display.clearDisplay(); displayItemText="Temperatura" ; displayPage = 6;}   
         }  
         if(displayPage == 6 && left) { left=false; displayPage = 2; display.clearDisplay(); displayItemText = "Temperatura";}
   
           //OPCJE->O Autorze - wejscie do ekranu  O Autorze
         if(displayPage == 2 && displayItemText == "O Autorze"){ 
             Serial.println("ok");
             if(select == LOW){ Serial.println("LOW");  delay(400); display.clearDisplay(); displayItemText="O Autorze" ; displayPage = 3;}     
         }
         //Opcje->O autorze->numer strony - zmiana strony opisu
          if(displayPage == 3){ //wejscie do opcji o Autorze
             if(right){ PageNumber++; right=false;}   
             if(left){ PageNumber--;left=false;}
             if(PageNumber>=4){PageNumber=1;}  
             if(PageNumber<=0){PageNumber=1; displayPage = 2; display.clearDisplay(); displayItemText = "O Autorze";}    
         }




         //Opcje->Alarm - wejscie do ekranu Alarm
          if(displayPage == 2 && displayItemText == "Alarm"){ 
             if(select == LOW){ display.clearDisplay();displayItem=1;displayPage=4;}     
         }
         //Opcje->Alarm->alarm on/off
          if(displayPage == 4 && displayItem == 1){ 
             if(right){ alarm=false; right=false; digitalWrite(3, LOW);}   
             if(left){ alarm=true; left=false;digitalWrite(3, HIGH);}
          }
           //Opcje->Alarm->wstecz
          if(displayPage == 4 && displayItem == 3){ //wejscie do opcji o Autorze
             if(select == LOW){delay(100);displayItemText = "Alarm";displayPage=2;}     
          }




      //Opcje->czujnik ruchu wejscie do ekranu 
         if(displayPage == 2 && displayItemText == "Czujnik ruchu"){            
             if(select == LOW){ delay(500); display.clearDisplay(); displayItemText="Czujnik ruchu" ; displayPage = 5; displayItem = 1;}   
         }  
         if(displayItem == 1 && displayPage == 5)
         {
         if(pressedLeft && sensitivity>=0) { sensitivity--; delay(50); }
         if( pressedRight && sensitivity<=1025) { sensitivity++; delay(50); }
         if(sensitivity == 1025){sensitivity=0;}
         else if(sensitivity == 0){sensitivity=1024;}
         if(select == LOW  && sensitivity<=999) { sensitivity+=25; delay(500); }
         }
         //Opcje->czujnik ruchu->wstecz
          if(displayPage == 5 && displayItem == 2){ //wejscie do opcji o Autorze
             if(select == LOW){delay(100);displayPage=2;}     
          }
          
        
    //OPCJE - samo wyswietlania dostepnych opcji do wybotu    
    if (displayPage == 2) 
    {    

     screenHeader(49," OPCJE ");
      

      
    int number_of_elements=sizeof(optionsMenu) / sizeof(char *);
selectSwitcher2(number_of_elements,optionsMenu);
for (int x = 0; x<number_of_elements;x++){
   
  
// 
//         Serial.println("jesterm ");
//                Serial.print(displayItemText);
//                  Serial.print("  ");
//                        Serial.println(myStrings[x]);
//                            Serial.println(sizeof(myStrings) / sizeof(char *));
        display.setCursor(0, (15+x*10));
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
        display.drawFastHLine(0,10,124,WHITE);
        display.setTextColor(WHITE, BLACK);
        display.setCursor(0, 15);
        switch (PageNumber) {
          case 1:
           
          display.println(F("Autor projektu"));
          display.println(F("Marcin Mazur"));
          display.println(F("Bialaczow 2021"));
          break;
          case 2:
         ;
         display.println(F("Alarm/GPS dla"));
         display.println(F("Suzuki GZ Marauder"));

          break;
          case 3:
          
         display.println(F("Rozpoczecie projektu"));
         display.println(F("Rozpoczecie projektu"));
          break;
          default:
          // statements
           break;
          }
        display.display();
         Serial.println(freeMemory());
        
  }
else if (displayPage == 4){ //Opcje->Alarm
         selectSwitcher(3);
         screenHeader(49," Alarm ");
        
      
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
         
        if(alarm){
          display.setTextColor(BLACK, WHITE); 
          display.print(F(" ON"));
          display.setTextColor(WHITE, BLACK); 
          display.print(F(" OFF"));
           //display.SH1106_command(SH1106_DISPLAYON);
         }else{
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
else if (displayPage == 5){ 
//   selectSwitcher(2);
//     screenHeader(10," Czujnik ruchu   ");
//     display.print(digitalRead(5));
//     display.setCursor(0, 25);
//     unsigned short shakeSensorAnalog = analogRead(17);
//     display.print(shakeSensorAnalog);
//
//      if (displayItem == 1) 
//          { 
//            display.setTextColor(BLACK, WHITE);
//          }
//          else 
//          {
//            display.setTextColor(WHITE, BLACK);
//          }
// 
//     display.setCursor(0, 35);
//     display.print(F("Sensitivity: "));
//     display.print(sensitivity);
//     //todo + przeniesc wykrywanie i obsluge alarmu po wstrzasie do zewnetrzej funkcji. Alarm ma dzialac bez ekranu nawet.
//     //zapis sensitivity do pamieci/ podzial sensitivity na 4 czesci
//   //  if (shakeSensorAnalog >= sensitivity ){howManyShocksAlreadyTrigger++;  display.setCursor(0, 45); display.print("TRIGGERED");display.print(howManyShocksAlreadyTrigger);}
////howManyShocksToTriggerAlarmx
////howManyShocksAlreadyTrigger
//       if (displayItem == 2) 
//          { 
//            display.setTextColor(BLACK, WHITE);
//          }
//          else 
//          {
//            display.setTextColor(WHITE, BLACK);
//          }
//          
//           display.setCursor(0, 55);
//           display.print(F("<-Wstecz "));
//              
//      display.display();
 }
 //Opcje->Temperatura   
 else if (displayPage == 6){     
     screenHeader(20," Temperatura ");
      if (currentMillis - previousMillis >= interval) {
          // save the last time you blinked the LED
          previousMillis = currentMillis;
          sensors.requestTemperaturesByAddress(Wortin);
          //Odczyt w&nbsp;stopniach celsjusza
          
          // Check if reading was successful
            if(sensors.getTempCByIndex(0) != DEVICE_DISCONNECTED_C) 
            {
                     
                    temperatureC = sensors.getTempCByIndex(0);
                     
             }else{
                 
                       temperatureC = 0;
              }
             
          }
     display.setTextSize(3);
     display.print(temperatureC);
     display.print(" C");
     display.display();
 }




}//end sisplaymenu()

void screenHeader(int x, char *header)
{
         display.setTextSize(1);
        display.clearDisplay();
        display.setTextColor(BLACK, WHITE);
        display.setCursor(x, 0);
        display.print(header);
        display.drawFastHLine(0,10,128,WHITE);
        display.setTextColor(WHITE, BLACK);
        display.setCursor(0, 15);
 }


//OPCJE - mechanizm wyboru odpowiedniej opcji, przechodzi do poczatku listy jak jestesmy juz na koncu, int wersion
void selectSwitcher(int amountOfItem){
     if (up) //jezeli joyem do gory
       {
        up = false; //usuwamy od razu stan
        displayItem++; //wystieltamy kolejny element
          if (displayItem == (amountOfItem+1)) //jezelu dojdziemy do konca listy
          {
            displayItem = 1; //wracamy na poczatek
          }      
        }else if(down) 
        {
          down = false;
          displayItem--;
          if (displayItem == 0)
          {
            displayItem = amountOfItem;
          }  
        }}



//OPCJE - mechanizm wyboru odpowiedniej opcji, przechodzi do poczatku listy jak jestesmy juz na koncu, string version
void selectSwitcher2(int amountOfItem,   char *myStrings[]){
     if (up) //jezeli joyem do gory
       {
         
        up = false; //usuwamy od razu stan
        iter++;
       
        displayItemText = myStrings[iter]; //wystieltamy kolejny element
        //Serial.println(displayItemText);
          if (iter == (amountOfItem)) //jezelu dojdziemy do konca listy
          {
            iter=0;
            displayItemText = myStrings[0]; //wracamy na poczatek
              //Serial.println(displayItemText);
          }      
        }else if(down) 
        {
            
          down = false;
           iter--;
             
           displayItemText = myStrings[iter]; 
            Serial.println(iter);
          if (iter == -1)
          {
            iter=amountOfItem-1;
            displayItemText = myStrings[amountOfItem-1];
             //Serial.println(displayItemText);
          }  
        }
   
        }
