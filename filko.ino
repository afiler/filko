#include <LiquidCrystal.h>


//   0   1   2   3   4   5   6   7
// .X  XX  XX  XX  X.  .X  XX  XX
// .X  X.  .X  ..  X.  .X  X.  .X
// .X  X.  .X  XX  XX  XX  XX  XX


char char_blocks[8][8]  = {
  {
    0b00011,
    0b00011,
    0b00000,
    0b00011,
    0b00011,
    0b00000,
    0b00011,
    0b00011
  },

  {
    0b11011,
    0b11011,
    0b00000,
    0b11000,
    0b11000,
    0b00000,
    0b11000,
    0b11000
  },
  
  {
    0b11011,
    0b11011,
    0b00000,
    0b00011,
    0b00011,
    0b00000,
    0b00011,
    0b00011
  },

  {
    0b11011,
    0b11011,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b11011,
    0b11011
  },

  {
    0b11000,
    0b11000,
    0b00000,
    0b11000,
    0b11000,
    0b00000,
    0b11011,
    0b11011
  },

  {
    0b00011,
    0b00011,
    0b00000,
    0b00011,
    0b00011,
    0b00000,
    0b11011,
    0b11011
  },

  {
    0b11011,
    0b11011,
    0b00000,
    0b11000,
    0b11000,
    0b00000,
    0b11011,
    0b11011
  },

  {
    0b11011,
    0b11011,
    0b00000,
    0b00011,
    0b00011,
    0b00000,
    0b11011,
    0b11011
  }
};

char numerals[10][4] = {
  {2, 3, 5, 6}, // 0
  {0, 1, 0, 1}, // 1
  {4, 8, 5, 9}, // 2
  {4, 8, 9, 6}, // 3
  {5, 6, 0, 1}, // 4
  {7, 4, 9, 6}, // 5
  {7, 4, 5, 6}, // 6
  {2, 3, 0, 1}, // 7
  {7, 8, 5, 6}, // 8
  {7, 8, 0, 1}
};




const int SELECTOR_PIN = A5;
const int POWER_PIN = 8;
const int SW2_PIN = 7;
const int SW1_PIN = 10;
const int METER_PIN = 9;
const int MUX_INPUT_PIN = 6;
const int MUX_STROBE_PIN = 13;

char titles[12][21];

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  pinMode(SELECTOR_PIN, INPUT);
  digitalWrite(SELECTOR_PIN, HIGH);
  pinMode(POWER_PIN, INPUT);
  digitalWrite(POWER_PIN, HIGH);
  pinMode(SW1_PIN, INPUT);
  digitalWrite(SW1_PIN, HIGH);
  pinMode(SW2_PIN, INPUT);
  digitalWrite(SW2_PIN, HIGH);
  pinMode(METER_PIN, OUTPUT);
  analogWrite(METER_PIN, 3);
  pinMode(MUX_INPUT_PIN, INPUT);
  pinMode(MUX_STROBE_PIN, OUTPUT);
  digitalWrite(MUX_STROBE_PIN, HIGH);
  //digitalWrite(INPUT_PIN, HIGH);
  DDRC = 0x0F;
  
  for (int i=0; i<12; i++) {
    //snprintf(titles[i], 21, "Title %02d %c    ", i, i);
    snprintf(titles[i], 21, "[\x01] #%02d %c %c %c %c    ", i, i, i+12, i+24, i+36);
  }
  
  lcd.begin(16, 2);
  lcd.clear();
  
  lcd.print("*****");
  delay(200);
  
  lcd.clear();
  
  load_font();
  put_num(1, 0);
  put_num(2, 3);
  put_num(3, 6);
  put_num(4, 9);
  put_num(5, 12);
  delay(1000);
  put_num(6, 0);
  put_num(7, 3);
  put_num(8, 6);
  put_num(9, 9);
  put_num(0, 12);
  delay(1000);
  
  Serial.begin(9600);
}

void loop() {
  int selection;
  char outStr[80];
  int selectorVal = which_pin(); // = analogRead(SELECTOR_PIN);

  /* snprintf(outStr, 80, "0,%d,%d,%d,%d,%d,%d", 
    selection,
    selectorVal,
    digitalRead(POWER_PIN),
    digitalRead(SW1_PIN),
    digitalRead(SW2_PIN),
    selectorVal); */

  //lcd.setCursor(0,1);
  //lcd.print(titles[selectorVal]);
  //check_pins();

  //lcd.clear();
  put_num(0, 0);
  put_num(0, 3);
  lcd.setCursor(5, 0);
  lcd.print("\xa5");
  lcd.setCursor(5, 1);
  lcd.print("\xa5");
  put_num(selectorVal / 10, 6);
  put_num(selectorVal % 10, 9);
  
  handle_serial();
}

void handle_serial() {
  while (Serial.available()) handle_line();
}

void handle_line() {
  if (!Serial.available()) return;
  
  char line[81];
  char c, i;
  int cmd;
  
  cmd = Serial.read();
  
  while (Serial.available() && (c = Serial.read()) && c != '\n')
    line[i++] = c;
  
  line[i++] = 0;
  /* lcd.setCursor(0,0);
  lcd.print("SER");
  lcd.print(line); */

  switch (cmd) {
    case 't':
      set_title(line);
      break;
    case 'c':
      set_clock(line);
      break;
    default:
      break;
  }
  
  Serial.println("ok");
}

void set_title(char *str) {
  int i = 10*(str[0]-0x30)+(str[1]-0x30);
  snprintf(titles[i], 21, "%-20s", str+2);
}

void set_clock(char *str) {
  
}


int which_pin() {
  for(int i=0; i<12; i++) {
    PORTC = i; //(PORTC & 0xf0) | (i & 0x0f);
    delay(10);
    digitalWrite(MUX_STROBE_PIN, LOW);
    int val = digitalRead(MUX_INPUT_PIN);
    digitalWrite(MUX_STROBE_PIN, HIGH);
    if (!val) return i;
  }
  return -1;
}

int check_pins() {
  for(int i=0; i<12; i++) {
    PORTC = i; //(PORTC & 0xf0) | (i & 0x0f);
    //delay(10);
    digitalWrite(MUX_STROBE_PIN, LOW);
    int val = digitalRead(MUX_INPUT_PIN);
    digitalWrite(MUX_STROBE_PIN, HIGH);
    lcd.setCursor(i, 0);
    if (!val) {
      lcd.print(i, HEX);
    } else {
      lcd.print(' ');
    }
  }
  return -1;
}



void load_font() {
  for (int pos = 0; pos < 8; pos++) {
    load_char(pos, char_blocks[pos]);
  }
}

void load_char(char pos, char* c) {
  for (int i=0; i < 8; i++) {
    lcd.command(0x40 + (pos)*8 + i); 
    lcd.write(c[i]);
  }
}

void put_num(int digit, int pos) {
  lcd.setCursor(pos, 0);
  lcd.print(get_char_block(numerals[digit][0]));
  lcd.print(get_char_block(numerals[digit][1]));
  lcd.setCursor(pos, 1);
  lcd.print(get_char_block(numerals[digit][2]));
  lcd.print(get_char_block(numerals[digit][3]));
}

char get_char_block(char c) {
  if (c == 0) return ' ';
  if (c == 9) return '_';
  return c-1;
}
