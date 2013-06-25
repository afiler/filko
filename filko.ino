#include <LiquidCrystal.h>
#include "/Users/andyf/src/filko/block_font_b.h"

const int SELECTOR_PIN = A5;
const int POWER_PIN = 8;
const int SW2_PIN = 10;
const int SW1_PIN = 7;
const int METER_PIN = 9;
const int MUX_INPUT_PIN = 6;
const int MUX_STROBE_PIN = 13;

char titles[12][17];
char status[17] = "                ";
char outStr[80];
int selection, power, sw1, sw2;
byte clock[4] = {255, 255, 255, 255};

int oldSelection = -1;
int oldPower = -1;
int oldSw1 = -1;
int oldSw2 = -1;

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
  
  for (int i=0; i<12; i++)
    snprintf(titles[i], 10, "Title %02d", i);
  
  lcd.begin(16, 2);
  lcd.clear();
  
  lcd.clear();
  
  load_font();
  put_num(1, 0);
  put_num(2, 3);
  put_num(3, 6);
  put_num(4, 9);
  put_num(5, 12);
  delay(500);
  put_num(6, 0);
  put_num(7, 3);
  put_num(8, 6);
  put_num(9, 9);
  put_num(0, 12);
  delay(500);
  
  lcd.clear();
  
  Serial.begin(9600);
  report();
}

void loop() {  
  selection = which_pin();
  power = !digitalRead(POWER_PIN);
  sw1 = !digitalRead(SW1_PIN);
  sw2 = !digitalRead(SW2_PIN);
  
  if (oldSelection != selection || oldPower != power ||
    oldSw1 != sw1 || oldSw2 != sw2)
  {
    if (oldPower != power) lcd.clear();
    draw_screen();
    report();
    oldSelection = selection;
    oldPower = power;
    oldSw1 = sw1;
    oldSw2 = sw2;
  }
  
  handle_serial();
}

void report() {
  snprintf(outStr, 80, "r%d,%d,%d,%d", 
    selection, power, sw1, sw2);
  Serial.println(outStr);
}

void draw_screen() {
  if (power) {
    put_num(selection < 9 ? -1 : (selection + 1) / 10, 0);
    put_num((selection + 1) % 10, 3);
    lcd.setCursor(6, 0);
    lcd.print(titles[selection]);
    lcd.setCursor(6, 1);
    //lcd.print(outStr);
    lcd.print(status);
  } else if (clock[0] >= 24 || clock[1] >= 24 || clock[2] >= 24 || clock[3] >= 24) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("\xa5");
  } else {
    put_num(clock[0], 0);
    put_num(clock[1], 3);
    lcd.setCursor(5, 0);
    lcd.print("\xa5");
    lcd.setCursor(5, 1);
    lcd.print("\xa5");
    put_num(clock[2], 6);
    put_num(clock[3], 9);
  }
}

void handle_serial() {
  while (Serial.available()) handle_line();
  draw_screen();
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
  
  switch (cmd) {
    case 't':
      set_title(line);
      break;
    case 's':
      set_status(line);
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
  snprintf(titles[i], 17, "%-16s", str+2);
}

void set_status(char *str) {
  snprintf(status, 17, "%-16s", str+1);
}

void set_clock(char *str) {
  clock[0] = str[0] - 0x30;
  clock[1] = str[1] - 0x30;
  clock[2] = str[2] - 0x30;
  clock[3] = str[3] - 0x30;
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
  lcd.print(digit < 0 ? ' ' : get_char_block(numerals[digit][0]));
  lcd.print(digit < 0 ? ' ' : get_char_block(numerals[digit][1]));
  lcd.setCursor(pos, 1);
  lcd.print(digit < 0 ? ' ' : get_char_block(numerals[digit][2]));
  lcd.print(digit < 0 ? ' ' : get_char_block(numerals[digit][3]));
}

char get_char_block(char c) {
  if (c == 0) return ' ';
  if (c == 9) return '_';
  return c-1;
}
