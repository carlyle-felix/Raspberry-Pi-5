#ifndef RPINS_H
#define RPINS_H

typedef struct line *Line_list;

Line_list populate_pins(void);
int print_pins(Line_list rpi, enum print format);

#endif