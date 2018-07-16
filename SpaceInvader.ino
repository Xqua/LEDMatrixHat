// Trinket/Gemma + LED matrix backpack jewelry.  Plays animated
// sequence on LED matrix.  Press reset button to display again,
// or add optional momentary button between pin #1 and +V.
// THERE IS NO ANIMATION DATA IN THIS SOURCE FILE, you should
// rarely need to change anything here.  EDIT anim.h INSTEAD.

#define BRIGHTNESS   12 // 0=min, 15=max
#define I2C_ADDR   0x71 // Edit if backpack A0/A1 jumpers set
#define I2C_ADDR_2   0x70 // Edit if backpack A0/A1 jumpers set
int I2C_ADDRS[] = {I2C_ADDR, I2C_ADDR_2};

#include <Wire.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include "anim.h"       // Animation data is located here

static const uint8_t PROGMEM reorder[] = { // Column-reordering table
    0x00,0x40,0x20,0x60,0x10,0x50,0x30,0x70,0x08,0x48,0x28,0x68,0x18,0x58,0x38,0x78,
    0x04,0x44,0x24,0x64,0x14,0x54,0x34,0x74,0x0c,0x4c,0x2c,0x6c,0x1c,0x5c,0x3c,0x7c,
    0x02,0x42,0x22,0x62,0x12,0x52,0x32,0x72,0x0a,0x4a,0x2a,0x6a,0x1a,0x5a,0x3a,0x7a,
    0x06,0x46,0x26,0x66,0x16,0x56,0x36,0x76,0x0e,0x4e,0x2e,0x6e,0x1e,0x5e,0x3e,0x7e,
    0x01,0x41,0x21,0x61,0x11,0x51,0x31,0x71,0x09,0x49,0x29,0x69,0x19,0x59,0x39,0x79,
    0x05,0x45,0x25,0x65,0x15,0x55,0x35,0x75,0x0d,0x4d,0x2d,0x6d,0x1d,0x5d,0x3d,0x7d,
    0x03,0x43,0x23,0x63,0x13,0x53,0x33,0x73,0x0b,0x4b,0x2b,0x6b,0x1b,0x5b,0x3b,0x7b,
    0x07,0x47,0x27,0x67,0x17,0x57,0x37,0x77,0x0f,0x4f,0x2f,0x6f,0x1f,0x5f,0x3f,0x7f,
    0x80,0xc0,0xa0,0xe0,0x90,0xd0,0xb0,0xf0,0x88,0xc8,0xa8,0xe8,0x98,0xd8,0xb8,0xf8,
    0x84,0xc4,0xa4,0xe4,0x94,0xd4,0xb4,0xf4,0x8c,0xcc,0xac,0xec,0x9c,0xdc,0xbc,0xfc,
    0x82,0xc2,0xa2,0xe2,0x92,0xd2,0xb2,0xf2,0x8a,0xca,0xaa,0xea,0x9a,0xda,0xba,0xfa,
    0x86,0xc6,0xa6,0xe6,0x96,0xd6,0xb6,0xf6,0x8e,0xce,0xae,0xee,0x9e,0xde,0xbe,0xfe,
    0x81,0xc1,0xa1,0xe1,0x91,0xd1,0xb1,0xf1,0x89,0xc9,0xa9,0xe9,0x99,0xd9,0xb9,0xf9,
    0x85,0xc5,0xa5,0xe5,0x95,0xd5,0xb5,0xf5,0x8d,0xcd,0xad,0xed,0x9d,0xdd,0xbd,0xfd,
    0x83,0xc3,0xa3,0xe3,0x93,0xd3,0xb3,0xf3,0x8b,0xcb,0xab,0xeb,0x9b,0xdb,0xbb,0xfb,
    0x87,0xc7,0xa7,0xe7,0x97,0xd7,0xb7,0xf7,0x8f,0xcf,0xaf,0xef,0x9f,0xdf,0xbf,0xff };

void ledCmd(uint8_t x) { // Issue command to LED backback driver
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(x);
  Wire.endTransmission();

  Wire.beginTransmission(I2C_ADDR_2);
  Wire.write(x);
  Wire.endTransmission();
}

void clear(void) { // Clear display buffer
  Wire.beginTransmission(I2C_ADDR);
  for(uint8_t i=0; i<17; i++) Wire.write(0);
  Wire.endTransmission();
  Wire.beginTransmission(I2C_ADDR_2);
  for(uint8_t i=0; i<17; i++) Wire.write(0);
  Wire.endTransmission();
}

void setup() {
  power_timer1_disable();    // Disable unused peripherals
  power_adc_disable();       // to save power
  PCMSK |= _BV(PCINT1);      // Set change mask for pin 1
  Wire.begin();              // I2C init
  clear();                   // Blank display
  ledCmd(0x21);              // Turn on oscillator
  ledCmd(0xE0 | BRIGHTNESS); // Set brightness
  ledCmd(0x81);              // Display on, no blink
}

uint8_t rep = REPS;

