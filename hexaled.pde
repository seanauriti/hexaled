/*************************************************
 * HexaLED
 * 
 * @Author: Robert Diamond
 * @Date: 8 October 2009
 *
 * Read up to 6 LEDs (on portb).
 *  and control the same number of ports (on portd)
 * Uses capacitive sensing.  All the LEDs cathodes (P) are 
 * connected to a common pin.  Each anode (N) is connected
 * through a 100Ω resistor to a port B pin.
 **************************************************/
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

const int samplebits = 2;
const int numSamples = 1 << samplebits;
 // These variables have to be volatile
 // because they can be changed in the
 // interrupts and read by loop 
 byte lastPortB = 0;
 long sums[6] = {0};
 int deltas[6] = {0};
 unsigned  newSamples[6] = {0}; 
 volatile byte flag = 0;
 // These are only read and written in loop
 byte outputStates;
 byte counts[6] = {numSamples,numSamples,numSamples,numSamples,numSamples,numSamples};
 unsigned  samples[6][numSamples] = {0};
 byte index = 0;
 int samplecount = 0;
 
 const unsigned top = 1000;
 
 const int threshold = 990;
 // base for relay control output pins
 const int output = 2;
 
 void setup()
 {
   cli();
   // re-appropriate timer1
   // Stop its clock
   TCCR1B = 0;
   // Set normal (non-pwm) mode
   TCCR1A = 0;
   // Clear the counter and set the top value
   ICR1 = top;
   TCNT1 = 0;
   // Set the timer mode so TOP = ICR1
   sbi(TCCR1B, WGM13);
   
   // Enable TOV1 interrupt
   sbi(TIFR1, TOV1);
   sbi(TIMSK1, TOIE1);
   
   // set the common_p bit to output
   DDRC = 1;
   
   for (byte i=0; i < 6; ++i) { pinMode(i+output, OUTPUT); digitalWrite(i+output, HIGH); }
   sei();
   
   outputStates = 0x3f;
   
   Serial.begin(19200);
   Serial.println("Good morning.  Would you like to play a game?");
 }
 
 void loop()
 {
   long average;
   
   cli();
   // make the common pin actively low
  PORTC &= 0xfe;
  
  for (byte i=0; i < 6; ++i) newSamples[i] = 0;
  

  // make the port b pins output. 
  DDRB |= 0x3f;
  PORTB |= 0x3f;  // set pull up resistors
  delayMicroseconds(1);
  DDRB &= ~0x3f;
  PORTB &= ~0x3f;  // turn off pullups
  lastPortB = 0x3f;
  
   // Start the clock at 2MHz
  TCNT1 = 0;
  TCCR1B = (2 << CS10);
  
  // clear interrupt indications
  sbi(TIFR1, TOV1);
  flag = 0;
  sei();

  // Now wait for something to happen
  byte i,j;
  while(TCNT1 < top)
  {
   byte pb = PINB;
   byte flipped = (lastPortB ^ pb) & 0x3f;
   
   if (!flipped) continue;
   //Serial.println(flipped);
   lastPortB = pb;
   cli();
   int count = TCNT1;
   sei();
   for (i=0, j=1; i < 6; ++i, j <<= 1)
   {
     if ((flipped & j) == j && (pb & j) == 0 && newSamples[i] == 0)
     {
       newSamples[i] = count;
     }
   } 
  }
  for (byte i=0, j=1; i < 6; ++i, j <<= 1)
    {
        //Serial.print((int)i);
        //Serial.print(" == ");
        //Serial.print(newSamples[i]);
        if (newSamples[i] == 0) newSamples[i] = top;
        sums[i] += newSamples[i];
        sums[i] -= samples[i][index];
        samples[i][index] = newSamples[i];
        average = sums[i] >> samplebits;
        deltas[i] = samples[i][(index - 1) & (numSamples - 1)] - newSamples[i];
        //Serial.print(" average ");
        //Serial.println(average);
        
        
        if (counts[i])
        {
          if (average > threshold)
            --counts[i];
        }
        else
        {
          if (average <= threshold)
          {
            Serial.print("Setting ");
            Serial.print((int)i);
            Serial.print(" to ");
            Serial.println((outputStates & j) ? LOW : HIGH);
           digitalWrite(output+i, (outputStates & j) ? LOW : HIGH);
           outputStates ^= j;
           counts[i] = numSamples;
          }
        }
    }
   ++index;
   index %= numSamples;
  
  // all bits finished, stop the clock
  TCCR1B &=  ~(7 << CS10);
  // flash the LED
  DDRB |= 0x3f;
  PORTB &= ~0x3f;
  // make the common pin actively high
  PORTC |= 1;
  delay(2);
  
  if (Serial.available())
  {
    byte b = Serial.read();
    switch(b)
    {
      case 'n': case 'N':  // all lights on
        for (byte i=0; i < 6; ++i) digitalWrite(output+i, HIGH);
        outputStates = 0x3f;
        break;
      case 'f': case 'F':  // all lights off
        for (byte i=0; i < 6; ++i) digitalWrite(output+i, LOW);
        outputStates = 0;
        break;
      case '0':case '1':case '2':case '3':case '4':
      case '5':
      {
        b -= '0';
        byte bb = (1 << b);
        digitalWrite(output+b, (outputStates & bb) ? LOW : HIGH);
        outputStates ^= bb;
        break;
      }
    }
      
  }
  
  Serial.print(millis());
  Serial.print("ms: ");
  Serial.print(samplecount++);
  Serial.print(": ");
  for (byte i=0; i < 6; ++i)
  {
    if (i) Serial.print(" == ");
    Serial.print((int)i);
    Serial.print(" < ");
    Serial.print(sums[i] >> samplebits);
  }
  Serial.println("");
  Serial.print("Deltas ");
  for (byte i=0; i < 6; ++i)
  {
    if (i) Serial.print(" == ");
    Serial.print((int)i);
    Serial.print(" < ");
    Serial.print(deltas[i]);
  }
  Serial.println("");
  
  // then do it again
 }

 // Occurs when the TIMER matches the overflow value
 // This is when we assume the LEDs aren't activated
 // and give up.
 ISR(TIMER1_OVF_vect)
 {
    flag = 2;
 }
   
