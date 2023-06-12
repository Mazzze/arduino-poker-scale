/*
 This example code uses bogde's excellent library:"https://github.com/bogde/HX711"
*/



#include "HX711.h"

#define calibration_factor_green 1140.0
#define calibration_factor_blue 1095.0
#define calibration_factor_white 1146.0
#define calibration_factor_red 1138.0

#define green_LOADCELL_DOUT_PIN  2
#define green_LOADCELL_SCK_PIN  3
#define blue_LOADCELL_DOUT_PIN  4
#define blue_LOADCELL_SCK_PIN  5
#define white_LOADCELL_DOUT_PIN  6
#define white_LOADCELL_SCK_PIN  7
#define red_LOADCELL_DOUT_PIN  8
#define red_LOADCELL_SCK_PIN  9

HX711 scale_green;
HX711 scale_blue;
HX711 scale_white;
HX711 scale_red;

#include <Arduino.h>
#include <AceTMI.h> // SimpleTmi1637Interface
#include <AceSegment.h> // Tm1637Module

using ace_tmi::SimpleTmi1637Interface;
using ace_segment::Tm1637Module;

// Replace these with the PIN numbers of your dev board.
const uint8_t CLK_PIN = A0;
const uint8_t DIO_PIN = 12;
const uint8_t NUM_DIGITS = 4;

// Many TM1637 LED modules contain 10 nF capacitors on their DIO and CLK lines
// which are unreasonably high. This forces a 100 microsecond delay between
// bit transitions. If you remove those capacitors, you can set this as low as
// 1-5 micros.
const uint8_t DELAY_MICROS = 100;

using TmiInterface = SimpleTmi1637Interface;
TmiInterface tmiInterface(DIO_PIN, CLK_PIN, DELAY_MICROS);
Tm1637Module<TmiInterface, NUM_DIGITS> ledModule(tmiInterface);

// Default weights and values
float green_weight = 11.36;
int green_unit_value = 25;
float blue_weight = 11.43;
int blue_unit_value = 10;
float white_weight = 11.5;
int white_unit_value = 1;
float red_weight = 11.43;
int red_unit_value = 5;

// LED segment patterns.
const uint8_t NUM_PATTERNS = 11;
const uint8_t neg_sign = 0b01000000;
const uint8_t PATTERNS[NUM_PATTERNS] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111, // 9
  0b00000000, // null as 10
};

const uint8_t NUM_CHARS = 80;
const char character_keys[NUM_CHARS] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
    'y', 'z', ' ', '-', '_', '=', '[', ']', '(', ')',
    '{', '}', '<', '>', ';', ':', '"', '\'', '\\', '|',
};

const uint8_t character_values[NUM_CHARS] = {
    0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111,
    0b01110111, 0b01111100, 0b01011000, 0b01011110, 0b01111001, 0b01110001, 0b01111101, 0b01110100, 0b00000110, 0b00011110,
    0b01110110, 0b00111000, 0b01010101, 0b01010100, 0b01011100, 0b01110011, 0b01100111, 0b01010000, 0b01101101, 0b01111000,
    0b00111110, 0b00101010, 0b01111110, 0b01101110, 0b01101100, 0b01011011, 0b01110111, 0b01111100, 0b01011000, 0b01011110,
    0b01111001, 0b01110001, 0b01111101, 0b01110100, 0b00000100, 0b00011110, 0b01110110, 0b00111000, 0b01010101, 0b01010100,
    0b01011100, 0b01110011, 0b01100111, 0b01010000, 0b01101101, 0b01111000, 0b00111110, 0b00101010, 0b01111110, 0b01101110,
    0b01101100, 0b01011011, 0b00000000, 0b01000000, 0b00001000, 0b01001000, 0b01111000, 0b01001110, 0b00110001, 0b00001101,
    0b01110001, 0b00011101, 0b00110000, 0b00001100, 0b01010010, 0b01010011, 0b00100010, 0b00100000, 0b01100100, 0b01100000
};

// Define menu states
enum MenuState {
  SEL_CAL,
  SEL_VAL,
  CALIBRATE_WEIGHT,
  CHANGE_VALUE,
  CSNL,
  EXIT_MENU
};

// Menu variables
MenuState currentMenuState;
int menu_speed_text = 300;


