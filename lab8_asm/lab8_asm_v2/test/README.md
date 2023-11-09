# Lab 8: Petr Vaněk

### Instruction set

1. Complete the conversion table with selected instructions:

   | **Instruction** | **Binary opcode** | **Hex opcode** | **Compiler Hex opcode** |
   | :-- | :-: | :-: | :-: |
   | `add r24, r0` | `0000_1101_1000_0000` | `0D80` | `800D` |
   | `com r26` | `1001_0101_1010_0000` | `95A0` | `A095` |
   | `eor r26, r27` | `0010_0111_1011_1010` | `27BA` | `BA27` |
   | `mul r22, r20` | `1001_1111_0110_0100` | `9F64` | `64FD` |
   | `ret` | `1001_0101_0000_1000` | `9508` | `0895` |

### 4-bit LFSR

2. Complete table with 4-bit LFSR values for different Tap positions:

   | **Tap position** | **Generated values** | **Length** |
   | :-: | :-- | :-: |
   | 4, 3 | 0 1 3 7 14 13 11 6 12 9 2 5 10 4 8 | 15 |
   | 4, 2 | 0 1 3 6 12 8 | 6 |
   | 4, 1 | 0 1 2 5 10 4 9 3 6 13 11 7 14 12 8 | 15 |
   for 5-bit register :0 1 3 7 14 28 25 18 4 8 17 2 5 10 21 11 23 15 30 29 27 22 13 26 20 9 19 6 12 24 16 Sequence lenght: 31
   for 6-bit register :0 1 3 7 15 31 62 61 59 55 47 30 60 57 51 39 14 29 58 53 43 22 44 24 48 33 2 5 11 23 46 28 56 49 35 6 13 27 54 45 26 52 41 18 36 8 17 34 4 9 19 38 12 25 50 37 10 21 42 20 40 16 32 Sequence lenght: 63
   for 7-bit register :0 1 3 7 15 31 63 126 125 123 119 111 95 62 124 121 115 103 79 30 61 122 117 107 87 46 92 56 112 97 67 6 13 27 55 110 93 58 116 105 83 38 76 24 49 98 69 10 21 43 86 44 88 48 96 65 2 5 11 23 47 94 60 120 113 99 71 14 29 59 118 109 91 54 108 89 50 100 73 18 37 74 20 41 82 36 72 16 33 66 4 9 19 39 78 28 57 114 101 75 22 45 90 52 104 81 34 68 8 17 35 70 12 25 51 102 77 26 53 106 85 42 84 40 80 32 64 Sequence lenght: 127

### Variable number of short pulses

3. Draw a flowchart of function `void burst_c(uint8_t number)` which generates a variable number of short pulses at output pin. Let the pulse width be the shortest one. The image can be drawn on a computer or by hand. Use clear descriptions of the individual steps of the algorithms.

![image](https://user-images.githubusercontent.com/99393183/203158403-205b559f-c93e-4052-9135-796951af4c34.png)
