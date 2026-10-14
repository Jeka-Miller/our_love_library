#include "utils.h"
#include "models.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void trim_newline(char *text) { text[strcspn(text, "\n")] = '\0'; }

void read_line(const char *prompt, char *buffer, size_t size) {
    printf("%s", prompt);
    if (fgets(buffer, (int)size, stdin) == NULL) { buffer[0] = '\0'; return; }
    trim_newline(buffer);
}

int read_int(const char *prompt) {
    char buffer[32]; char *end; long value;
    while (1) {
        read_line(prompt, buffer, sizeof(buffer));
        value = strtol(buffer, &end, 10);
        while (isspace((unsigned char)*end)) end++;
        if (buffer[0] != '\0' && *end == '\0') return (int)value;
        printf("请输入有效的数字。\n");
    }
}

int contains_ignore_case(const char *text, const char *keyword) {
    size_t text_length = strlen(text), keyword_length = strlen(keyword);
    if (keyword_length == 0) return 1;
    if (keyword_length > text_length) return 0;
    for (size_t i = 0; i <= text_length - keyword_length; i++) {
        size_t j = 0;
        while (j < keyword_length && tolower((unsigned char)text[i + j]) == tolower((unsigned char)keyword[j])) j++;
        if (j == keyword_length) return 1;
    }
    return 0;
}

static int is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static int days_in_month(int year, int month) {
    static const int table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month < 1 || month > 12) return 30;
    if (month == 2 && is_leap_year(year)) return 29;
    return table[month - 1];
}

void today_str(char *buffer, size_t size) {
    time_t raw = time(NULL);
    struct tm *info = localtime(&raw);
    if (info == NULL || strftime(buffer, size, "%Y-%m-%d", info) == 0) {
        if (size > 0) buffer[0] = '\0';
    }
}

int date_is_valid(const char *date) {
    int year, month, day;
    if (strlen(date) != 10) return 0;
    if (date[4] != '-' || date[7] != '-') return 0;
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit((unsigned char)date[i])) return 0;
    }
    if (sscanf(date, "%d-%d-%d", &year, &month, &day) != 3) return 0;
    if (year < 1900 || year > 3000) return 0;
    if (month < 1 || month > 12) return 0;
    if (day < 1 || day > days_in_month(year, month)) return 0;
    return 1;
}

void date_add_days(const char *date, int days, char *out, size_t out_size) {
    int year, month, day;
    char base[SMALL_TEXT_SIZE];
    if (date_is_valid(date)) snprintf(base, sizeof(base), "%s", date);
    else today_str(base, sizeof(base));
    if (sscanf(base, "%d-%d-%d", &year, &month, &day) != 3) {
        snprintf(out, out_size, "%s", base);
        return;
    }
    day += days;
    while (day > days_in_month(year, month)) {
        day -= days_in_month(year, month);
        month++;
        if (month > 12) { month = 1; year++; }
    }
    snprintf(out, out_size, "%04d-%02d-%02d", year, month, day);
}

static int contains_digit(const char *text) {
    for (; *text != '\0'; text++) if (isdigit((unsigned char)*text)) return 1;
    return 0;
}

void read_name(const char *prompt, char *buffer, size_t size) {
    while (1) {
        read_line(prompt, buffer, size);
        if (buffer[0] == '\0') { printf("姓名不能为空，请重新输入。\n"); continue; }
        if (contains_digit(buffer)) { printf("姓名中不能包含数字，请重新输入。\n"); continue; }
        return;
    }
}

void read_borrow_date(char *buffer, size_t size) {
    char input[SMALL_TEXT_SIZE];
    char today[SMALL_TEXT_SIZE];
    char prompt[SMALL_TEXT_SIZE * 2];
    today_str(today, sizeof(today));
    snprintf(prompt, sizeof(prompt), "借阅日期 [直接回车使用今天 %s]: ", today);
    while (1) {
        read_line(prompt, input, sizeof(input));
        if (input[0] == '\0') { snprintf(buffer, size, "%s", today); return; }
        if (date_is_valid(input)) { snprintf(buffer, size, "%s", input); return; }
        printf("日期格式应为 YYYY-MM-DD（例如 %s），请重新输入或直接回车。\n", today);
    }
}

int confirm_action(const char *action) {
    char input[SMALL_TEXT_SIZE];
    char prompt[SMALL_TEXT_SIZE * 2];
    snprintf(prompt, sizeof(prompt), "你选择了「%s」，确认执行吗？(y/N): ", action);
    read_line(prompt, input, sizeof(input));
    return input[0] == 'y' || input[0] == 'Y';
}