void Play(int Mode) {
  uint8_t AnimDelay;
  uint16_t MidAnimDelay;
  int ArraySize;
  if (Mode == 0) {
    ArraySize = sizeof(animBlink);
    AnimDelay = 15;
    MidAnimDelay = 15;
  }
  else if (Mode == 1) {
    ArraySize = sizeof(animLookRight);
    AnimDelay = 25;
    MidAnimDelay = 1000;
  }
  else if (Mode == 2) {
    ArraySize = sizeof(animLookLeft);
    AnimDelay = 25;
    MidAnimDelay = 1000;
  }
  else if (Mode == 3) {
    ArraySize = sizeof(animLookUp);
    AnimDelay = 25;
    MidAnimDelay = 1000;
  }
  else if (Mode == 4) {
    ArraySize = sizeof(animLookDown);
    AnimDelay = 25;
    MidAnimDelay = 1000;
  }
  else if (Mode == 5) {
    ArraySize = sizeof(animDead);
    AnimDelay = 30;
    MidAnimDelay = 1000;
  }
  else if (Mode == 6) {
    ArraySize = sizeof(animBlink);
    AnimDelay = 25;
    MidAnimDelay = 25;
  }
  else if (Mode == 7) {
    ArraySize = sizeof(animHeart);
    AnimDelay = 30;
    MidAnimDelay = 300;
  }
  else if (Mode == 8) {
    ArraySize = sizeof(animAngryLeft);
    AnimDelay = 30;
    MidAnimDelay = 7000;
  }
  else if (Mode == 9) {
    ArraySize = sizeof(animBlaze);
    AnimDelay = 30;
    MidAnimDelay = 4000;
  }
  else if (Mode == 10) {
    ArraySize = sizeof(animSadLeft);
    AnimDelay = 30;
    MidAnimDelay = 4000;
  }
  else if (Mode == 11) {
    ArraySize = sizeof(animManga);
    AnimDelay = 15;
    MidAnimDelay = 15;
  }
  else if (Mode == 12) {
    ArraySize = sizeof(animSad2Right);
    AnimDelay = 30;
    MidAnimDelay = 4000;
  }

  for(int i=0; i<ArraySize; i) { // For each frame...
    Send(Mode, i);
    i+=8;
    delay(AnimDelay);
  }

  delay(MidAnimDelay);

  for(int i=ArraySize-8; i>-1; i) { // For each frame...
    Send(Mode, i);
    i-=8;
    delay(AnimDelay);
  }
}

void Send(uint8_t Mode, int i) {
  for (uint8_t k = 0; k<2; k++) {
    Wire.beginTransmission(I2C_ADDRS[k]);
    Wire.write(0);                  // Start address
    for(uint8_t j=0; j<8; j++) {    // 8 rows...
      if (Mode == 0) {
        Wire.write(pgm_read_byte(&reorder[pgm_read_byte(&animBlink[i+j])]));
      }
      else if (Mode == 1) {
        Wire.write(pgm_read_byte(&reorder[pgm_read_byte(&animLookRight[i+j])]));
      }
      else if (Mode == 2) {
        Wire.write(pgm_read_byte(&reorder[pgm_read_byte(&animLookLeft[i+j])]));
      }
      else if (Mode == 3) {
        Wire.write(pgm_read_byte(&reorder[pgm_read_byte(&animLookUp[i+j])]));
      }
      else if (Mode == 4) {
        Wire.write(pgm_read_byte(&reorder[pgm_read_byte(&animLookDown[i+j])]));
      }
      else if (Mode == 5) {
        Wire.write(pgm_read_byte(&reorder[pgm_read_byte(&animDead[i+j])]));
      }
      else if (Mode == 6) { // Wink
        if (k == 0) {
          Wire.write(pgm_read_byte(&reorder[pgm_read_byte(&Neutral[0+j])]));
        }
        else {
          Wire.write(pgm_read_byte(&reorder[pgm_read_byte(&animBlink[i+j])]));
        }
      }
      else if (Mode == 7) {
        Wire.write(pgm_read_byte(&reorder[pgm_read_byte(&animHeart[i+j])]));
      }
      else if (Mode == 8) {
        if (k == 0) {
          Wire.write(pgm_read_byte(&reorder[pgm_read_byte(&animAngryLeft[i+j])]));
        }
        else {
          Wire.write(pgm_read_byte(&reorder[pgm_read_byte(&animAngryRight[i+j])]));
        }
      }
      else if (Mode == 9) {
        Wire.write(pgm_read_byte(&reorder[pgm_read_byte(&animBlaze[i+j])]));
      }
      else if (Mode == 10) {
        if (k == 0) {
          Wire.write(pgm_read_byte(&reorder[pgm_read_byte(&animSadLeft[i+j])]));
        }
        else {
          Wire.write(pgm_read_byte(&reorder[pgm_read_byte(&animSadRight[i+j])]));
        }
      }
      else if (Mode == 11) {
        Wire.write(pgm_read_byte(&reorder[pgm_read_byte(&animManga[i+j])]));
      }
      else if (Mode == 12) {
        if (k == 0) {
          Wire.write(pgm_read_byte(&reorder[pgm_read_byte(&animSad2Left[i+j])]));
        }
        else {
          Wire.write(pgm_read_byte(&reorder[pgm_read_byte(&animSad2Right[i+j])]));
        }      
      }

      Wire.write(0);
    }
    Wire.endTransmission();
  }
}

