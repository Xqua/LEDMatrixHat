# How to give life to a hat, a journey into 5kb of memory.

## Intro and inspiration
Ok so it all started with Sabo. I am a big One Piece fan, and Sabo is probably my favorite character (RIP Ace ... you were the one before ! T_T ) 
![](https://i.imgur.com/Ab2LhGq.png =250x)

Then, it there was a steam punk party and I got a top hat and some goggles and this happened:
![](https://i.imgur.com/T1A1PwH.png =250x)

Then it was a matter of time before Big Mom's hat took life, infused by Big Mom's soul. So well, I though, I'd give it a try, took some of my soul, and put it into the hat. And behold it was alive !
![](https://i.imgur.com/MI5iBLt.png =250x)

But more than this, it was really alive

[![Video](https://img.youtube.com/vi/lCmvI1q-8ME/0.jpg)](https://www.youtube.com/watch?v=lCmvI1q-8ME)
https://youtu.be/lCmvI1q-8ME

## The components

Crafts
- A stylish hat (cheap in thrift stores ;) )
- Some goggles, steam punk or not ! you can make this a bro hat with a cap and sunglasses ;)
- A glue gun

Electronics
- 2x 8x8 LED matrix. I used the adafruit mini white, but you can look for other and thye could work better! https://www.adafruit.com/product/1080
- 1x trinket 5V. https://www.adafruit.com/product/1501
- 1x ~5V Battery pack. This can be a LiCell or some old AAA as long as it delivers between 4.5 and 5V. I used this: https://www.adafruit.com/product/727
- Some wires to connect things
- A soldering Iron
- Some AAA batteries ! (I was out when I started! What a mistake XD)

Total cost: ~45-70$ depending on the hat and goggles !

That's it ! super easy no ?

## Make some holes 

I used my soldering iron to poke 4 holes per matrix a bit on the left and right of each matrix. That way the wires can go out of the matrix and in the hat, where we will solder it all to the trinket.

You can use any punch technic depending on your hat. But the idea is that the wire should run behind the glasses so as to not be apparent. (though that's a style so go as you like !)

## Solder it all 

### I2C Madness

Alright so now we have our wire our holes and our components ready, the first thing we want to do is to change the I2C address of one of the Matrix. If you don't know how I2C work, well, you don't have to ! Just know that each device on an I2C chain has a unique address (like a IP address), and that you send a ping to that address to listen (kinda like an header in a packet), and then your instructions. 
So here we have 2 matrix, so we need to direct our signal at each independently (right eye != left eye). Matrix 1 will have the default address 0x70 and matrix 2 a different one 0x71.
The problem is that, the address is electronic, so we need to change it by soldering together a jumper pad (instructions here: https://learn.adafruit.com/adafruit-led-backpack/changing-i2c-address)
![](https://i.imgur.com/QfpdwPX.png)

We want to drop solder accross the A0 pads, that way it will increase the address by 1 (0x70+1 => 0x71)

### Get the wires where they belong.
Alright so we'll basically follow this schematic 
![](https://i.imgur.com/mbgvuZ1.png)

Which is basically an adaptation of the Space Invader schematic from adafruit: https://learn.adafruit.com/trinket-slash-gemma-space-invader-pendant/wirin

**DON'T FORGET TO RUN THE WIRE THROUGH THE HAT BEFORE YOU SOLDER !**
**DON'T FORGET TO RUN THE WIRE THROUGH THE HAT BEFORE YOU SOLDER !**
**DON'T FORGET TO RUN THE WIRE THROUGH THE HAT BEFORE YOU SOLDER !**

## Programming 

The next challenge is to program this trinket. For me it was the first time working with one of thoose chips. I've done some arduino before, but I never had to think about memory space! 
Basically, I had wrote the whole code, and literaly the last line put it over the memory limit... -_- Alright 2h30 min later I had refactored the code so that it would fit. 

So what does this do? Well we define our animations (I made them using LibreOffice, conditional formating, copy paste into atom for cleanup.) and we store them into long byte array (basicaly a char array) that look like this:
```
static const uint8_s PROGMEM anim = {
B00011000,
B00100100,
B01000010,
B10000001,
B10000001,
B01000010,
B00100100,
B00011000
}
```
Notice the PROGMEM ? Well that's to avoid overloading our memory with thoose ! So they are stored in the ROM instead of the RAM. 
As you can see, it is basically a 1 for the pixel on and a 0 for the pixel off. 

Then the rest of the code is basically an adaptation of: https://learn.adafruit.com/trinket-slash-gemma-space-invader-pendant/source-code

With the main part of the code being:
```  
for(int i=0; i<sizeof(anim); i) { // For each frame...
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(0);                  // Start address
    for(uint8_t j=0; j<8; j++) {    // 8 rows...
        Wire.write(pgm_read_byte(&reorder[pgm_read_byte(&anim[i++])]));
        Wire.write(0);
    }
    Wire.endTransmission();
    delay(10);
}
```

Then the final touch up was to add some randomness into this, so that it would play a different animation randomly, and with blinks of the eyes to make it more lively. 

The sketch is in the folder: XXXXXXXXXXXXX