//button pins
const int tareButtonPin = 13;
const int recalibrationButtonPin = 10;

void setup() {
  Serial.begin(9600);
  Serial.println("HEllO crule world");

  scale_green.begin(green_LOADCELL_DOUT_PIN, green_LOADCELL_SCK_PIN);
  scale_green.set_scale(calibration_factor_green); //Thistotal_valueis obtained by using the SparkFun_HX711_Calibration sketch
  scale_green.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
  
  scale_blue.begin(blue_LOADCELL_DOUT_PIN, blue_LOADCELL_SCK_PIN);
  scale_blue.set_scale(calibration_factor_blue); //Thistotal_valueis obtained by using the SparkFun_HX711_Calibration sketch
  scale_blue.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0

  scale_white.begin(white_LOADCELL_DOUT_PIN, white_LOADCELL_SCK_PIN);
  scale_white.set_scale(calibration_factor_white); //Thistotal_valueis obtained by using the SparkFun_HX711_Calibration sketch
  scale_white.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
  
  scale_red.begin(red_LOADCELL_DOUT_PIN, red_LOADCELL_SCK_PIN);
  scale_red.set_scale(calibration_factor_red); //Thistotal_valueis obtained by using the SparkFun_HX711_Calibration sketch
  scale_red.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0


  /* 7 segments*/
  tmiInterface.begin();
  ledModule.begin();
  ledModule.setBrightness(1);

  byte loading_speed = 0;

  for (int i = 0; i <= 3; i++) {
    for (int j = 0; j <= 7; j++){
      ledModule.setPatternAt(i, pow(2,j));
      ledModule.flush();
      delay(loading_speed);
    }
  }

  // Buttons
  pinMode(tareButtonPin, INPUT);
 
  tareing();
}

void loop() {
  float green_grams = scale_green.get_units();
  float blue_grams = scale_blue.get_units();
  float white_grams = scale_white.get_units();
  float red_grams = scale_red.get_units();

    // Send data to the serial plotter in CSV format
  Serial.print(blue_grams);
  Serial.print("<- BLUE  ");
  Serial.print(red_grams); // Use println to end the line
  Serial.print("<-  RED  ");
  Serial.print(green_grams);
  Serial.print("<-  Green  ");
  Serial.print(white_grams);
  Serial.print("<- white  ");
  Serial.println();
  
  int green_value = round(green_grams/green_weight)*green_unit_value;
  int blue_value = round(blue_grams/blue_weight)*blue_unit_value;
  int white_value = round(white_grams/white_weight)*white_unit_value;
  int red_value = round(red_grams/red_weight)*red_unit_value;

  int total_value= green_value + blue_value + white_value + red_value; // dumb value to not make number 4294967296
  int actual_value = total_value*1;
  int abs_value = abs(total_value);
  float rounding_error_green = green_value*green_weight/green_unit_value - green_grams;
  float rounding_error_blue = blue_value*blue_weight/blue_unit_value - blue_grams;
  float rounding_error_white = white_value*white_weight/white_unit_value - white_grams;
  float rounding_error_red = red_value*red_weight/red_unit_value - red_grams; 
  
 /* 
  Serial.print("Reading: ");
  Serial.print(total_value, 1);
  Serial.print(" total_value(kr), ");
  Serial.print(actual_value);
  Serial.print(" actual_value(kr), ");
  Serial.print(green_grams, 1); //scale.get_units() returns a float
  Serial.print(" g green, ");
  Serial.print(rounding_error_green);
  Serial.print(" rounding_error_green(g), ");

  Serial.print(blue_grams, 1); //scale.get_units() returns a float
  Serial.print(" g blue, ");
  Serial.print(rounding_error_blue);
  Serial.print(" rounding_error_blue(g), ");

  Serial.print(white_grams, 1); //scale.get_units() returns a float
  Serial.print(" g white, ");
  Serial.print(rounding_error_white);
  Serial.print(" rounding_error_white(g), ");

  Serial.print(red_grams, 1); //scale.get_units() returns a float
  Serial.print(" g red, ");
  Serial.print(rounding_error_red);
  Serial.print(" rounding_error_red(g), ");
  Serial.print(abs_value, 1);
  Serial.print(" abs_value (kr), ");

*/
//  Serial.print(tareValue); //scale.get_units() returns a float
//  Serial.print(" tareValue"); //You can change this to kg but you'll need to refactor the calibration_factor

  /* 7 segments */
  int digit0 = 0;
  int digit1 = 0;
  int digit2 = 0;
  int digit3 = 0;


  if (abs_value == 0) {
    digit0 = digit1 = digit2 =  digit3 = PATTERNS[0];
  };
  if (abs_value >= 1){
    digit3 = PATTERNS[(abs_value / 1U) % 10];
  }

  if (abs_value >= 10){
    digit2 = PATTERNS[(abs_value / 10U) % 10];
  }

  if (abs_value >= 100){
    digit1 = PATTERNS[(abs_value / 100U) % 10];
  }

  if (abs_value >= 1000){
    digit0 = PATTERNS[(abs_value / 1000U) % 10];
  };

  if ((actual_value <= -1) && (actual_value > -10)){
    digit2 = neg_sign;
  }

  if ((actual_value <= -10) && (actual_value > -100)){
    digit1 = neg_sign;
  }

  if ((actual_value <= -100) && (actual_value > -1000)){
    digit0 = neg_sign;
  }

// bruh text errors
  if ((actual_value <= -1000)||(actual_value >= 10000)) {
    digit0 = 0b01111100;
    digit1 = 0b01010000;
    digit2 = 0b00011100;
    digit3 = 0b01110100;
  };
 
  ledModule.setPatternAt(0, digit0);
  ledModule.setPatternAt(1, digit1);
  ledModule.setPatternAt(2, digit2);
  ledModule.setPatternAt(3, digit3);
  
  ledModule.flush();

 //Buttons
  if(digitalRead(tareButtonPin) == 1) { 
    tareing();
    }

  // Check for recalibration button press
  if (digitalRead(recalibrationButtonPin) == 1) {
    currentMenuState = SEL_CAL;
    Serial.println("menu");
    while (currentMenuState != EXIT_MENU) {
      switch (currentMenuState) {
        case SEL_CAL:
          displayCAL();
          break;
        case SEL_VAL:
          displayVAL();
          break;
        case CALIBRATE_WEIGHT:
          calibrateWeight();
          break;
        case CHANGE_VALUE:
          changeValue();
          break;
        case CSNL:
          dispCSNL();
          break;
      }
    }
    flushtextToLED("exit");
    delay(1000);
    tareing();
  }
} //loop end

