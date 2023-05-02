/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/Users/rickierha/Projects/Particle/ee1301/fazbear/src/fazbear.ino"
/*
 * FREDDY FAZBEAR LAMP
 * Just a silly little lamp
 * Rose Rodrigues - RODR0875 (5784539)
 * EE 1301 Spring 2023
 */

#include <particle.h>
#include <speaker.h>
#include <cstring>
#include <SdFat.h>
#include <neopixel.h>

double lerp(double a, double b, double t);
void setup();
void loop();
int attackMode(const char* junk);
int selectFile(const char* filename);
int readChunk();
#line 14 "/Users/rickierha/Projects/Particle/ee1301/fazbear/src/fazbear.ino"
SerialLogHandler logHandler(LOG_LEVEL_NONE, {   // Logging level for non-app messages
    { "app", LOG_LEVEL_INFO }                   // Logging level for application messages
}); 

// MicroSD Card Variables

typedef struct wav_header {
  // RIFF Header
  char     riff_header[4];   // Contains "RIFF"
  uint32_t wav_size;         // Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
  char     wave_header[4];   // Contains "WAVE"
    
  // Format Header
  char     fmt_header[4];    // Contains "fmt " (includes trailing space)
  uint32_t fmt_chunk_size;   // Should be 16 for PCM
  uint16_t audio_format;     // Should be 1 for PCM. 3 for IEEE Float
  uint16_t num_channels;
  uint32_t sample_rate;
  uint32_t byte_rate;        // Number of bytes per second. sample_rate * num_channels * Bytes Per Sample
  uint16_t sample_alignment; // num_channels * Bytes Per Sample
  uint16_t bit_depth;        // Number of bits per sample
    
  // Data
  char     data_header[4];   // Contains "data"
  uint32_t data_bytes;       // Number of bytes in data. Number of samples * num_channels * sample byte size
  // uint8_t bytes[];        // Remainder of wave file is bytes
} WavHeader_t;

const int    SD_SS      = A2; 
const size_t BUFFERSIZE = 1024;

// Speaker Vars

uint16_t     data[2][BUFFERSIZE];

SdFat        sd;
File         wavFile;
WavHeader_t  wh;

Speaker speaker(data[0], data[1], BUFFERSIZE);

String testing;

// Neopixel Vars

int pixelPin = D4;
int pixelNum = 1;
int pixelModel =  WS2812;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(pixelNum, pixelPin, pixelModel);

// Servo Vars
Servo FreddyJaw;

// Light Sensor Vars
int LSPin = A1;

// Custom Vars

const int FREDDY_NOSE_PIN = D5;
bool FreddyNoseState = 0, oldFreddyNoseState = 0;
bool FreddyLightOn = true, FreddyAttackMode = false;
float soundVolume = 0; int FreddyAttackState = 0;
long int whenToActivate = 0, lastMillis = 0; int FreddyAttackLight = 0x000000;
double currentAngle = 0;

double lerp(double a, double b, double t)
{
  double res = a + ((b - a) * t);
  if (isnan(a) || isnan(b)) {b = 5;}
  if (isnan(res)) {res = b;}
  return res;
}

