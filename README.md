## Suggestions for Plans

1. Find out which ports need to be referenced on the chipkit for the different functions
2. Understand the reading writing communication protocol that the RFID card uses
3. Figure out how to display data in the display on the chipkit
4. 
5. Translate the arduino based code into code that works for pic32
6. Learn about cloning a card
7. Learn about miFare cryptographic attacks and vulnerabilities
8. Attempt cryptographic attacks
9. Attempt cloning of RFID card

### Operations for TODO 1 - Ports

```
READ reads one block
WRITE writes one block
DECREMENT decrements the contents of one block and stores the result
in the data-register
INCREMENT increments the contents of one block and stores the result
in the data-register
TRANSFER writes the contents of the data-register to one block
RESTORE stores the contents of one block in the data-register
```

## Resources

### CHIPKIT
 - [General pin descriptions](https://reference.digilentinc.com/chipkit_uno32/refmanual)
 - [CHIPKIT serial communication](https://pic-microcontroller.com/chipkit-tutorial-2-serial-communication-pc/)
 - [Microship forum post about similar project](https://www.microchip.com/forums/m1060620.aspx)

### RFID Shield
 - [Velleman reference manual](https://www.kjell.com/globalassets/mediaassets/745241_87046_manual_en.pdf?ref=F235D313D7)
 - RFID.h and MFRC522.h from arduino library
 - PDF from arduino library (really good)
 - [MFRC522 Documentation](https://www.nxp.com/docs/en/data-sheet/MFRC522.pdf) (Section 8.1 is about digital communication)


## Findings:

page 32 of http://ww1.microchip.com/downloads/en/DeviceDoc/61143H.pdf has trise, porte references

page 11 onwards of https://reference.digilentinc.com/_media/chipkit_uno32:chipkit_uno32_rm.pdf has pinout numbers for pic32

This has basic io pin numbers https://reference.digilentinc.com/reference/add-ons/basic-io-shield/reference-manual

Possibly useful referring to similar project: https://www.youtube.com/watch?v=rgERJZDBJyo  https://github.com/ljos/MFRC522



