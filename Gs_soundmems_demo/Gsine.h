#ifndef GSINE_H
#define GSINE_H

void Gsine() {

// Persistent local variable.
  static int thisphase = 0;                                                     // Phase change value gets calculated.

// Local variables. Play around with these.
  uint8_t allfreq = 21;                                                         // G! originally 32 //You can change the frequency, thus distance between bars. Wouldn't recommend changing on the fly.
  uint8_t thiscutoff = 66;                                                     // G! originally 192 //You can change the cutoff value to display this wave. Lower value = longer wave.

  uint8_t bgbright = 10;                                                        // Brightness of background colour.
  uint8_t colorIndex;
  
  timeval = 30;                                                                 // Our EVERY_N_MILLIS_I timer value.

  thiscutoff = beatsin8(12,64, 224);                                            //G! Originally was commented out original beatsin8(12,64,224)

//  thiscutoff = 255 - sampleavg;                                            //G! commented out
  
  
//  thisphase += sampleavg/2;                                                     // Move the sine waves along as a function of sound.

  thisphase = beatsin16(99,-600, 600);                                            //G! originally 20, -600, 600

  
  colorIndex = millis() >> 4;                                                   // G! Originally >> 4 // millis() can be used for so many things.
  
  for (int k=0; k<NUM_LEDS; k++) {                                            // For each of the LED's in the strand, set a brightness based on a wave as follows:
    int thisbright = qsuba(cubicwave8((k*allfreq)+thisphase), thiscutoff);      // qsub sets a minimum value called thiscutoff. If < thiscutoff, then bright = 0. Otherwise, bright = 128 (as defined in qsub)..
    leds[k] = ColorFromPalette( currentPalette, colorIndex, thisbright, currentBlending);    // Let's now add the foreground colour. By Andrew Tuline.
    colorIndex +=3;
  }
  
  addGlitter(sampleavg/2);                                                      // Add glitter based on sampleavg. By Andrew Tuline.
  
} // Gsine()

#endif
