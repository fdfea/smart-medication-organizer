# **Smart Medication Organizer**

### **Device Description**

The Smart Medication Organizer (SMO) is a wirelessly programmable, internet-of-things device that assists in the consistent self-administration of pharmaceuticals by patients, in order to reduce nonadherence. To accomplish this goal, the SMO provides timed auditory and visual indications to signal to the user to take their medications. The device encompasses a screen, a speaker, a wall outlet power supply, a microcontroller, and six medication compartments, each with an LED. The electronics are located inside a wooden box to hide the details from the user. The device connects to a user's wireless local area network, where it can be reached from the user's personal or mobile device through a web application. The web application allows the user to configure when they need to take which medications, which compartment each medication is in, and dosage information. This information is encrypted to protect the user's data privacy and then sent to the device, where events are scheduled based on the information provided. When it is time for the user to take their medication, the speaker plays a sound to draw the user's attention, the LEDs indicate in which compartment the medication to take is located, and the screen displays other necessary dosage information. The user can stop the sound and LEDs by pressing an okay button to confirm they have taken their medication, and the next medication event will be scheduled automatically. The device is a compact, easy-to-use, and affordable solution to unintentional nonadherence. 

![Alt text](images/smo-outside.jpeg?raw=true "SMO Outside") ![Alt text](images/smo-inside.jpg?raw=true "SMO Inside ")

### **Explanation of Embedded Software**

The embedded software is controlled by the MSP432P401R microcontroller and the CC3120BOOST wireless networking booster pack. The software is divided into several modules: Wi-Fi connection, real-time clock (RTC) management, user configuration server, hardware drivers, and medication information management and lifecycle. The resources are managed by the TI-RTOS real-time operating system and many of the TI MSP432 SDK APIs were leveraged to simplify implementation. When the microcontroller is powered on, the device connects to the user’s wireless local area network using hardcoded login information and is assigned an IP address. (We would have liked the Wi-Fi connection to be initiated from the client-side, but the limited nature of the semester restricted some of the advanced features we had hoped to implement). Once the device is connected to the internet, it queries a remote time server and starts the RTC module with the current time information. The RTC module configures two interrupts: one that triggers every minute and updates the time/date on the screen and one that is triggered by an alarm which can be set in the RTC module. Additionally, after connecting to Wi-Fi, the device opens a UDP server that can be reached by the user application. When the server receives data it decrypts the packet using AES-256-ECB encryption and validates the input and then updates the device's medication information. The server expects the packet to be organized as follows: 1 byte to indicate how many medication events, n , the packet contains, followed by 35*n bytes for the medication event data. Each medication is encoded as follows: 1 byte for the hour to take, 1 byte for the minute to take, 1 byte for the how many to take, 1 byte for which compartment the medication is in, 1 byte for the length of the med info string, and 30 bytes for the med info string. The screen driver communicates with the screen (EVE3-50A) via SPI. The driver allows the SMO to display the date, time, and medication info. The screen also controls the PWM output to the speaker (SP-3020),  which allows the SMO to start and stop the sound and manipulate the volume and pitch. The LED driver communicates with the LED integrated circuit (LP5018) via I2C, which controls the six RGB LEDs (IN-S128TATRGB) on the SMO. The SMO can turn on and off any of the individual LEDs and set the color and brightness. The main SMO control logic algorithm is as follows: When the UDP server receives a valid medication info packet, it clears any previous data that was set and stores the information contained in the packet. Then, the SMO finds the event which most closely follows the current time and schedules an RTC alarm for the event's time. When the alarm occurs, the SMO activates the LEDs specified by the event and sounds the speaker to signal to the user that it is time to take a medication. The SMO also displays the medication dosage and info string on the screen. The user can press the button (40-2388-01) to acknowledge the event and turn off the speaker and LEDs, or the event will timeout after 5 minutes. The next event is automatically scheduled when one occurs, and the whole process repeats indefinitely while the device is powered.