// setup() runs once, when the device is first turned on.
void setup() {
  Serial.begin(9600);

  // Sets up speaker
  uint16_t audioFrequency = 44100;
  speaker.begin(audioFrequency);

  Particle.function("playWav", selectFile);
  Particle.function("atkMode", attackMode);

  // Sets up microSD card reader

  if (sd.begin(SD_SS)) 
    Log.info("SD initialised");
  else
    Log.warn("failed to open card");

  // Neopixel & servo setup
  strip.begin(); FreddyJaw.attach(D0);

  // PIN Setup

  pinMode(FREDDY_NOSE_PIN, INPUT_PULLDOWN);

  whenToActivate = rand() % 100000 + 5000;
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  FreddyNoseState = digitalRead(FREDDY_NOSE_PIN);

  // Speaker thing
  if(speaker.ready())
  {
    readChunk();
  }


  if(!FreddyAttackMode) // Regular headlamp functionality
  {
    double angleToAimFor = wavFile.isOpen() ? soundVolume / 20.0 : 5.0;
    Log.printf("%f // %i // %f\n", angleToAimFor, wavFile.isOpen(), currentAngle);
    currentAngle = lerp(currentAngle, angleToAimFor, (millis() - lastMillis)/70.0);
    if (isnan(currentAngle)) currentAngle = angleToAimFor;
    FreddyJaw.write(currentAngle);
    // Freddy button press
    if(FreddyNoseState == HIGH && oldFreddyNoseState == LOW)
    {
      selectFile("honk.wav");
      FreddyLightOn = !FreddyLightOn;
    }

    // Laughs out of nowhere >:)
    if(millis() > whenToActivate)
    {
      int index = (rand() % 31)/10;
      char laughPath[] = "laughN.wav";
      char fileName[7];
      strcpy(fileName, "laugh");
      strncat(fileName, itoa(index, new char[2], 10), 1);
      strncpy(laughPath, fileName, 6);
      selectFile(laughPath);
      whenToActivate += rand() % 100000 + 5000;
    }

    // Light up based on things
    strip.setPixelColor(0, (FreddyLightOn ? FreddyAttackLight : 0x000000));
  }
  else  // Attack mode functionality - overrides everything until done
  {
    if(millis() > whenToActivate)
    {
      switch(FreddyAttackState)
      {
        case 0:
          FreddyJaw.write(0);
          FreddyLightOn = false;
          selectFile("powerdown.wav");
          whenToActivate = millis() + 5000 + (rand() % 4) * 5000;
          FreddyAttackState++;
          break;

        case 1:
          selectFile("toreador.wav");  
          FreddyLightOn = true; FreddyAttackLight = 0xFFFFFF;
          whenToActivate = millis() + 5000 + (rand() % 4) * 5000;
          FreddyAttackState++;
          break;

        case 2:
          selectFile("flicker.wav"); FreddyLightOn = false;
          whenToActivate = millis() + 2000 + (rand() % 4000);
          FreddyAttackState++;
          break;

        case 3:
          selectFile("footsteps.wav");
          whenToActivate = millis() + 2000 + (rand() % (rand() % 8 + 2)) * 2000;
          FreddyAttackState++;
          break;

        case 4:
          selectFile("scream.wav");          
          FreddyAttackState++;
          whenToActivate = millis() + 360;
          break;

        case 5:
          FreddyLightOn = true; FreddyAttackLight = 0x00FF00;
          FreddyJaw.write(40);
          whenToActivate = millis() + 1000;
          FreddyAttackState++;
          break;

        case 6:
          FreddyAttackState++;
          FreddyJaw.write(0);
          selectFile("static.wav");
          FreddyLightOn = false;
          whenToActivate = millis() + 10000;
          break;

        case 7:
          FreddyAttackState = 0;
          FreddyAttackMode = false;
          FreddyLightOn = true; FreddyAttackLight = 0xFFFFFF;
          whenToActivate = millis() + (rand() % 100000 + 5000);
          break;
      }
    }
    strip.setPixelColor(0, (FreddyLightOn ? FreddyAttackLight : 0x000000));
    if(FreddyAttackState == 2 && !(rand() % 10)) {FreddyLightOn = !FreddyLightOn;}
  }

  strip.show();
  oldFreddyNoseState = FreddyNoseState;
  lastMillis = millis();
}

// Only attack if it's dark!
int attackMode(const char* junk)
{
  Log.printf("Attempted attack at brightness %i\n", analogRead(LSPin));

  int ret = 0;
  if(!FreddyAttackMode && analogRead(LSPin) > 1000)
  {
    FreddyAttackMode = ret = true;
    FreddyAttackState = whenToActivate = 0;
  }
  else if(!FreddyAttackMode)
  {
    selectFile("block.wav");
  }
  return ret;
}

// Everything below was taken from the example folder of the speaker library. It involves memory manipulation and wav file reading, so for a 1000-level course I think it's okay not to handwrite this part manually <3 not that i would know how to do that o_o;;
// I modified a bit of the readChunk() thing to make Freddy's jaw move using RMS loudness to get volume, and some post-processing to make it smoother 

int selectFile(const char* filename) {
  int retVal = 0;
  
  if (!strcmp("ls", filename) || !strcmp("dir", filename)) {
    sd.ls("/", LS_R);
    return 0;
  }
  
  if (wavFile.isOpen()) wavFile.close();

  if(!strcmp(filename, "laugh3.wav")) FreddyAttackLight = 0xFFFF00;
  
  wavFile = sd.open(filename);

  if (wavFile) {
    memset((uint8_t*)data, 0x80, sizeof(data)); // reset buffer to bias value 0x8080 (quicker via memset() than writing 0x8000 in a loop)
    if (sizeof(wh) == wavFile.read((uint8_t*)&wh, sizeof(wh))) {
      retVal = wh.data_bytes;
      readChunk();
      speaker.begin(wh.sample_rate);
      Log.printf("Playing %s", filename);
    }
  } 
  else {
    Log.error("%s not found", filename);
  }

  return retVal;
}

int readChunk() {
  int retVal = 0;
  if (wavFile.isOpen()) {
    float meanSquareValue = 0;
    uint16_t* wav = speaker.getBuffer();
    uint8_t   buf[BUFFERSIZE * 2 * wh.num_channels];
    int       n = wavFile.read(buf, sizeof(buf));
    if (n < sizeof(buf)) wavFile.close();               // when all data is read close the file
    
    memset((uint8_t*)wav, 0x80, BUFFERSIZE);            // reset buffer to bias value 0x8080


    for(int b = 0; b < n; b += wh.sample_alignment) {
      wav[retVal++] = *(uint16_t*)&buf[b] + 32768;      // convert int16_t to uin16_t with bias 0x8000
      meanSquareValue += ((int16_t)(wav[retVal-1]))*((int16_t)(wav[retVal-1]));
    }
    

    meanSquareValue = sqrt(meanSquareValue);
    soundVolume = sqrt(1048525 - meanSquareValue);
  }
  else
  {
    speaker.end();
    FreddyAttackLight = 0xFFFFFF; // Reset special laugh light, just in case
  }

  return retVal;
}
