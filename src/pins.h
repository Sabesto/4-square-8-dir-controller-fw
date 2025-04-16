// IO Expander pins
#define P00 0
#define P01 1
#define P02 2
#define P03 3
#define P04 4
#define P05 5
#define P06 6
#define P07 7
#define P10 8
#define P11 9
#define P12 10
#define P13 11
#define P14 12
#define P15 13
#define P16 14
#define P17 15

// IO Expander 1
#define IOEX1_ADDR 0x74
#define RELAY_0    P00
#define RELAY_1    P01
#define RELAY_2    P02
#define RELAY_3    P03
#define RELAY_4    P04
#define RELAY_5    P05
#define RELAY_PTT  P10
// Lookup table for direction -> relay
#define DIR_N  (uint8_t)((1 << RELAY_0) | (1 << RELAY_3))
#define DIR_NE (uint8_t)0
#define DIR_E  (uint8_t)((1 << RELAY_2) | (1 << RELAY_3) | (1 << RELAY_5))
#define DIR_SE (uint8_t)((1 << RELAY_0) | (1 << RELAY_1) | (1 << RELAY_2) | (1 << RELAY_3) | (1 << RELAY_4) | (1 << RELAY_5))
#define DIR_S  (uint8_t)((1 << RELAY_1) | (1 << RELAY_2) | (1 << RELAY_5))
#define DIR_SW (uint8_t)((1 << RELAY_4) | (1 << RELAY_5))
#define DIR_W  (uint8_t)((1 << RELAY_0) | (1 << RELAY_1) | (1 << RELAY_5))
#define DIR_NW (uint8_t)((1 << RELAY_0) | (1 << RELAY_1) | (1 << RELAY_2) | (1 << RELAY_3)) 


// IO Expander 2
#define IOEX2_ADDR 0x75
#define ROT_SW_0   P00
#define ROT_SW_1   P01
#define ROT_SW_2   P02
#define ROT_SW_3   P03
#define ROT_SW_4   P04
#define ROT_SW_5   P05
#define ROT_SW_6   P06
#define ROT_SW_7   P07

#define DIR_LED_NW P10
#define DIR_LED_N  P11
#define DIR_LED_NE P12
#define DIR_LED_W  P13
#define DIR_LED_E  P14
#define DIR_LED_SE P15
#define DIR_LED_S  P16
#define DIR_LED_SW P17

// ESP32 pins
#define SWR_LED_0 9
#define SWR_LED_1 8
#define SWR_LED_2 18
#define SWR_LED_3 17
#define SWR_LED_4 16
#define SWR_LED_5 15
#define SWR_LED_6 7
#define SWR_LED_7 6
#define SWR_LED_8 5
#define SWR_LED_9 4

#define SWR_FILT      13
#define RELAY_CURRENT 14

#define PTT_LED   48
#define PTT_SENSE 2     // PTT is active when LOW?
#define BUZZER    38
#define RESET     40
#define INT       39
#define SDA       21
#define SCL       47
#define TOGGLE_SW 43

