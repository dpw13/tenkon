//
// Authors: dpw13, kitsune
// Created by kitsune on 6/22/2024.
//

#ifndef SERIAL_H
#define SERIAL_H

void initializeSerial();
<<<<<<< Updated upstream
void writeCharToSerial(const char c);
=======
void writeSerial(const char c);
>>>>>>> Stashed changes
void writeStringToSerial(const char *buffer, const int length);
void readSerial(char *c);
int readFromSerial(char *buffer, const int size);

#endif //SERIAL_H