void loop() {

  // 0 Blink
  // 
  
  float p;
  p = random(100);
  int minPost;
  int maxPost;
  Play(0);
  delay(300);
  if (p<10) {  // LookRight
     Play(1);
     minPost = 1500;
     maxPost = 3000;
   }
   else if (p<20) {  // LookLeft
     Play(2);
     minPost = 1500;
     maxPost = 3000;
   }
   else if (p<30) {  // LookUp
     Play(3);
     minPost = 1500;
     maxPost = 3000;
   }
   else if (p<40) {  // LookDown
     Play(4);
     minPost = 1500;
     maxPost = 3000;
   }
   else if (p<45) {  // Dead
     Play(5);
     minPost = 3000;
     maxPost = 5000;
   }
   else if (p<60) {  // Wink
     Play(6);
     minPost = 500;
     maxPost = 1500;
   }
   else if (p<70) {  // Heart
     for (uint8_t j =0 ; j<3; j++) {
        Play(7);
     }
     minPost = 3000;
     maxPost = 5000;
   }
   else if (p<75) {  // Manga
     for (uint8_t i = 0; i<7; i++) {
       Play(11);
     }
     minPost = 500;
     maxPost = 1000;
   }
   else if (p<80) {  // Angry
     Play(8);
     minPost = 1000;
     maxPost = 5000;
   }
   else if (p<90) {  //  Blaze
     Play(9);
     minPost = 1000;
     maxPost = 5000;
   }
   else if (p<95) {
    Play(12);
    minPost = 1000;
    maxPost = 5000;
   }
   else {  // Sad
     Play(10);
     minPost = 1000;
     maxPost = 5000;
   }

   int D = random(minPost, maxPost);
   delay(D);
  //  if (p<0.10){
//    Blink();
//  }
//  else if (p<0.20) {
//    LookRight();
//  }
//  else if (p<0.30) {
//    LookLeft();
//  }
//  else if (p<0.40) {
//    LookUp();
//  }
//  else if (p<0.50) {
//    LookDown();
//  }
//  else if (p<0.55) {
//    Dead();
//  }
//  else if (p<0.60) {
//    for (uint8_t i = 0; i<4; i++) {
//      Wink();
//    }
//  }
//  else if (p<0.70) {
//    Heart();
//  }
//  else if (p<0.75) {
//    for (uint8_t i = 0; i<10; i++) {
//      Manga();
//    }
//  }
//  else if (p<0.80) {
//    Angry();
//  }
//  else if (p<0.90) {
//    Blaze();
//  }
//  else {
//    Sad();
//  }
  
//
//  Blink();
//  LookRight();
//  LookLeft();
//  LookUp();
//  LookDown();
//  Dead();
//  Wink();
//  Heart();
//  Angry();
//  Blaze();
//  Sad();
//  for (uint8_t i = 0; i<10; i++) {
//  Manga();
//}
//
//  
//  if (p<0.10){
//    Blink();
//  }
//  else if (p<0.20) {
//    LookRight();
//  }
//  else if (p<0.30) {
//    LookLeft();
//  }
//  else if (p<0.40) {
//    LookUp();
//  }
//  else if (p<0.50) {
//    LookDown();
//  }
//  else if (p<0.55) {
//    Dead();
//  }
//  else if (p<0.60) {
//    for (uint8_t i = 0; i<4; i++) {
//      Wink();
//    }
//  }
//  else if (p<0.70) {
//    Heart();
//  }
//  else if (p<0.75) {
//    for (uint8_t i = 0; i<10; i++) {
//      Manga();
//    }
//  }
//  else if (p<0.80) {
//    Angry();
//  }
//  else if (p<0.90) {
//    Blaze();
//  }
//  else {
//    Sad();
//  }
  

//  if(!--rep) {             // If last cycle...
//    ledCmd(0x20);          // LED matrix in standby mode
//    GIMSK = _BV(PCIE);     // Enable pin change interrupt
//    power_all_disable();   // All peripherals off
//    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
//    sleep_enable();
//    sei();                 // Keep interrupts disabled
//    sleep_mode();          // Power down CPU (pin 1 will wake)
//    // Execution resumes here on wake.
//    GIMSK = 0;             // Disable pin change interrupt
//    rep   = REPS;          // Reset animation counter
//    power_timer0_enable(); // Re-enable timer
//    power_usi_enable();    // Re-enable USI
//    Wire.begin();          // Re-init I2C
//    clear();               // Blank display
//    ledCmd(0x21);          // Re-enable matrix
//  }
}

ISR(PCINT0_vect) {} // Button tap

