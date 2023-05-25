// rise and shine sunrise
#include <Adafruit_NeoPixel.h>
#include <math.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define LED_PIN    1

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 28
// Color temperature range (K)
#define K_LOW  1500
#define K_HIGH 4000
// The brightness curve shape from 'sunrise brightness.xlsx'
// Lower numbers give a faster curve.
#define BRIGHT_CURVE  675

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)


// setup() function -- runs once at startup --------------------------------

void setup()
{
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.
  //Serial.begin(9600);      // for debug
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(255); // Set BRIGHTNESS to max
} // end setup


// loop() function -- runs repeatedly as long as board is on ---------------

void loop()
{
  float x;
  float y;
  float i;
  int kx;
  float fullbright;
  float brightmath;

  for (int k=0; k<5000; k++)
  {
    // a logarigthmic brigtness curve defined in 'sunrise brightness.xlsx'
    fullbright = logf((float)(5000+BRIGHT_CURVE)/BRIGHT_CURVE);
    brightmath = logf((float)(k+BRIGHT_CURVE)/BRIGHT_CURVE) / fullbright * 255;
    if (brightmath < 1) brightmath = 1;
    if (brightmath > 255) brightmath = 255;
    strip.setBrightness((int) brightmath);
    // change color every other brightness change for 2500 steps total
    kx = k/2 + 1500;

    // circular brightness pattern to mix with the overall brigntness of the strip
    for(int pixel=0; pixel<15; pixel++)
    {
      // formula for a circle of radius 15
      x = pixel + 0.5; //even pixels means never start at x=0
      y = (float) sqrt( (double)(sq(15)-sq(x)) );
      i = (15-y)*2 + 1;
      strip.setPixelColor((13+pixel), KtoPixelColor(kx,i,0));
      strip.setPixelColor((14-pixel), KtoPixelColor(kx,i,1));
    }
    strip.show();

    delay(120); // 120 ms * 5000 steps = 10 minutes

  } // next color temperature

  while (1) {} // infinite do-nothing loop

} // end main program loop

uint32_t KtoPixelColor(int32_t color_temp, float intensity, int blue_block)
{
  uint32_t red;
  uint32_t green;
  uint32_t blue;
  float tmp_internal;
  float tmp_red;
  float tmp_blue;
  float tmp_green;

  tmp_internal = color_temp / 100.0;

  // red
  if (color_temp <= 6600)
  {
      tmp_red = 255;
  }
  else
  {
      tmp_red = 329.698727446 * pow((tmp_internal - 60), -0.1332047592);
      if (tmp_red < 0)
      {
        tmp_red = 0;
      }
      else if(tmp_red > 255)
      {
        tmp_red = 255;
      }
  }
  red = (int)(tmp_red / intensity);

  // green
  if (color_temp <= 6600)
  {
    tmp_green = 99.4708025861 * log(tmp_internal) - 161.1195681661;
    if (tmp_green < 0)
    {
      tmp_green = 0;
    }
    else if (tmp_green > 255)
    {
      tmp_green = 255;
    }
  }
  else
  {
    tmp_green = 288.1221695283 * pow((tmp_internal - 60), -0.0755148492);
    if (tmp_green < 0)
    {
      tmp_green = 0;
    }
    else if (tmp_green > 255)
    {
      tmp_green = 255;
    }
  }
  green = (int)(tmp_green/intensity);

  //blue
  if (blue_block > 0)
  {
    if (color_temp>4000)
    color_temp = 4000;    // remove some blue if needed
    tmp_internal = color_temp/100;
  }

  if (color_temp >= 6600)
  {
    tmp_blue = 255;
  }
  else if (color_temp <= 1904)
  {
    tmp_blue = 0;
  }
  else
  {
    tmp_blue = 138.5177312231 * log(tmp_internal - 10) - 305.0447927307;
    if (tmp_blue < 0)
    {
      tmp_blue = 0;
    }
    else if (tmp_blue > 255)
    {
      tmp_blue = 255;
    }
  }
  blue = (int)(tmp_blue/intensity);

  return(strip.Color(red, green, blue));
}
