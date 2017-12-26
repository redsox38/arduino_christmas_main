//sine wave freq detection with 38.5kHz sampling rate and interrupts
//by Amanda Ghassaei
//http://www.instructables.com/id/Arduino-Frequency-Detection/
//July 2012

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 
 * The mega should have a HC05 module in master mode connected to pins 18/19 to 
 * send commands in SPP mode to an additional arduino with an HC05 in slave mode and its own
 * relay board
*/


//data storage variables
byte newData = 0;
byte prevData = 0;

const byte ledPin = 13;

//freq variables
unsigned int timer = 0;//counts period of wave
unsigned int period;
int frequency;
unsigned int delay_time = 200;

void setup(){
  
  // setup HC-05 that has already been configured for master mode
  // and paired with another HC-05
  pinMode(ledPin, OUTPUT);
  
  // setup console for debugging
  Serial.begin(9600);
  
  // Open up serial line to HC05 on pins 18/19
  Serial1.begin(38400);
  
  // setup PORTC pins
  pinMode(30,OUTPUT);//led indicator pin
  pinMode(31,OUTPUT);//output pin
  pinMode(32,OUTPUT);
  pinMode(33,OUTPUT);
  pinMode(34,OUTPUT);
  pinMode(35,OUTPUT);
  pinMode(36,OUTPUT);
  pinMode(37,OUTPUT);

  PORTC = B11111111;

  cli();//disable interrupts
  
  //set up continuous sampling of analog pin 0
  
  //clear ADCSRA and ADCSRB registers
  ADCSRA = 0;
  ADCSRB = 0;
  
  ADMUX |= (1 << REFS0); //set reference voltage
  ADMUX |= (1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only
  
  ADCSRA |= (1 << ADPS2) | (1 << ADPS0); //set ADC clock with 32 prescaler- 16mHz/32=500kHz
  ADCSRA |= (1 << ADATE); //enabble auto trigger
  ADCSRA |= (1 << ADIE); //enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN); //enable ADC
  ADCSRA |= (1 << ADSC); //start ADC measurements
  
  sei();//enable interrupts
  
}

ISR(ADC_vect) {//when new ADC value ready

  prevData = newData;//store previous value
  newData = ADCH;//get value from A0
  if (prevData < 127 && newData >=127){//if increasing and crossing midpoint
    period = timer;//get period
    timer = 0;//reset timer
    frequency = 38462/period;
    delay(10);
  }
    
  timer++;//increment timer at rate of 38.5kHz
}

void loop(){
  //frequency = 38462/period;//timer rate/period
  //print results
  if (frequency >= 0 and frequency <= 250) {
    //bass
    digitalWrite(ledPin, HIGH);
    Serial1.write("3"); 
    PORTC |= B00000000;
    // double the normal delay to match the time for sequence 3 on the uno board
    delay(2 * delay_time);
    PORTC &= B11111111;
    digitalWrite(ledPin, LOW);
  } else if (frequency > 250 and frequency <= 500) {
    //low midrange
    digitalWrite(ledPin, HIGH);
    Serial1.write("1");
    PORTC |= B10010000;
    delay(delay_time);
    PORTC &= B01101111;
    digitalWrite(ledPin, LOW);
  } else if (frequency > 500 and frequency <= 2000) {
    //midrange
    digitalWrite(ledPin, HIGH);
    Serial1.write("2");
    PORTC |= B11011011;
    delay(delay_time);
    PORTC &= B00100100;
    digitalWrite(ledPin, LOW);
  } else if (frequency > 2000 and frequency <= 4000) {
    // upper midrange
    PORTC |= B01111101;
    delay(delay_time);
    PORTC &= B10000010;
  } else if (frequency > 4000 and frequency <= 20000) {
    // presence
    PORTC |= B01111010;
    delay(delay_time);
    PORTC &= B10000101;
  }

  //Serial.println(" hz");
    
}
