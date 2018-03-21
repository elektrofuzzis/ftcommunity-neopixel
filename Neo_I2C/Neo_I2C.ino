// ************************************************
//
// fischertechnik NeoPixel Controller
//
// Version 1.0a
//
// (C) 2017 Christian Bergschneider & Stefan Fuss
//
// ************************************************

#include <Wire.h>
#include <Adafruit_NeoPixel.h>

// limits the Brightness to 12,5% and max. current per LED
// do not change without detailed power calculation!
#define MAX_Brightness           32

// Digital pins to select the mode
#define  SW_MODE0                10
#define  SW_MODE1                11
#define  SW_MODE2                12

// I2C Address
#define  SW_I2C0                 3
#define  SW_I2C1                 4
#define  SW_I2C2                 5
#define  SW_I2C3                 6
#define  SW_I2C4                 7
#define  SW_I2C5                 8
#define  SW_I2C6                 9


// Modes
#define  MODE_I2C                0
#define  MODE_NightRider         1
#define  MODE_RunningLight       2
#define  MODE_Fill               3
#define  MODE_Blink              4
#define  MODE_Fader              5
#define  MODE_SideStep           6
#define  MODE_Rainbow            7

// RGB/RGBW
#define  SW_TYPE_RGBW            9

// RGB Pins
#define  SW_COLOR2               8
#define  SW_COLOR1               7
#define  SW_COLOR0               6

// Digital Pins to define the number of LEDs
#define  SW_LEDS0                3
#define  SW_LEDS1                4
#define  SW_LEDS2                5


// I2C stuff
#define  MAX_SENT_BYTES          10
byte     receivedCommands[MAX_SENT_BYTES];
boolean  newI2CDataAvailable = false;

#define I2C_CMD_Initialize           0
#define I2C_CMD_Show                 1
#define I2C_CMD_setBrightness        2
#define I2C_CMD_setPixelColor        3
#define I2C_CMD_setPixelColorRGBW    4
#define I2C_CMD_setPixelColorAll     5
#define I2C_CMD_setPixelColorRGBWAll 6
#define I2C_CMD_NightRider           7
#define I2C_CMD_RunningLight         8
#define I2C_CMD_Fill                 9
#define I2C_CMD_Blink                10
#define I2C_CMD_Fader                11
#define I2C_CMD_SideStep             12
#define I2C_CMD_Rainbow              13

const int NumberOfLEDs[] = {1,2,4,8,12,16,32,64};

uint32_t DELAY = 400;
boolean  firstTime = true;
byte     LastMode  = -1;
byte     ThisMode;

#define  NEOPIXEL_PIN           2
Adafruit_NeoPixel NeoPixel;

byte SWGetMode() {
  // returns the selected mode (dip switches)

   return digitalRead( SW_MODE0 ) + 
          digitalRead( SW_MODE1 ) * 2 +
          digitalRead( SW_MODE2 ) * 4 ;
}

byte SWGetLEDs() {
  // returns the number of LEDs (dip switches)

  return NumberOfLEDs[ digitalRead( SW_LEDS0 ) + 
                       digitalRead( SW_LEDS1 ) * 2 +
                       digitalRead( SW_LEDS2 ) * 4 ];
}

byte SWGetI2CAddress() {
  // returns the I2C Address (dip switches)

  return digitalRead( SW_I2C0 ) + 
         digitalRead( SW_I2C1 ) * 2 +
         digitalRead( SW_I2C2 ) * 4 +
         digitalRead( SW_I2C3 ) * 8 +
         digitalRead( SW_I2C4 ) * 16 +
         digitalRead( SW_I2C5 ) * 32 +
         digitalRead( SW_I2C6 ) * 64;
}

byte SWGetRGBWType() {
  // return RGB/RGBW

  if ( digitalRead( SW_TYPE_RGBW ) ) {
    return NEO_RGBW;
  } else {
    return NEO_RGB;
  }
}

uint32_t SWGetRainbowParam() {
  // return RainbowParam

  return ( digitalRead(SW_COLOR0) +
           digitalRead(SW_COLOR1) * 2 +
           digitalRead(SW_COLOR2) * 4 ) * 4;

}      

