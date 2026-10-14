#ifndef UTILS_H
#define UTILS_H
#include <stddef.h>
void read_line(const char *prompt, char *buffer, size_t size);
int read_int(const char *prompt);
int contains_ignore_case(const char *text, const char *keyword);
void trim_newline(char *text);

void read_name(const char *prompt, char *buffer, size_t size);
void read_borrow_date(char *buffer, size_t size);
void today_str(char *buffer, size_t size);
int date_is_valid(const char *date);
void date_add_days(const char *date, int days, char *out, size_t out_size);
int confirm_action(const char *action);
#endif
