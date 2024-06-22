//
// Authors: dpw13, kitsune
// Created by kitsune on 6/22/2024.
//

#ifndef SERIAL_H
#define SERIAL_H

void initializeSerial();
void writeCharToSerial(const char c)
void writeStringToSerial(const char *buffer, const int length);
char readSerial();
int readFromSerial(char *buffer, const int size);


#endif //SERIAL_H