void tareing() {
  // taring dispaly   
  ledModule.setPatternAt(0, neg_sign);
  ledModule.setPatternAt(1, neg_sign);
  ledModule.setPatternAt(2, neg_sign);
  ledModule.setPatternAt(3, neg_sign);   
  ledModule.flush();
  
  scale_green.tare();
  scale_blue.tare();
  scale_white.tare();
  scale_red.tare();
}

    
  
uint8_t getCharPattern(char c) {
    for (uint8_t i = 0; i < NUM_CHARS; i++) {
        if (character_keys[i] == c) {
            return character_values[i];
        }
    }
    return 0; // default value for unknown characters
}


void textToLED(String text, int delayTime) {
  if (text.length() <= 4) {
    // Display the text without scrolling
    while (true) {
      for (int i = 0; i < 4; i++) {
        char c = i < text.length() ? text.charAt(i) : ' ';
        uint8_t pattern = getCharPattern(c);
        ledModule.setPatternAt(i, pattern);

        // Check for button presses
        if (digitalRead(tareButtonPin) == HIGH || digitalRead(recalibrationButtonPin) == HIGH) {
          return;
        }
      }
      ledModule.flush();
    }
  } else {
    // Scroll the text
    String textWithSpaces = "    " + text + "    ";
    while (true) {
      for (int i = 0; i < textWithSpaces.length() - 3; i++) {
        for (int j = 0; j < 4; j++) {
          char c = textWithSpaces.charAt(i + j);
          uint8_t pattern = getCharPattern(c);
          ledModule.setPatternAt(j, pattern);

          // Check for button presses
          if (digitalRead(tareButtonPin) == HIGH || digitalRead(recalibrationButtonPin) == HIGH) {
            return;
          }
        }
        ledModule.flush();
        delay(delayTime);
      }
    }
  }
}


