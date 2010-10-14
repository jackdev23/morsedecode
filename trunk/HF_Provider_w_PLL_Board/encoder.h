#ifndef ___MORSE_H____
#define ___MORSE_H____

#define MORSE_BUFFER_LENGTH 256

// the functions that you will want to do
void encoder_init(uint8_t downlink_wpm, PORT_t *port, uint8_t pin);
uint8_t encoder_is_busy();
uint8_t encoder_send( char *fmt, ... );

#endif