uint32_t SWGetColor() {
  // return Color

  switch ( digitalRead(SW_COLOR0) +
           digitalRead(SW_COLOR1) * 2 +
           digitalRead(SW_COLOR2) * 4 ) {
    case 0:
      // green
      return NeoPixel.Color( 255, 0, 0 );
      break;
    case 1:
      // red
      return NeoPixel.Color( 0, 255, 0 );
      break;
    case 2:
      // blue
      return NeoPixel.Color( 0, 0, 255 );
      break;
    case 3:
      // pink
      return NeoPixel.Color( 0, 255, 255 );
      break;
    case 4:
      // purple
      return NeoPixel.Color( 0, 128, 255 );
      break;
    case 5:
      // yellow
      return NeoPixel.Color( 128, 255, 0 );
      break;
    case 6:
      // light blue
      return NeoPixel.Color( 255, 0, 128 );
      break;
    case 7:
      // white
      return NeoPixel.Color( 255, 255, 255 );
      break;
    default:
      // nothing
      return NeoPixel.Color( 0, 0, 0 );
  }
  
}

void debug( char* Msg ) {
  Serial.println( Msg );
}

void debugValue( char* Msg, int Value) {
  char Buffer[255];
  sprintf( Buffer, Msg, Value );
  Serial.println( Buffer );
}

void debugValue( char* Msg, int Value1, int Value2) {
  char Buffer[255];
  sprintf( Buffer, Msg, Value1, Value2 );
  Serial.println( Buffer );
}

void debugValue( char* Msg, int Value1, int Value2, int Value3) {
  char Buffer[255];
  sprintf( Buffer, Msg, Value1, Value2, Value3 );
  Serial.println( Buffer );
}

void debugValue( char* Msg, int Value1, int Value2, int Value3, int Value4) {
  char Buffer[255];
  sprintf( Buffer, Msg, Value1, Value2, Value3, Value4 );
  Serial.println( Buffer );
}

void debugValue( char* Msg, int Value1, int Value2, int Value3, int Value4, int Value5) {
  char Buffer[255];
  sprintf( Buffer, Msg, Value1, Value2, Value3, Value4, Value5 );
  Serial.println( Buffer );
}

void setup() {
  // put your setup code here, to run once:

  // Setup serial IO
  Serial.begin( 9600 );

  // Setup digtial IO
  pinMode( SW_MODE0, INPUT_PULLUP );
  pinMode( SW_MODE1, INPUT_PULLUP );
  pinMode( SW_MODE2, INPUT_PULLUP );

  pinMode( SW_TYPE_RGBW, INPUT_PULLUP );
  pinMode( SW_COLOR0, INPUT_PULLUP );
  pinMode( SW_COLOR1, INPUT_PULLUP );
  pinMode( SW_COLOR2, INPUT_PULLUP );

  pinMode( SW_LEDS0, INPUT_PULLUP );
  pinMode( SW_LEDS1, INPUT_PULLUP );
  pinMode( SW_LEDS2, INPUT_PULLUP );


  if (SWGetMode() == MODE_I2C) {

    // I2C
    Wire.begin(SWGetI2CAddress()); 
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);
    
  } else {
    
    // Setup NeoPixels
    NeoPixel = Adafruit_NeoPixel( SWGetLEDs(), NEOPIXEL_PIN, SWGetRGBWType() + NEO_KHZ800);
    NeoPixel.begin();
    NeoPixel.setBrightness(MAX_Brightness);
    NeoPixel.show();
  }

}

void All( uint32_t color) {
  // set all LEDs to one COLOR

  for (int i=0; ((i<NeoPixel.numPixels())&(!newI2CDataAvailable)); i++) {
    NeoPixel.setPixelColor( i, color );
    NeoPixel.show();
  }
   
}

void AllOff() {
  // set all LEDs off

  All( NeoPixel.Color( 0, 0, 0 ) );
  
}

