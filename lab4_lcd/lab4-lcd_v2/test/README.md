# Lab 4: Petr Vaněk

### Stopwatch

1. Draw a flowchart for `TIMER2_OVF_vect` interrupt service routine which overflows every 16&nbsp;ms but it updates the stopwatch LCD screen approximately every 100&nbsp;ms (6 x 16&nbsp;ms = 100&nbsp;ms). Display tenths of a second, seconds, and minutes and let the stopwatch counts from `00:00.0` to `59:59.9` and then starts again. The image can be drawn on a computer or by hand. Use clear descriptions of the individual steps of the algorithms.

![image](https://user-images.githubusercontent.com/99393183/197411707-8220844c-49b4-4ccc-9a1a-b5a4a35548f6.png)



### Kitchen alarm

2. Draw a schema of the kitchen alarm application that counts down the time with an LCD, one LED and three push buttons: start, +1 minute, -1 minute. Use the +1/-1 minute buttons to increment/decrement the timer value. After pressing the Start button, the countdown starts. The countdown value is shown on the display in the form of mm.ss (minutes.seconds). At the end of the countdown, the LED will start blinking. The image can be drawn on a computer or by hand. Always name all components and their values.

![image](https://user-images.githubusercontent.com/99393183/197786951-9a5773db-115b-4610-9fe3-9f5c81d12a86.png)
