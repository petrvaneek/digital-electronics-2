# C project 

## Team members

* Michael Doležel (responsible for GPS sensor data processing, saving data to PC and github)
* Samuel Vandovič (responsible for support)
* Petr Vaněk (responsible for DH12 and OLED display communication)
* Dominik (responsible for presentation)


### Table of contents

* [Topic](#Topic)
* [Project objectives](#objectives)
* [Theoretical description and explanation](#Theoretical)
* [Library copyrights](#Library_copyrights)
* [Data saving to PC](#Data_saving_to_PC)
* [Schematic](#Schematic)
* [Software](#Software)
* [Presentation](#Presentation)
* [Flowcharts](#Flowcharts)
* [Conclusion](#Conclusion)
* [References](#references)


**GPS-based environmental sensor data logger**
Create a comprehensive data logging system using an AVR microcontroller. The system integrates GPS functionality for location tracking and an I2C environment sensor to capture data related to environmental conditions. The project aims to log and display sensor data and provide the capability to export the collected information for analysis.

<a name="objectives"></a>
## Project objectives

- [x] Create a program capable of reading GPS data
- [x] Format them and save them to a .txt file 
- [x] Add Humidity and temperature data
- [x] Display data in real time on oled display


<a name="Theoretical"></a>
## Theoretical description and explanation

In our project use GPS module [*GPS NEO-6M GYNEO6MV2*](https://dratek.cz/arduino/1510-gps-neo-6m-gyneo6mv2-modul-s-antenou.html?ehub=e6081933ad2044229d14dafac815f8e3) and save data "[**GPRMC**](https://docs.novatel.com/OEM7/Content/Logs/GPRMC.htm)" then we merge them with data from Humidity/temperature **DHT12 digital sensor** and display them on **SH1106 I2C OLED display 128x64**. Data are also saved to a .txt file for future use and analysis.

GPS sensor communicates with UART protocol and sends info in various formats from a few GPS satellites messages usually look like this: 

```
$GPRMC 093533.00 A 4913.58989 N 01634.42942 E 0.322 21.61 161123   A53\r\n
$GPVTG 21.61 T A 0.322 0.597 K A01\r\n
$GPGGA 093533.00 4913.58989 N 01634.42942 E 1 03 4.64 261.4 M 42.4 M   56\r\n
$GPGSA A 2 04 09 16           4.74 4.64 1.0009\r\n
$GPGSV 2 1 06 04 21 082 36 07       35 09 55 078 40 13 00 263 42\r\n
$GPGSV 2 2 06 16 16 041 36 20 47 2987D\r\n
$GPGLL 4913.58989 N 01634.42942 E 093533.00 A A65\r\n
$GPRMC 093533.00 A 4913.58989 N 01634.42942 E 0.322 21.61 161123   A53\r\n
```

![gps_1](https://github.com/petrvaneek/digital-electronics-2/assets/99393183/614dadb3-e2dc-40a2-9304-28a0ec6ddf9e)

![gps_2](https://github.com/petrvaneek/digital-electronics-2/assets/99393183/edbb58b2-0d11-4acd-b8eb-05109badd70d)

![gps_3](https://github.com/petrvaneek/digital-electronics-2/assets/99393183/edc0fa65-2d28-41ee-b12b-0412224d36a2)


GPS sensor sends them at a rate of one message per second on a baud rate of 9600
Our problem was that working with this sensor turned out to be a bit complicated due to the low power of the antenna causing us to receive GPS signal outside of the window or outside 

So we created a simple solution to transmit our measured signal on one Arduino and process the data in the second Arduino 

![Arduino_to_Arduino](https://github.com/petrvaneek/digital-electronics-2/assets/99393183/c4eacff8-0d6b-4c80-9566-91069f5e739f)


The next task was to receive the data coming to UART, select the GPRMC part, and store it as data.
the GPRMC looks like this: **$GPRMC 093533.00 A 4913.58989 N 01634.42942 E 0.322 21.61 161123   A53\r\n**
The first field is UTC of position in hhmmss.ss format so in this case we measured at 09:35:33 UTC. Next is position status with only two values (A = data valid, V = data invalid). after that is latitude in format 
(DDmm.mm) followed by latitude direction (N = North, S = South) as can be expected, the next two fields are longitude (DDDmm.mm) and longitude direction (E = East, W = West). The following fields are Speed over ground in knots than Track made good - degrees True (which means the traveling direction that is independent of the direction in which the device is pointing), then the date (dd/mm/yy), last is Checksum and sentence terminator.

![GPS_meas2](https://github.com/petrvaneek/digital-electronics-2/assets/99393183/ee0cb98e-544a-4477-9ac9-1c633051ad79)


From all this data we decided that only Date, time, and position are useful.
These coordinates from the window we measured in are roughly 50m from our correct location, which can be explained by measurement a short time after the sensor found the GPS signal. 

Longitude and latitude are in Decimal degrees (DD) format (Latitude4913.58989 N, Longitude: 01634.42942 E) first we need to move decimal point like this (Latitude:49.1361781 N, Longitude: 016.3448005 E)
To translate this format to degrees, minutes, seconds (DMS) format we need few operations.

**Latitude:**

- Degrees: 49 degrees
- Minutes: 13.58989 minutes
- Seconds: (0.58989 * 60) = 35.3934 seconds

So, the DMS format for the latitude is approximately 49° 13' 35.3934'' N.

**Longitude:**

- Degrees: 16 degrees
- Minutes: 34.42942 minutes
- Seconds: (0.42942 * 60) = 25.7652 seconds

So, the DMS format for the longitude is approximately 16° 34' 25.7652'' E.

<a name="Schematic"></a>
## Schematic of our project:

![Project_bb](https://github.com/petrvaneek/digital-electronics-2/assets/99393183/3369fbbf-e71b-4091-8360-6b638de0adcd)




<a name="Software"></a>
## Software description

### Functions to extract data from GPRMC message 

Function to extract time from the GPRMC sentence and format it as "XX:XX:XX"
```
void extractAndFormatTime(char *data, char *formattedTime) {
    // Define array size of time string + Null-terminate
    char time[7];
    // Extract the time part from the input data starting from the 7th character taking 6 characters
    strncpy(time, data + 7, 6);
    // Null-terminate the extracted time string to make it a valid C string
    time[6] = '\0';  
    // Format the time and store it in the formattedTime string
    sprintf(formattedTime, "%c%c:%c%c:%c%c", time[0], time[1], time[2], time[3], time[4], time[5]);
}
}
```

Function to extract date from the GPRMC sentence and format it as "XX.XX.XX"
_This function was bit more problematic because our GPS GPRMC sentence is variable at length due to speed and angle data. This property was not problem for rest of data because they are before Speed and angle_
```
void extractAndFormatDate(char *data, char *formattedDate) {
    // Define array size of date string + Null-terminate
    char date[7];
    // Calculate the starting index for the date by subtracting 12 characters from the end of the string
    int startIndex = strlen(data) - 15;  // Adjusted for the date length
    // Extract the date part from the input data starting from the calculated index taking 6 characters
    strncpy(date, data + startIndex, 6);
    // Null-terminate the extracted date string to make it a valid C string
    date[6] = '\0';
    // Format the date and store it in the formattedDate string
    sprintf(formattedDate, "%c%c.%c%c.%c%c", date[0], date[1], date[2], date[3], date[4], date[5]);
}
```


Function to extract latitude from the GPRMC sentence
```
void extractLatitude(char *data, char *latitude) {
    // Extract the time part from the input data starting from the 19th character taking 10 characters
    strncpy(latitude, data + 19, 10);  
    // Null-terminate the extracted time string to make it a valid C string
    latitude[10] = '\0';  
}
```

Function to extract latitude direction N/S from the GPRMC sentence
```
char extractLatitudeDir(char *data) {
    // Extract the latitude direction from the input data from the 30th character
    return data[30];
}
```

Function to extract longitude from the GPRMC sentence
```
void extractLongitude(char *data, char *longitude) {
    // Extract the time part from the input data starting from the 19th character taking 10 characters
    strncpy(longitude, data + 32, 10);
    // Null-terminate the extracted time string to make it a valid C string
    longitude[11] = '\0';
}
```

Function to extract longitude direction E/W from the GPRMC sentence
```
char extractLongitudeDir(char *data) {
    // Extract the latitude direction from the input data from the 44th character
    return data[44];
}
```

Main function

```
int main(void) {
    uart_init();

    while (1) {
        // Receive data from the RX pin
        char received_char = uart_receive();

        // Save the received character into the buffer
        received_data[received_index] = received_char;

        // Increment the buffer index (with overflow protection)
        received_index = (received_index + 1) % BUFFER_SIZE;

        // Check for a newline character, indicating the end of a message
        if (received_char == '\n') {
            // Null-terminate the string
            received_data[received_index] = '\0';

            // Check if the message starts with "$GPRMC"
            if (strncmp(received_data, gprmc_prefix, strlen(gprmc_prefix)) == 0) {
            .
            .
            .
            }

            // Reset the buffer index for the next message
            received_index = 0;
        }
    }

    return 0;
}
```







Check if the message starts with "$GPRMC" and extract and format the time, date, latitude, and longitude data and transmit them through UART
```
char gprmc_prefix[] = "$GPRMC";

            // Check if the message starts with "$GPRMC"
            if (strncmp(received_data, gprmc_prefix, strlen(gprmc_prefix)) == 0) {
                // Extract and format the time, date, latitude, and longitude data
                char formattedTime[9];
                char formattedDate[9];
                char latitude[11];
                char longitude[13];
                char latitudeDir, longitudeDir;

                extractAndFormatTime(received_data, formattedTime);
                extractAndFormatDate(received_data, formattedDate);
                extractLatitude(received_data, latitude);
                extractLongitude(received_data, longitude);
                latitudeDir = extractLatitudeDir(received_data);
                longitudeDir = extractLongitudeDir(received_data);

                // Transmit the formatted data
                uart_transmit_string("Time:,");
                uart_transmit_string(formattedTime);
                uart_transmit_string(",");

                uart_transmit_string("Date:,");
                uart_transmit_string(formattedDate);
                uart_transmit_string(",");

                uart_transmit_string("Latitude:,");
                uart_transmit_string(latitude);
                uart_transmit_string(" ");
                uart_transmit(latitudeDir);
                uart_transmit_string(",");


                uart_transmit_string("Longitude:,");
                uart_transmit_string(longitude);
                uart_transmit_string(" ");
                uart_transmit(longitudeDir);
                


                uart_transmit_string("\r");
                uart_transmit_string("\n");
            }
```



<a name="Library_copyrights"></a>
## Library copyrights 
All used libraries have free license use for our case


<a name="Data_saving_to_PC"></a>
## Data saving to PC

Data are being formatted like this: **Time:,12:36:50, Date:, 26.11.23, Latitude:,4913.58989 N, Longitude:, 01634.42942 E** "," are for saving data to Excel for further analysis. We use a program called **CoolTerm** to save UART data from the GPS sensor to a .txt file.

_Underneath is a video with a manual for saving data with CoolTerm_

[![CoolTerm](https://img.youtube.com/vi/RWgyCcnUxPY/hqdefault.jpg)](https://www.youtube.com/embed/RWgyCcnUxPY)

![notepad](https://github.com/petrvaneek/digital-electronics-2/assets/99393183/ced98b3d-8fd0-4905-abfb-2cf846528b95)




Then we added temperature and humidity data from the DH12 Sensor

![excel2](https://github.com/petrvaneek/digital-electronics-2/assets/99393183/b76216aa-8182-4a94-aaf2-a9dd741fd481)


After that data can be processed

![Graph](https://github.com/petrvaneek/digital-electronics-2/assets/99393183/91ca5443-58d1-4e6c-9d24-d3f4ff2ee90c)


<a name="Flowcharts"></a>
## Flowcharts

Put flowcharts of your algorithm(s) and direct links to source files in `src` or `lib` folders.

<a name="Conclusion"></a>
## Conclusion

Could you write an instruction manual for your application, including photos and a link to a short app video?

<a name="references"></a>
## References

1. [Atmel AVR Microcontroller ATmega328P for initializing of uart communication page 168-169](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf)
2. [CoolTerm Download site](https://freeware.the-meiers.org/)
3. [GPS module datasheet](https://www.waveshare.com/w/upload/a/a9/U-blox-6-Receiver-Description-Including-Protocol-Specification.pdf)
