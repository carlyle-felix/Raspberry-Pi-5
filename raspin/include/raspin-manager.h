#ifndef RPINS_H
#define RPINS_H

#define NO_CONSUMER ""

typedef struct line *Line_list;
enum print {PRINT_LIST, PRINT_LAYOUT};

Line_list populate_pins(void);
void print_pins(const char *chip_dev, Line_list rpi, enum print format);
void free_line_list(Line_list rpi);

#endif