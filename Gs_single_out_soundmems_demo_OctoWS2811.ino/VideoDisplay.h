#ifndef VideoDisplay_H
#define VideoDisplay_H

// The actual arrangement of the LEDs connected to this Teensy 3.0 board.
// LED_HEIGHT *must* be a multiple of 8.  When 16, 24, 32 are used, each
// strip spans 2, 3, 4 rows.  LED_LAYOUT indicates the direction the strips
// are arranged.  If 0, each strip begins on the left for its first row,
// then goes right to left for its second row, then left to right,
// zig-zagging for each successive row.
#define LED_WIDTH      150   // number of LEDs horizontally
#define LED_HEIGHT     8   // number of LEDs vertically (must be multiple of 8)
// #define LED_LAYOUT     1    // 0 = even rows left->right, 1 = even rows right->left // G! commented, not using Zig Zag
#define NUM_LEDS      2400  // G! total number of LEDs in matrix (i.e. 150 LEDs x 8 strips = 1200

// The portion of the video image to show on this set of LEDs.  All 4 numbers
// are percentages, from 0 to 100.  For a large LED installation with many
// Teensy 3.0 boards driving groups of LEDs, these parameters allow you to
// program each Teensy to tell the video application which portion of the
// video it displays.  By reading these numbers, the video application can
// automatically configure itself, regardless of which serial port COM number
// or device names are assigned to each Teensy 3.0 by your operating system.
/*
// G! Stand alone Octo/Teensy Board
#define VIDEO_XOFFSET  0
#define VIDEO_YOFFSET  0       // display entire image
#define VIDEO_WIDTH    100
#define VIDEO_HEIGHT   100
*/ 
// G! Octo/Teensy Board #1
#define VIDEO_XOFFSET  0
#define VIDEO_YOFFSET  0     // display upper half
#define VIDEO_WIDTH    100
#define VIDEO_HEIGHT   50

// G! Octo/Teensy Board #2
#define VIDEO_XOFFSET1  0
#define VIDEO_YOFFSET1  50    // display lower half
#define VIDEO_WIDTH1    100
#define VIDEO_HEIGHT1   50

