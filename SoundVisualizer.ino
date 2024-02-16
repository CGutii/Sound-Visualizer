// Cristian Gutierrez
// Sound Visualizer (SHPE)
// 2/17/24

#include "LedControl.h"

// Initialize the LED matrix using the LedControl library
LedControl ledMatrix = LedControl(12, 10, 11, 1);

// Maximum value for scaling the LED display brightness
const int maxBrightnessScale = 11;

// Pin connected to the sound sensor
const int soundSensorPin = A0;
// Time window for sampling sound signals (50ms corresponds to a 20Hz sampling rate)
const int samplingWindow = 50;
// Variable to store the sound sample
unsigned int soundSample;

// Variables to track the start time of sampling and the duration of each cycle
unsigned long samplingStartTime;
unsigned long cycleDuration;

// Variables to store the maximum and minimum sound levels detected in a sampling window
unsigned int maxSoundLevel = 0;
unsigned int minSoundLevel = 1024;
// Index for iterating through arrays
unsigned char arrayIndex = 0;

// Arrays to store peak-to-peak values and their display equivalents for 8 cycles
unsigned int peakToPeakValues[8];
unsigned int displayPeakValues[8];
// Temporary array to assist in displaying dropping LED levels
unsigned int tempDisplayValues[8] = {0,0,0,0,0,0,0,0};
// Buffers to store maximum and minimum sound levels for each cycle
unsigned int maxSoundBuffer[8];
unsigned int minSoundBuffer[8];

void setup() {
  // Initialize the LED matrix display
  ledMatrix.shutdown(0, false); // Turn on the display
  ledMatrix.setIntensity(0, 1); // Set brightness level
  ledMatrix.clearDisplay(0); // Clear the display

  // Initialize serial communication
  Serial.begin(9600);
}

void loop() {
  // Record the start time of the sampling
  samplingStartTime = millis();

  // Reset sound level extremes for the new sampling window
  maxSoundLevel = 0;
  minSoundLevel = 1024;
  
  // Collect sound samples over the defined sampling window
  while (millis() - samplingStartTime < samplingWindow) {
    soundSample = analogRead(soundSensorPin);
    
    // Update maximum and minimum sound levels
    if (soundSample < 1024) {
      if (soundSample > maxSoundLevel) {
        maxSoundLevel = soundSample;
      }
      if (soundSample < minSoundLevel) {
        minSoundLevel = soundSample;
      }
    } 

    // Store max and min sound levels at predefined intervals within the sampling window
    cycleDuration = millis() - samplingStartTime;
    if (cycleDuration == 1 || cycleDuration == 2 || cycleDuration == 4 || cycleDuration == 8 
        || cycleDuration == 16 || cycleDuration == 32 || cycleDuration == 40 || cycleDuration == 50) {
              maxSoundBuffer[arrayIndex] = maxSoundLevel;
              minSoundBuffer[arrayIndex] = minSoundLevel;
              arrayIndex = (arrayIndex + 1) % 8;
              delay(1);
    }
  }

  // Reset the array index for the next loop iteration
  arrayIndex = 0;

  // Process the collected sound samples
  for (int i = 0; i < 8; i++) {
    // Calculate the peak-to-peak value
    peakToPeakValues[i] = maxSoundBuffer[i] - minSoundBuffer[i];
    
    // Scale the peak-to-peak value for display
    displayPeakValues[i] = map(peakToPeakValues[i], 0, 1023, 0, maxBrightnessScale);

    // Display the scaled value on the LED matrix
    displayLed(displayPeakValues[i], i);
    
    // Implement a dropping effect for the LED levels
    if (displayPeakValues[i] >= tempDisplayValues[i]) {
      tempDisplayValues[i] = displayPeakValues[i];
    }
    else {
      tempDisplayValues[i]--;
    }
    
    // Update the LED matrix with the new level
    ledMatrix.setLed(0, i, tempDisplayValues[i], true);
    delayMicroseconds(250);
  }
}

// Function to display the sound level on a specific row of the LED matrix
void displayLed(int peakLevel, int row) {
  // Select the pattern to display based on the peak level
  switch (peakLevel) {
    case 0 : ledMatrix.setRow(0, row, 0x80); break;
    case 1 : ledMatrix.setRow(0, row, 0xC0); break;
    case 2 : ledMatrix.setRow(0, row, 0xE0); break;
    case 3 : ledMatrix.setRow(0, row, 0xF0); break;
    case 4 : ledMatrix.setRow(0, row, 0xF8); break;
    case 5 : ledMatrix.setRow(0, row, 0xFC); break;
    case 6 : ledMatrix.setRow(0, row, 0xFE); break;
    case 7 : ledMatrix.setRow(0, row, 0xFF); break;
  }
}
