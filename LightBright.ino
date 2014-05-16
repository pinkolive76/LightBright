// 'Light Bright' LED rainbow sneakers sketch by Brigit Lyons
// This is a highly modified version of the 'Firewalker' LED sneakers sketch for 
//   Adafruit NeoPixels by Phillip Burgess

#include <Adafruit_NeoPixel.h>
#include <math.h>

// LEDs go around the full perimeter of the shoe sole, but the step animation
// is mirrored on both the inside and outside faces, while the strip doesn't
// necessarily start and end at the heel or toe.  These constants help configure
// the strip and shoe sizes, and the positions of the front- and rear-most LEDs.
// Brigit's shoes: 39 LEDs total, 20 LEDs along the outside, LED #7 at heel, LED #27 at toe.
#define N_LEDS        39 // TOTAL number of LEDs in strip
#define HALF_STRIP    20 // cieling(N_LEDS/2), only used for LED animation logic
#define OUTSIDE_LEN   20 // Number of LEDs on the OUTSIDE of shoe (from pixels HEEL to TOE-1)
#define INSIDE_LEN    19 // Number of LEDs on the INSIDE of shoe (from pixels HEEL to TOE-1)
#define HEEL           7 // Index of REAR-MOST LED on shoe (remember, the first LED is #0)
#define TOE           27 // Index of TOP-MOST LED on shoe
#define STEP_PIN       9 // Analog input for footstep
#define LED_PIN        6 // LED strip is connected here
#define MAXSTEPS       3 // Process (up to) this many concurrent steps

// The readings from the sensors are usually around xxx when not being pressed,
// then dip below 50 when the heel is standing on it. Depending on you weight,
// readings may differ, so adjust as needed.
#define STEP_TRIGGER    50 // Reading must be below this to trigger step
#define STEP_HYSTERESIS 75 // After trigger, must return to this level

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

int
  stepMag[MAXSTEPS],        // Magnitude of steps
  stepFiltered,             // Current filtered pressure reading
  stepMin,                  // Minimum reading during current step                 
  outsideLEDs[OUTSIDE_LEN], // A list of the outside LEDs (from heel to toe-1)
  insideLEDs[INSIDE_LEN];   // A list of the inside LEDs (from heel-1 to toe)
uint8_t
  colorArray[HALF_STRIP][3],// What colors we want the LEDs to display
  stepNum = 0;              // Current step number in stepMag table
boolean
  stepping  = false;        // If set, step was triggered, waiting to release

/////////////////////////////////////////////////////////////////////////////////////
// Initialization functions
/////////////////////////////////////////////////////////////////////////////////////

void init_colorArray() {
  // calculate a nice rainbow of hues using sine
  double freq = .3;
  uint8_t i, r, g, b;
  for(i=0; i<HALF_STRIP; i++) {
    colorArray[i][0] = sin(freq*i + 0) * 127 + 128;
    colorArray[i][1] = sin(freq*i + 2) * 127 + 128;
    colorArray[i][2] = sin(freq*i + 4) * 127 + 128;
  }
}

void init_outsideLEDs() {
  int count = 0;
  for(int i=HEEL; i<TOE; i++) {
    if(count > OUTSIDE_LEN) return;
    outsideLEDs[count++] = i;
  }
}

void init_insideLEDs() {
  int count = 0;
  // from heel-1 to the pixel 0
  for(int i=HEEL-1; i>=0; i--) {
    if(count > INSIDE_LEN) return;
    insideLEDs[count++] = i;
  }
  // from the last pixel to the toe
  for(int i=N_LEDS-1; i>=TOE; i--) {
    if(count >= INSIDE_LEN) return;
    insideLEDs[count++] = i;
  }
}

/////////////////////////////////////////////////////////////////////////////////////
// Animation functions
/////////////////////////////////////////////////////////////////////////////////////

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<HALF_STRIP; i++) {
    if(i < INSIDE_LEN) strip.setPixelColor(insideLEDs[i], c);
    if(i < OUTSIDE_LEN) strip.setPixelColor(outsideLEDs[i], c);
    strip.show();
    delay(wait);
  }
}

void wipeOff(uint8_t wait) {
  colorWipe(strip.Color(0, 0, 0), wait);
}

void colorArrayWipe(uint8_t wait) {
  for(uint16_t i=0; i<HALF_STRIP; i++) {
    if(i < INSIDE_LEN)
      strip.setPixelColor(insideLEDs[i], colorArray[i][0], colorArray[i][1], colorArray[i][2]);
    if(i < OUTSIDE_LEN)
      strip.setPixelColor(outsideLEDs[i], colorArray[i][0], colorArray[i][1], colorArray[i][2]);
    strip.show();
    delay(wait);
  }
}

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  //Serial.println("setup");
  pinMode(9, INPUT_PULLUP); // Set internal pullup resistor for sensor pin

  init_colorArray();
  init_outsideLEDs();
  init_insideLEDs();

  // Clear step magnitude buffer
  memset(stepMag, 0, sizeof(stepMag));
  strip.begin();
  stepFiltered = analogRead(STEP_PIN); // Initial input
}

void loop() {
  uint8_t i;

  // Read analog input, with a little noise filtering
  stepFiltered = ((stepFiltered * 3) + analogRead(STEP_PIN)) >> 2;
  Serial.print("sensor = ");
  Serial.println(stepFiltered);
  //Serial.print("Step trigger = ");
  //Serial.println(STEP_TRIGGER);

  // The strip doesn't simply display the current pressure reading.  Instead,
  // there's a bit of an animated flourish from heel to toe.  This takes time,
  // and during quick foot-tapping there could be multiple step animations
  // 'in flight,' so a short list is kept.
  if(stepping) { // If a step was previously triggered...
    //Serial.println("A step was previously triggered");
    if(stepFiltered >= STEP_HYSTERESIS) { // Has step let up?
      //Serial.println("The step has let up");
      stepping = false;                   // Yep! Stop monitoring.
      // Add new step to the step list (may be multiple in flight)
      stepMag[stepNum] = stepMin;//(STEP_HYSTERESIS - stepMin) * 6; // Step intensity
      if(++stepNum >= MAXSTEPS) stepNum = 0; // If many, overwrite oldest
    } 
    else if(stepFiltered < stepMin) stepMin = stepFiltered; // Track min val
  } 
  else if(stepFiltered < STEP_TRIGGER) { // No step yet; watch for trigger
    //Serial.println("A Step!");
    stepping = true;         // Got one!
    stepMin  = stepFiltered; // Note initial value
  }
  
  for(i=0; i<MAXSTEPS; i++) {     // For each step in the buffer...
    if(stepMag[i] <= 0) {
      //Serial.println("No active step");
      //wipeOff(25);
      continue; // Skip if inactive
    }
    Serial.println("ACTIVE STEP");
    colorArrayWipe(25);
    wipeOff(25);
    stepMag[i] = 0;
  }
  
  delayMicroseconds(1500);
}