void NightRider(uint32_t LedColor, uint32_t wait) {
  // NightRider with selectable color

  for (int i=0; ((i<NeoPixel.numPixels())&(!newI2CDataAvailable)); i++) {
    NeoPixel.setPixelColor( i-1, 0, 0, 0 );
    NeoPixel.setPixelColor( i, LedColor );
    NeoPixel.show();
    delay(wait);
  }

  for (int i=NeoPixel.numPixels()-2; ((i>=0)&(!newI2CDataAvailable)); i--) {
    NeoPixel.setPixelColor( i+1, 0, 0, 0 );
    NeoPixel.setPixelColor( i, LedColor );
    NeoPixel.show();
    delay(wait);
  }
  
}

void RunningLight(uint32_t LedColor, uint32_t wait) {
  // Running Light with selectable color

  for (int i=0; ((i<=NeoPixel.numPixels())&(!newI2CDataAvailable)); i++) {
    NeoPixel.setPixelColor( i-1, 0, 0, 0 );
    NeoPixel.setPixelColor( i, LedColor );
    NeoPixel.show();
    delay(wait);
  }

}

void Fill(uint32_t LedColor, uint32_t wait) {
  // Fill with selectable color

  for (int j=NeoPixel.numPixels(); j>1; j--) {

    for (int i=0; ((i<j)&(!newI2CDataAvailable)); i++) {
      NeoPixel.setPixelColor( i-1, 0, 0, 0 );
      NeoPixel.setPixelColor( i, LedColor );
      NeoPixel.show();
      delay(wait);
    }

  }

  AllOff();
  delay(DELAY);

}

void Blink(uint32_t LedColor, uint32_t wait) {
  // Blink with selectable COLOR

  All( LedColor);
  delay(wait);
  
  AllOff();
  delay(wait);
}

void Fader(uint32_t LedColor, uint32_t wait) {
  // BlinkFade with selectable COLOR

  All( LedColor);

  for (int i=1; (i<=MAX_Brightness); i++ ) {
    NeoPixel.setBrightness(i);
    NeoPixel.show();
    delay(wait);
  }
      
  for (int i=MAX_Brightness; (i>=1); i-- ) {
    NeoPixel.setBrightness(i);
    NeoPixel.show();
    delay(wait);
  }
  
}

void SideStep(uint32_t LedColor, uint32_t wait) {
  // SideStep with selectable COLOR

  AllOff( );

  for (int i=0; (i<NeoPixel.numPixels()); i=i+4 ) {
    NeoPixel.setPixelColor( i, LedColor );
    NeoPixel.setPixelColor( i+1, LedColor );
    NeoPixel.setPixelColor( i+2, 0, 0, 0 );
    NeoPixel.setPixelColor( i+3, 0, 0, 0 );
  }
  NeoPixel.show();
  delay(wait);
  
  for (int i=0; (i<NeoPixel.numPixels()); i=i+4 ) {
    NeoPixel.setPixelColor( i, 0, 0, 0 );
    NeoPixel.setPixelColor( i+1, LedColor );
    NeoPixel.setPixelColor( i+2, LedColor );
    NeoPixel.setPixelColor( i+3, 0, 0, 0 );
  }
  NeoPixel.show();
  delay(wait);

  for (int i=0; (i<NeoPixel.numPixels()); i=i+4 ) {
    NeoPixel.setPixelColor( i, 0, 0, 0 );
    NeoPixel.setPixelColor( i+1, 0, 0, 0 );
    NeoPixel.setPixelColor( i+2, LedColor );
    NeoPixel.setPixelColor( i+3, LedColor );
  }
  NeoPixel.show();
  delay(wait);

  for (int i=0; (i<NeoPixel.numPixels()); i=i+4 ) {
    NeoPixel.setPixelColor( i, LedColor );
    NeoPixel.setPixelColor( i+1, 0, 0, 0 );
    NeoPixel.setPixelColor( i+2, 0, 0, 0 );
    NeoPixel.setPixelColor( i+3, LedColor );
  }
  NeoPixel.show();
  delay(wait);

}

uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return NeoPixel.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return NeoPixel.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return NeoPixel.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void Rainbow(uint8_t wait, byte factor) {
  uint16_t i, j;

  for(j=0; ((j<256)&(!newI2CDataAvailable)); j++) {
    for(i=0; i<NeoPixel.numPixels(); i++) {
      NeoPixel.setPixelColor(i, Wheel((i*factor+j) & 255));
    }
    NeoPixel.show();
    delay(wait);
  }
}


void requestEvent() {
  Wire.write( newI2CDataAvailable );
}


void receiveEvent(int bytesReceived){

  // get the data from master
  for (int a = 0; a < bytesReceived; a++) {
    if ( a < MAX_SENT_BYTES) {
      receivedCommands[a] = Wire.read();
    } else {
      Wire.read();  // if we receive more data then allowed just throw it away
    }
  }

  // fillup CMD buffer with 0, if the master send less than MAX_SENT_BYTES
  for (int a = bytesReceived; a<MAX_SENT_BYTES; a++) {
    receivedCommands[a] = 0;
  }

  newI2CDataAvailable = true;

}   

void I2C( byte addr ) {

  int RGB_Type;
  int LED = receivedCommands[2] + receivedCommands[1 ] * 256;

  if (newI2CDataAvailable) {

    switch (receivedCommands[0]) {
      case I2C_CMD_Initialize:
        // #LED msb, #LED lsb, Type RGBW
        RGB_Type = receivedCommands[3]*256 + receivedCommands[4];
        debugValue( "I2C_CMD_Initialize(%i, %i, %i)", LED, RGB_Type, receivedCommands[5] );
        NeoPixel = Adafruit_NeoPixel( LED, NEOPIXEL_PIN, RGB_Type + NEO_KHZ800);
        NeoPixel.begin();
        NeoPixel.setBrightness( min( receivedCommands[5], MAX_Brightness ) );
        NeoPixel.show();
        break;
      case I2C_CMD_Show:
        // Show
        debug( "I2C_CMD_Show" );
        NeoPixel.show();
        break;
      case I2C_CMD_setBrightness:
        // Brightness
        debugValue( "I2C_CMD_setBrightness(%i)",receivedCommands[1] );
        NeoPixel.setBrightness( min( receivedCommands[1], MAX_Brightness ) );
        NeoPixel.show();
        break;
      case I2C_CMD_setPixelColor:
        // #LED msb, #LED lsb, Red, Green, Blue
        debugValue( "I2C_CMD_setPixelColor(%i, %i, %i, %i)", LED,receivedCommands[3], receivedCommands[4], receivedCommands[5] );
        NeoPixel.setPixelColor( LED, receivedCommands[3], receivedCommands[4], receivedCommands[5] );
        NeoPixel.show();
        break;
      case I2C_CMD_setPixelColorRGBW:
        // #LED msb, #LED lsb, Red, Green, Blue, White
        debugValue( "I2C_CMD_setPixelColorRGBW(%i, %i, %i, %i, %i)", LED,receivedCommands[3], receivedCommands[4], receivedCommands[5], receivedCommands[6] );
        NeoPixel.setPixelColor( receivedCommands[1] * 256 + receivedCommands[2], receivedCommands[3], receivedCommands[4], receivedCommands[5], receivedCommands[6] );
        break;
      case I2C_CMD_setPixelColorAll:
        // Red, Green, Blue
        debugValue( "I2C_CMD_setPixelColorAll(%i, %i, %i, %i)" ,receivedCommands[1], receivedCommands[2], receivedCommands[3], NeoPixel.numPixels() );
        for (int a=0; a<NeoPixel.numPixels(); a++) {
          NeoPixel.setPixelColor( a, receivedCommands[1], receivedCommands[2], receivedCommands[3] );
        }
        break;  
      case I2C_CMD_setPixelColorRGBWAll:
        // Red, Green, Blue
        for (int a=0; a<NeoPixel.numPixels(); a++) {
          NeoPixel.setPixelColor( a, receivedCommands[1], receivedCommands[2], receivedCommands[3], receivedCommands[4] );
        }
        break;
    }

    newI2CDataAvailable = false;
  }

  // continous commands
  switch (receivedCommands[0]) {    
    case I2C_CMD_NightRider-1:
      // catch non-continous commands
      break;
    case I2C_CMD_NightRider:
      // Red, Green, Blue, delay msb, delay lsb
      NightRider(NeoPixel.Color( receivedCommands[1], receivedCommands[2], receivedCommands[3]),receivedCommands[4] * 256 + receivedCommands[5]);
      break;
    case I2C_CMD_RunningLight:
      // Red, Green, Blue, delay msb, delay lsb
      RunningLight(NeoPixel.Color( receivedCommands[1], receivedCommands[2], receivedCommands[3]),receivedCommands[4] * 256 + receivedCommands[5]);
      break;
    case I2C_CMD_Fill:
      // Red, Green, Blue, delay msb, delay lsb
      Fill(NeoPixel.Color( receivedCommands[1], receivedCommands[2], receivedCommands[3]),receivedCommands[4] * 256 + receivedCommands[5]);
      break;
    case I2C_CMD_Blink:
      // Red, Green, Blue, delay msb, delay lsb
      Blink(NeoPixel.Color( receivedCommands[1], receivedCommands[2], receivedCommands[3]),receivedCommands[4] * 256 + receivedCommands[5]);
      break;
    case I2C_CMD_Fader:
      // Red, Green, Blue, delay msb, delay lsb
      Fader(NeoPixel.Color( receivedCommands[1], receivedCommands[2], receivedCommands[3]),receivedCommands[4] * 256 + receivedCommands[5]);
      break;
    case I2C_CMD_SideStep:
      // Red, Green, Blue, delay msb, delay lsb
      SideStep(NeoPixel.Color( receivedCommands[1], receivedCommands[2], receivedCommands[3]),receivedCommands[4] * 256 + receivedCommands[5]);
      break;
    case I2C_CMD_Rainbow:
      // Increment, delay msb, delay lsb
      Rainbow(receivedCommands[1], receivedCommands[2] * 256 + receivedCommands[3]);
      break;
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:

  if (firstTime) {
    debug("");
    debug( "fischertechnik NeoPixel Controller" );
    debug( "Version 1.00a" );
    debug( "(C) 2017 Christian Bergschneider, Stefan Fuss" );
    firstTime = false;
  }

  ThisMode = SWGetMode();

  if ( LastMode != ThisMode ) {
    AllOff();
    switch (ThisMode) {
      case MODE_I2C:
        //I2C
        debugValue("I2C mode(%i)", SWGetI2CAddress() );
        break;
      case MODE_NightRider:
        debugValue("NightRider(%i)", SWGetColor() );
        break;
      case MODE_RunningLight:
        debugValue("RunningLight(%i)", SWGetColor() );
        break;
      case MODE_Fill:
        debugValue("Fill(%i)", SWGetColor() );
        break;
      case MODE_Blink:
        debugValue("Blink(%i)", SWGetColor() );
        break;
      case MODE_Fader:
        debugValue("Fader(%i)", SWGetColor() );
        break;
      case MODE_SideStep:
        debugValue("SideStep(%i)", SWGetColor() );
        break;
      case MODE_Rainbow:
        debugValue("Rainbow3(10, %i)", SWGetRainbowParam() );;
        break;
      default:
        debug("default");
        break;
    }
  }

  switch (ThisMode) {
    case MODE_I2C:
      //I2C
      I2C(SWGetI2CAddress());
      break;
    case MODE_NightRider:
      NightRider(SWGetColor(), DELAY);
      break;
    case MODE_RunningLight:
      RunningLight(SWGetColor(), DELAY);
      break;
    case MODE_Fill:
      Fill(SWGetColor(), DELAY);
      break;
    case MODE_Blink:
      Blink(SWGetColor(), DELAY);
      break;
    case MODE_Fader:
      Fader(SWGetColor(), DELAY);
      break;
    case MODE_SideStep:
      SideStep(SWGetColor(), DELAY);
      break;
    case MODE_Rainbow:
      Rainbow(10, SWGetRainbowParam());
      break;
    default:
      debug("default");
      break;
  }

  LastMode = ThisMode;

}