void flushtextToLED(String text) {
    // Display the text without scrolling
    for (int i = 0; i < 4; i++) {
      char c = i < text.length() ? text.charAt(i) : ' ';
      uint8_t pattern = getCharPattern(c);
      ledModule.setPatternAt(i, pattern);
    }
    ledModule.flush();
}

void flushtextToLeftOnLED(String text) {
    // Display the text without scrolling, from the left
    int numLEDs = 4;
    int startIndex = numLEDs - text.length();

    for (int i = 0; i < numLEDs; i++) {
        char c = i >= startIndex && i < numLEDs ? text.charAt(i - startIndex) : ' ';
        uint8_t pattern = getCharPattern(c);
        ledModule.setPatternAt(i, pattern);
    }
    ledModule.flush();
}


void displayCAL() {
  flushtextToLED(" cal");
  delay(500);
  while (true) {
    // select calibrate weight
    if (digitalRead(tareButtonPin) == HIGH && digitalRead(recalibrationButtonPin != HIGH))  {
      currentMenuState = CALIBRATE_WEIGHT;
      return;
    }
        // scroling
    if (digitalRead(recalibrationButtonPin) == HIGH && digitalRead(tareButtonPin) != HIGH)  {
      currentMenuState = SEL_VAL;
      return;
    }
  }
}

void displayVAL() {
  flushtextToLED("valu");
  delay(500);
  while (true) {
    // select calibrate weight
    if (digitalRead(tareButtonPin) == HIGH && digitalRead(recalibrationButtonPin != HIGH)) {
      currentMenuState = CHANGE_VALUE;
      return;
    }
        // scroling
    if (digitalRead(recalibrationButtonPin) == HIGH && digitalRead(tareButtonPin) != HIGH)  {
      currentMenuState = CSNL;
        return;
      }
  }
}

void dispCSNL() {
  flushtextToLED("csnL");
  delay(500);
  while (true) {
    // select calibrate weight
    if (digitalRead(tareButtonPin) == HIGH && digitalRead(recalibrationButtonPin != HIGH)) {
      green_weight = 11.3;
      blue_weight = 11.46;
      white_weight = 11.5;
      red_weight = 11.4;
      delay(500);
      textToLED("the poker game have started", 300);
      currentMenuState = EXIT_MENU;
      return;
    }
        // scroling
    if (digitalRead(recalibrationButtonPin) == HIGH && digitalRead(tareButtonPin) != HIGH)  {
      currentMenuState = EXIT_MENU;
        return;
      }
  }
}




void calibrateWeight() {
  flushtextToLED("10ch");
  delay(500);


  while (true) {
    // select calibrate weight
    if (digitalRead(tareButtonPin) == HIGH && digitalRead(recalibrationButtonPin) != HIGH)  {
      float green_grams = scale_green.get_units();
      float blue_grams = scale_blue.get_units();
      float white_grams = scale_white.get_units();
      float red_grams = scale_red.get_units();
      int numberOfchips = 10;

      if (green_grams / numberOfchips > 4 && blue_grams / numberOfchips > 4 && white_grams / numberOfchips > 4 && red_grams / numberOfchips > 4) {
        green_weight = green_grams / numberOfchips;
        blue_weight = blue_grams / numberOfchips;
        white_weight = white_grams / numberOfchips;
        red_weight = red_grams / numberOfchips;
        currentMenuState = EXIT_MENU;
        return;
      } 
      else {
        // Handle the case where one or more values are not greater than 4
        flushtextToLED("nah");
        delay(1000);
        currentMenuState = EXIT_MENU;
        return;
      }

    }
        // scroling
    if (digitalRead(recalibrationButtonPin) == HIGH && digitalRead(tareButtonPin) != HIGH)  {
      currentMenuState = SEL_VAL;
      return;
    }
  }
}

