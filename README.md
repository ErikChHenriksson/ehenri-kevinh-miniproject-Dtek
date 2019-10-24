# Readme

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
