# C project

## Topic

**GPS-based environmental sensor data logger**
Create a comprehensive data logging system using an AVR microcontroller. The system integrates GPS functionality for location tracking and an I2C environment sensor to capture data related to environmental conditions. The project aims to log and display sensor data and provide the capability to export the collected information for analysis.

## Team members

* Michael Doležel (responsible for ...)
* Samuel Vandovič (responsible for ...)
* Petr Vaněk (responsible for ...)
* Dominik (responsible for ...)

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

![GPS data](images/gps_1.png)
![GPS data](images/gps_2.png)
![GPS data](images/gps_3.png)

GPS sensor sends them at a rate of one message per second on a baud rate of 9600
Our problem was that working with this sensor turned out to be a bit complicated due to the low power of the antenna causing us to receive GPS signal outside of the window or outside 

![GPS signal measurment](images/gps_measuring.png)

So we created a simple solution to transmit our measured signal on one Arduino and process the data in the second Arduino 

![Ardzuino as source of signal](images/Arduino_to_Arduino.png)

The next task was to receive the data coming to UART and then select the GPRMC part and store it as data.
the GPRMC looks like this: **$GPRMC 093533.00 A 4913.58989 N 01634.42942 E 0.322 21.61 161123   A53\r\n**
The first field is UTC of position in hhmmss.ss format so in this case we measured at 09:35:33 UTC. Next is position status with only two values (A = data valid, V = data invalid). after that is latitude in format 
(DDmm.mm) followed by latitude direction (N = North, S = South) as can be expected, the next two fields are longitude (DDDmm.mm) and longitude direction (E = East, W = West). The following fields are Speed over ground in knots than Track made good - degrees True (which means the traveling direction that is independent of the direction in which the device is pointing), then the date (dd/mm/yy), last is 	
Checksum and sentence terminator.

From all this data we decided that only Date, time, and position are useful.
These coordinates that we measured from the window we measured in are roughly 223m from our correct location, which can be explained by measurement a short time after the sensor found the GPS signal. 

![Measured GPS location](images/GPS_meas.png)

Schematic of our project:

![Schematics](images/Project_bb.png)

## Software description

### Functions to extract data from GPRMC message 

Function to extract time from the GPRMC sentence and format it as "XX:XX:XX"
```
void extractAndFormatTime(char *data, char *formattedTime) {
    // Define array size of time string + Null-terminate
    char time[7];
    7// Extract the time part from the input data starting from the 7th character taking 6 characters
    strncpy(time, data + 7, 6);
    // Null-terminate the extracted time string to make it a valid C string
    time[6] = '\0';
    // Format the time and store it in the formattedTime string
    sprintf(formattedTime, "%c%c:%c%c:%c%c", time[0], time[1], time[2], time[3], time[4], time[5]);
}
```

Function to extract date from the GPRMC sentence and format it as "XX.XX.XX"
```
void extractAndFormatDate(char *data, char *formattedDate) {
    // Declare an array size of date string + Null-terminate
    char date[7];
    // Extract the date part from the input data starting from the 58th character taking 6 characters
    strncpy(date, data + 58, 6);
    // Null-terminate the extracted date string to make it a valid C string
    date[6] = '\0';
    // Format the date and store it in the formattedDate string
    sprintf(formattedDate, "%c%c.%c%c.%c%c", date[0], date[1], date[2], date[3], date[4], date[5]);
}
```


// Function to extract latitude from the GPRMC sentence
```
void extractLatitude(char *data, char *latitude) {
    // Declare an array size of latitude string + Null-terminate
    char latitude[10];
    // Extract the date part from the input data starting from the 20th character taking 9 characters
    strncpy(latitude, data + 20, 9);
    // Null-terminate the string
    latitude[9] = '\0';
}
```

// Function to extract longitude from the GPRMC sentence
```
void extractLongitude(char *data, char *longitude) {
    // Declare an array size of latitude string + Null-terminate
    char date[11];
    // Extract the date part from the input data starting from the 34th character taking 10 characters
    strncpy(longitude, data + 33, 10);  // Start at position 33 taking 10 characters
    // Null-terminate the string
    longitude[10] = '\0';
}
```





Put flowcharts of your algorithm(s) and direct links to source files in `src` or `lib` folders.

## Instructions

Could you write an instruction manual for your application, including photos and a link to a short app video?

## References

1. Write your text here.
2. ...
