#include <util/delay.h>

#define SEN_MAX 150
#define MOT_TM 1000

const uint8_t pd[10] = {0xfb,0x63,0xb7,0x77,0x6f,0x5f,0xdf,0x73,0xff,0x7f};
const uint8_t pb[10] = {0xff,0xfe,0xff,0xff,0xfe,0xff,0xff,0xfe,0xff,0xff};

int main()
{
  DDRD = 0xFF;
  DDRB = 0xFF;
  while(1)
  {
    uint16_t sensor = analog();
    
    if(sensor > SEN_MAX) sensor = SEN_MAX;
    
    uint8_t motor_speed = sensor*10 / SEN_MAX ;
   
    seven_seg(motor_speed);
    
    motor(motor_speed);
    
    _delay_ms(10);
  }
}
void seven_seg(uint8_t i)
{
  PORTD &= pd[i];
  PORTD |= pd[i] & 0xfc;
  PORTB &= pb[i];
  PORTB |= pb[i] & 0x01;
}
uint16_t analog_read(uint8_t pin)
{
  ADMUX |= (pin & 0x0F);
  ADMUX |= (1<<REFS0);
  ADMUX &= ~(1<<ADLAR);
  
  ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
  ADCSRA |= (1<<ADEN);
  ADCSRA |= (1<<ADSC);
  
  while(ADCSRA & (1<< ADSC));
  
  return ADCL + (ADCH<<8);
  
}
uint16_t analog()
{
  uint16_t sum = 0;
  
  for(uint8_t i = 0; i<10 ; i++)
    sum += analog_read(0);
    
  return sum/10;
}
void motor(uint8_t spd)
{
  static uint8_t motor_state;
  
  if(spd)
  {
    motor_state ^= 1;
  
    if(motor_state)
    {
      PORTB |= (1<<PORTB1);
      PORTB &= ~(1<<PORTB2);
    }else
      {
        PORTB &= ~(1<<PORTB1);
        PORTB |= (1<<PORTB2);
      }
    _delay_ms(MOT_TM);
    
    PORTB &= ~(1<<PORTB1);
    PORTB &= ~(1<<PORTB2);
    
    for(uint8_t i =0; i<(10-spd) ; i++)
      _delay_ms(1000);
   }
}