void changeValue() {
  flushtextToLED("colr");
  delay(500);
   // While loop to check if any of the colors is greater than 1000
  while (true) {
    int press_weight = 200;
    float green_grams = scale_green.get_units();
    float blue_grams = scale_blue.get_units();
    float white_grams = scale_white.get_units();
    float red_grams = scale_red.get_units();

    if (digitalRead(recalibrationButtonPin) == HIGH && digitalRead(tareButtonPin) != HIGH)  {
      currentMenuState = EXIT_MENU;
      return;
    }
    if (white_grams > press_weight) {
      flushtextToLED(" vit");
      delay(700);
      while (true) {
        green_grams = scale_green.get_units();
        blue_grams = scale_blue.get_units();
        white_grams = scale_white.get_units();
        red_grams = scale_red.get_units();
        int new_value = round(white_grams/white_weight) + round(red_grams/red_weight)*5 + round(blue_grams/blue_weight)*10 + round(green_grams/green_weight)*25;
        if (digitalRead(tareButtonPin) == HIGH) {
          white_unit_value = new_value;
          flushtextToLED(" vit");
          delay(500);
          flushtextToLeftOnLED(String(new_value));
          delay(500);
          return;
        }
        else if (digitalRead(recalibrationButtonPin) == 1){
          return;
        }
        if (new_value <= 10000) {;
          flushtextToLED("pres");
          delay(500);
          flushtextToLED("tare");
          delay(500);
          flushtextToLeftOnLED(String(new_value));
          delay(500);
        }
        else;
          flushtextToLED(" big");
      } 
    }
    else if (red_grams > press_weight) {
      flushtextToLED(" red");
      delay(700);
      while (true) {
        green_grams = scale_green.get_units();
        blue_grams = scale_blue.get_units();
        white_grams = scale_white.get_units();
        red_grams = scale_red.get_units();
        int new_value = round(white_grams/white_weight) + round(red_grams/red_weight)*5 + round(blue_grams/blue_weight)*10 + round(green_grams/green_weight)*25;
        if (digitalRead(tareButtonPin) == HIGH) {
          red_unit_value = new_value;
          flushtextToLED(" red");
          delay(500);
          flushtextToLeftOnLED(String(new_value));
          delay(500);
          return;
        }
        else if (digitalRead(recalibrationButtonPin) == 1){
          return;
        }
        if (new_value <= 10000) {;
          flushtextToLED("pres");
          delay(500);
          flushtextToLED("tare");
          delay(500);
          flushtextToLeftOnLED(String(new_value));
          delay(500);
        }
        else;
          flushtextToLED(" big");
      } 
    }
    else if (blue_grams > press_weight) {
      flushtextToLED("blue");
      delay(700);
      while (true) {
        green_grams = scale_green.get_units();
        blue_grams = scale_blue.get_units();
        white_grams = scale_white.get_units();
        red_grams = scale_red.get_units();
        int new_value = round(white_grams/white_weight) + round(red_grams/red_weight)*5 + round(blue_grams/blue_weight)*10 + round(green_grams/green_weight)*25;
        if (digitalRead(tareButtonPin) == HIGH) {
          blue_unit_value = new_value;
          flushtextToLED("blue");
          delay(500);
          flushtextToLeftOnLED(String(new_value));
          delay(500);
          return;
        }
        else if (digitalRead(recalibrationButtonPin) == 1){
          return;
        }
        if (new_value <= 10000) {;
          flushtextToLED("pres");
          delay(500);
          flushtextToLED("tare");
          delay(500);
          flushtextToLeftOnLED(String(new_value));
          delay(500);
        }
        else;
          flushtextToLED(" big");
      } 
    }
    else if (green_grams > press_weight) {
      flushtextToLED("gren");
      delay(700);
      while (true) {
        green_grams = scale_green.get_units();
        blue_grams = scale_blue.get_units();
        white_grams = scale_white.get_units();
        red_grams = scale_red.get_units();
        int new_value = round(white_grams/white_weight) + round(red_grams/red_weight)*5 + round(blue_grams/blue_weight)*10 + round(green_grams/green_weight)*25;
        if (digitalRead(tareButtonPin) == HIGH) {
          green_unit_value = new_value;
          flushtextToLED("gren");
          delay(500);
          flushtextToLeftOnLED(String(new_value));
          delay(500);
          return;
        }
        else if (digitalRead(recalibrationButtonPin) == 1){
          return;
        }
        if (new_value <= 10000) {;
          flushtextToLED("pres");
          delay(500);
          flushtextToLED("tare");
          delay(500);
          flushtextToLeftOnLED(String(new_value));
          delay(500);
        }
        else;
          flushtextToLED(" big");
      } 
    }
  }
}


  
