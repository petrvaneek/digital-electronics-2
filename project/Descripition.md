# Project 1

Application of analog joy-stick (2 ADC channels, 1 push button), rotary encoder, and Digilent PmodCLP LCD module.

## Instructions

The goal of the project is cooperation in pairs, further study of the topic, design of own solutions, implementation, creation of project documentation and presentation of results. The division of roles and tasks within the team is up to their members.

* Students work on a project in the labs during the 10th and 11th weeks of the semester.

* Through the BUT e-learning, students submit a link to the GitHub repository, which contains the complete project in PlatfomIO and a descriptive README file. The submission deadline is the day before the next laboratory, i.e. in 2 weeks.

* The source code for the AVR must be written in C and/or Assembly and must be implementable on Arduino Uno board using toolchains from the semester, ie PlatformIO and not in the Arduino-style. No other development tools are allowed.

## Recommended GitHub repository structure

   ```c
   YOUR_PROJECT        // PlatfomIO project
   ├── include         // Included files
   ├── lib             // Libraries
   ├── src             // Source file(s)
   │   └── main.c
   ├── test            // No need this
   ├── platformio.ini  // Project Configuration File
   └── README.md       // Report of this project
   ```

## Recommended README.md file structure

### Team members

* Member 1 (responsible for xxx)
* Member 2 (responsible for xxx)

## Hardware description

![image](https://user-images.githubusercontent.com/99393183/206266908-b6661ff2-e668-4a27-8c88-51d229987e95.png)

## Software description

1. Write your text here.
2. ...
Using the joystick, the user can choose one of the 5 passwords he wants to remind himself of (credit card, email, etc.). With the rotary encoder, the user can edit the password – increment the value by turning it one way, or decrement it by turning it the other way.

S pomocí joysticku je možné zvolit požadované číselné heslo, které LCD displej vypíše. Je možné si vybrat jedno z 5 možností – PIN, emai, phone, job a notebook. S pomocí enkodéru je možné heslo editovat. Otočením enkodéru na jednu stranu heslo inkrementuje o 1, otočením na druhou dekrementuje o 1.

## Video

Insert a link to a short video with your practical implementation example (1-3 minutes, e.g. on YouTube).

## References

1. Write your text here.
2. ...
Using the joystick, the user can choose one of the 5 passwords he wants to remind himself of (credit card, email, etc.). With the rotary encoder, the user can edit the password – increment the value by turning it one way, or decrement it by turning it the other way.

S pomocí joysticku je možné zvolit požadované číselné heslo, které LCD displej vypíše. Je možné si vybrat jedno z 5 možností – PIN, emai, phone, job a notebook. S pomocí enkodéru je možné heslo editovat. Otočením enkodéru na jednu stranu heslo inkrementuje o 1, otočením na druhou dekrementuje o 1.
