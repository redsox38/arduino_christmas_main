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
*/


//data storage variables
byte newData = 0;
byte prevData = 0;

//freq variables
unsigned int timer = 0;//counts period of wave
unsigned int period;
int frequency;
unsigned int delay_time = 200;

void setup(){
  
  //Serial.begin(9600);
  
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
  if (frequency >= 1 and frequency <= 399) {
    PORTC |= B01101111;
    delay(delay_time);
    PORTC &= B10010000;
  }
  if (frequency >= 201 and frequency <= 850.99) {
    PORTC |= B10010000;
    delay(delay_time);
    PORTC &= B01101111;
  }
  if (frequency >= 800 and frequency <= 1500) {
    PORTC |= B11011011;
    delay(delay_time);
    PORTC &= B00100100;
  }
  if (frequency >= 1000 and frequency <= 3000) {
    PORTC |= B01111101;
    delay(delay_time);
    PORTC &= B10000010;
  }
  if (frequency >= 1800 and frequency <= 20000) {
    PORTC |= B11111010;
    delay(delay_time);
    PORTC &= B00000101;
  }

  //Serial.println(" hz");
    
}