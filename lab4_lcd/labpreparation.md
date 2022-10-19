

1. Use schematic of the [LCD keypad shield](https://oshwlab.com/tomas.fryza/arduino-shields) and find out the connection of LCD display. What data and control signals are used? What is the meaning of these signals?

   | **LCD signal(s)** | **AVR pin(s)** | **Description** |
   | :-: | :-: | :-- |
   | RS | PB0 | Register selection signal. Selection between Instruction register (RS=0) and Data register (RS=1) |
   | R/W | GND  | only write data (logical 0) |
   | E | PB1 | enable. This loads the data into the HD44780 on the falling edge |
   | D[3:0] | not used | not used |
   | D[7:4] | PD4, PD5, PD6, PD7 |  Upper nibble used in 4-bit mode |
   | K | throught transistor controlled by PB2 | Backlight cathode |
   
   2. What is the ASCII table? What are the codes/values for uppercase letters A to Z, lowercase letters a to z, and numbers 0 to 9 in this table?

| **Char** | **Decimal**|**Hexadecimal** |
| :-: | :-: | :-: |
| A | 65 | 0x41 | 
| B | 66 | 0x42 | 
| C | 67 | 0x43 |
| D | 68 | 0x44 | 
| E | 69 | 0x45 |
| F | 70 | 0x46 |
| G | 71 | 0x47 |
| H | 72 | 0x48 |
| I | 73 | 0x49 |
| J | 74 | 0x4A |
| K | 75 | 0x4B |
| L | 76 | 0x4C |
| M | 77 | 0x4D |
| N | 78 | 0x4E |
| O | 79 | 0x4F |
| P | 80 | 0x50 |
| Q | 81 | 0x51 |
| R | 82 | 0x52 |
| S | 83 | 0x53 |
| T | 84 | 0x54 |
| U | 85 | 0x55 |
| V | 86 | 0x56 |
| W | 87 | 0x57 |
| X | 88 | 0x58 |
| Y | 89 | 0x59 |
| Z | 90 | 0x5A |
| a | 97 | 0x61 |
| b | 98 | 0x62 |
| c | 99 | 0x63 |
| d | 100 | 0x64 |
| e | 101 | 0x65 |
| f | 102 | 0x66 |
| g | 103 | 0x67 |
| h | 104 | 0x68 |
| i | 105 | 0x69 |
| j | 106 | 0x6A |
| k | 107 | 0x6B |
| l | 108 | 0x6C |
| m | 109 | 0x6D |
| n | 110 | 0x6E |
| o | 111 | 0x6F |
| p | 112 | 0x70 |
| q | 113 | 0x71 |
| r | 114 | 0x72 |
| s | 115 | 0x73 |
| t | 116 | 0x74 |
| u | 117 | 0x75 |
| v | 118 | 0x76 |
| w | 119 | 0x77 |
| x | 120 | 0x78 |
| y | 121 | 0x79 |
| z | 122 | 0x7A |
| 0 | 48 | 0x30 |
| 1 | 49 | 0x31 |
| 2 | 50 | 0x32 |
| 3 | 51 | 0x33 |
| 4 | 52 | 0x34 |
| 5 | 53 | 0x35 |
| 6 | 54 | 0x36 |
| 7 | 55 | 0x37 |
| 8 | 56 | 0x38 |
| 9 | 57 | 0x39 |
    // Custom character definition using https://omerk.github.io/lcdchargen/
    uint8_t customChar[8] = {
        0b00100,
        0b00100,
        0b01110,
        0b01110,
        0b00100,
        0b00100,
        0b00100,
        0b11111
    };

    // Initialize display
    lcd_init(LCD_DISP_ON);

    lcd_command(1<<LCD_CGRAM);       // Set addressing to CGRAM (Character Generator RAM)
                                     // ie to individual lines of character patterns
    for (uint8_t i = 0; i < 8; i++)  // Copy new character patterns line by line to CGRAM
        lcd_data(customChar[i]);
    lcd_command(1<<LCD_DDRAM);       // Set addressing back to DDRAM (Display Data RAM)
                                     // ie to character codes

    // Display symbol with Character code 0
    lcd_putc(0x00);
