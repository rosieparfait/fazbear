# Freddy Fazbear Luminary

## Sole Member

Rose Rodrigues (5784539 - rodr0875)

## Description

These files contain the Particle Photon source code for a luminary shaped like the head of the fictional character Freddy Fazbear of the Five Nights at Freddy’s series, where light comes from his eyes (or more accurately, inside his head). It can turn on or off by pressing down on his nose, playing a honk at each turn. His jaw also moves proportionally to the volume of the sounds emitted by the speaker. 

It's also connected to an Alexa skill which, when prompted with “enter attack mode” (or a similar utterance), and when the light sensor doesn’t detect enough light, will begin a routine similar to the one found in the first installment of the series, where his eyes flash randomly while March of the Toreadors plays, followed by darkness intermittently riddled with footstep sounds, finally turning his lights on (for the project, it would be red, to add variety) and yelling out. The timing follows very closely the timings found in the game. Additionally, when prompted with "begin audio prompt" (or a similar utterance), you can specify one of a few stored sound files for Freddy to say. They're all mostly for fun.

## List of Files

- asfiles *(AWS Lambda Files used in Skill)*
  - **index.js** *(.js source file for the Alexa skill interpretations)*
  - node_modules *(Imported libraries)*
  - package-lock.json *(Dependencies)*
  - package.json *(Dependencies)*
  - recipes.json *(Leftover file from template)*
- ppfiles *(Particle Photon files)*
  - .vscode *(Auto-generated VS Code files)*
  - lib *(Library files)*
  - project.properties *(Listed dependencies)*
  - src *(Source files)*
    - fazbear.cpp *(Generated .cpp file for Photon)*
    - **fazbear.ino** *(Raw programmed .ino script for the project)*
  - target *(Compiler files)*

## List of Components Used *(With Adafruit PID where applicable)*

- **3968** - 4Ω 3W 40mm Speaker
- **254** - MicroSD Card Breaker Board
- **2130** - PAM8302 Adafruit Mono 2.5W Class D Audio Amp
- **HS-311** Servo Motor
- **WS2812** NeoPixel iLED
- 10kΩ LDR Photoresistor
- Particle Photon Board

## Circuit Connections

[Connections](/readmeassets/fr.png)