// Loop statement from the OctoWS2811 VideoDisplay.ino example
// Moved into .h file by G! a.k.a. RokGoblin
void VideoDisplay() {
	//
	// wait for a Start-Of-Message character:
	//
	//   '*' = Frame of image data, with frame sync pulse to be sent
	//         a specified number of microseconds after reception of
	//         the first byte (typically at 75% of the frame time, to
	//         allow other boards to fully receive their data).
	//         Normally '*' is used when the sender controls the pace
	//         of playback by transmitting each frame as it should
	//         appear.
	//   
	//   '$' = Frame of image data, with frame sync pulse to be sent
	//         a specified number of microseconds after the previous
	//         frame sync.  Normally this is used when the sender
	//         transmits each frame as quickly as possible, and we
	//         control the pacing of video playback by updating the
	//         LEDs based on time elapsed from the previous frame.
	//
	//   '%' = Frame of image data, to be displayed with a frame sync
	//         pulse is received from another board.  In a multi-board
	//         system, the sender would normally transmit one '*' or '$'
	//         message and '%' messages to all other boards, so every
	//         Teensy 3.0 updates at the exact same moment.
	//
	//   '@' = Reset the elapsed time, used for '$' messages.  This
	//         should be sent before the first '$' message, so many
	//         frames are not played quickly if time as elapsed since
	//         startup or prior video playing.
	//   
	//   '?' = Query LED and Video parameters.  Teensy 3.0 responds
	//         with a comma delimited list of information.
	//
	int startChar = Serial.read();

	if (startChar == '*') {
		// receive a "master" frame - we send the frame sync to other boards
		// the sender is controlling the video pace.  The 16 bit number is
		// how far into this frame to send the sync to other boards.
		unsigned int startAt = micros();
		unsigned int usecUntilFrameSync = 0;
		int count = Serial.readBytes((char*)&usecUntilFrameSync, 2);
		if (count != 2) return;
		count = Serial.readBytes((char*)drawingMemory, sizeof(drawingMemory));
		if (count == sizeof(drawingMemory)) {
			unsigned int endAt = micros();
			unsigned int usToWaitBeforeSyncOutput = 100;
			if (endAt - startAt < usecUntilFrameSync) {
				usToWaitBeforeSyncOutput = usecUntilFrameSync - (endAt - startAt);
			}
			digitalWrite(12, HIGH);
			pinMode(12, OUTPUT);
			delayMicroseconds(usToWaitBeforeSyncOutput);
			digitalWrite(12, LOW);
			// WS2811 update begins immediately after falling edge of frame sync
			digitalWrite(13, HIGH);
//			leds.show();
        FastLED.show();
			digitalWrite(13, LOW);
		}

	}
	else if (startChar == '$') {
		// receive a "master" frame - we send the frame sync to other boards
		// we are controlling the video pace.  The 16 bit number is how long
		// after the prior frame sync to wait until showing this frame
		unsigned int usecUntilFrameSync = 0;
		int count = Serial.readBytes((char*)&usecUntilFrameSync, 2);
		if (count != 2) return;
		count = Serial.readBytes((char*)drawingMemory, sizeof(drawingMemory));
		if (count == sizeof(drawingMemory)) {
			digitalWrite(12, HIGH);
			pinMode(12, OUTPUT);
			while (elapsedUsecSinceLastFrameSync < usecUntilFrameSync) /* wait */;
			elapsedUsecSinceLastFrameSync -= usecUntilFrameSync;
			digitalWrite(12, LOW);
			// WS2811 update begins immediately after falling edge of frame sync
			digitalWrite(13, HIGH);
//      leds.show();
        FastLED.show();
			digitalWrite(13, LOW);
		}

	}
	else if (startChar == '%') {
		// receive a "slave" frame - wait to show it until the frame sync arrives
		pinMode(12, INPUT_PULLUP);
		unsigned int unusedField = 0;
		int count = Serial.readBytes((char*)&unusedField, 2);
		if (count != 2) return;
		count = Serial.readBytes((char*)drawingMemory, sizeof(drawingMemory));
		if (count == sizeof(drawingMemory)) {
			elapsedMillis wait = 0;
			while (digitalRead(12) != HIGH && wait < 30); // wait for sync high
			while (digitalRead(12) != LOW && wait < 30);  // wait for sync high->low
			// WS2811 update begins immediately after falling edge of frame sync
			if (wait < 30) {
				digitalWrite(13, HIGH);
//      leds.show();
        FastLED.show();
				digitalWrite(13, LOW);
			}
		}

	}
	else if (startChar == '@') {
		// reset the elapsed frame time, for startup of '$' message playing
		elapsedUsecSinceLastFrameSync = 0;

	}
	else if (startChar == '?') {
		// when the video application asks, give it all our info
		// for easy and automatic configuration
		Serial.print(LED_WIDTH);
		Serial.write(',');
		Serial.print(LED_HEIGHT);
		Serial.write(',');
		//    Serial.print(LED_LAYOUT); //G! commented, not using Zig Zag
		//    Serial.write(',');  // G!
		Serial.print(0);
		Serial.write(',');
		Serial.print(0);
		Serial.write(',');
		Serial.print(VIDEO_XOFFSET);
		Serial.write(',');
		Serial.print(VIDEO_YOFFSET);
		Serial.write(',');
		Serial.print(VIDEO_WIDTH);
		Serial.write(',');
		Serial.print(VIDEO_HEIGHT);
		Serial.write(',');
		Serial.print(0);
		Serial.write(',');
		Serial.print(0);
		Serial.write(',');
		/*
		 *  Added by G! making unique definitions for second Octo board
		 */
		Serial.print(0);
		Serial.write(',');
		Serial.print(0);
		Serial.write(',');
		Serial.print(VIDEO_XOFFSET1);
		Serial.write(',');
		Serial.print(VIDEO_YOFFSET1);
		Serial.write(',');
		Serial.print(VIDEO_WIDTH1);
		Serial.write(',');
		Serial.print(VIDEO_HEIGHT1);
		Serial.write(',');
		Serial.print(0);
		Serial.write(',');
		Serial.print(0);
		Serial.write(',');
		/*
		 * G! finished adding definitions
		 */
		Serial.print(0);
		Serial.println();

	}
	else if (startChar >= 0) {
		// discard unknown characters
	}
} // VideoDisplay()

#endif
