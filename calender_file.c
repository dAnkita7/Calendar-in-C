#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_DAYS 31
#define MAX_REMINDERS 100 // Increased the maximum number of reminders
#define COLOR_RESET   "\x1B[0m"
#define COLOR_RED     "\x1B[31m"
#define COLOR_GREEN   "\x1B[32m"
#define COLOR_YELLOW  "\x1B[33m"
#define COLOR_BLUE    "\x1B[34m"
#define COLOR_MAGENTA "\x1B[35m"
#define COLOR_CYAN    "\x1B[36m"
#define COLOR_WHITE   "\x1B[37m"
#define COLOR_GRAY    "\x1B[90m"
#define COLOR_BG_BLUE "\x1B[44m"
#define COLOR_BG_YELLOW "\x1B[43m"

struct Reminder {
    int year;
    int month;
    int day;
    char message[50];
    int repeat_yearly; // Flag to indicate whether reminder should be repeated every year
};

int isLeapYear(int year) {
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
        return 1;
    else
        return 0;
}

int daysInMonth(int month, int year) {
    int days;

    switch(month) {
        case 4: case 6: case 9: case 11:
            days = 30;
            break;
        case 2:
            days = (isLeapYear(year)) ? 29 : 28;
            break;
        default:
            days = 31;
    }

    return days;
}

void saveReminders(struct Reminder reminders[], int num_reminders) {
    FILE *file = fopen("reminders.txt", "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    for (int i = 0; i < num_reminders; i++) {
        fprintf(file, "%d %d %d %d %s\n", reminders[i].year, reminders[i].month, reminders[i].day, reminders[i].repeat_yearly, reminders[i].message);
    }

    fclose(file);
}

int loadReminders(struct Reminder reminders[]) {
    FILE *file = fopen("reminders.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }

    int num_reminders = 0;
    while (fscanf(file, "%d %d %d %d %[^\n]s", &reminders[num_reminders].year, &reminders[num_reminders].month, &reminders[num_reminders].day, &reminders[num_reminders].repeat_yearly, reminders[num_reminders].message) != EOF) {
        if (reminders[num_reminders].repeat_yearly && reminders[num_reminders].year != localtime(&(time_t){time(NULL)})->tm_year + 1900) {
            // Skip loading reminder if it should repeat yearly and it's not for the current year
            continue;
        }
        num_reminders++;
    }

    fclose(file);
    return num_reminders;
}

void printCalendar(int month, int year, struct Reminder reminders[], int num_reminders) {
    char *months[] = {"January", "February", "March", "April", "May", "June", "July",
                      "August", "September", "October", "November", "December"};

    printf("\n%s%20s %d%s\n", COLOR_RED, months[month - 1], year, COLOR_RESET);
    printf("%sSun\tMon\tTue\tWed\tThu\tFri\tSat%s\n", COLOR_CYAN, COLOR_RESET);

    // Get the first day of the month
    struct tm first_day = {0};
    first_day.tm_year = year - 1900;
    first_day.tm_mon = month - 1;
    first_day.tm_mday = 1;
    mktime(&first_day);

    // Print spaces for the days before the 1st of the month
    for (int i = 0; i < first_day.tm_wday; i++)
        printf("\t");

    // Print dates and highlight reminders, Sundays, and Saturdays
    for (int i = 1; i <= daysInMonth(month, year); i++) {
        int isReminder = 0;
        int reminder_index = -1;
        struct tm current_date = first_day;
        current_date.tm_mday = i;
        mktime(&current_date);
        
        int isSunday = (current_date.tm_wday == 0); // Sunday
        int isSaturday = (current_date.tm_wday == 6); // Saturday

        for (int j = 0; j < num_reminders; j++) {
            if (reminders[j].day == i && reminders[j].month == month && reminders[j].year == year) {
                isReminder = 1;
                reminder_index = j;
                break;
            }
        }

        if (isReminder) {
            printf(COLOR_MAGENTA "%d\t" COLOR_RESET, i); // Highlight reminder day with magenta
            if (reminders[reminder_index].repeat_yearly) {
                reminders[reminder_index].year = year + 1; // Move the reminder to next year
            }
        } else if (isSunday)
            printf(COLOR_RED "%d\t" COLOR_RESET, i); // Highlight Sunday with red
        else if (isSaturday)
            printf(COLOR_GREEN "%d\t" COLOR_RESET, i); // Highlight Saturday with green
        else
            printf("%d\t", i);

        if ((i + first_day.tm_wday) % 7 == 0) printf("\n");
    }
    printf("\n");
}

int main() {
    int month, year;
    struct Reminder reminders[MAX_REMINDERS];
    int num_reminders = 0;

    // Load reminders from file
    num_reminders = loadReminders(reminders);

    // Get current system time
    time_t t = time(NULL);
    struct tm *timeinfo = localtime(&t);
    month = timeinfo->tm_mon + 1; // Months are 0 to 11, so adding 1
    year = timeinfo->tm_year + 1900; // Years since 1900

    printf("Today's Date: %d-%02d-%02d\n", year, month, timeinfo->tm_mday);

    // Additional code for calendar navigation and reminder handling (not shown for brevity)

    char choice;
    do {
        printCalendar(month, year, reminders, num_reminders);
        printf("\nOptions:\n");
        printf("N - Next month\n");
        printf("P - Previous month\n");
        printf("B - Choose a specific month\n");
        printf("S - Set a reminder\n");
        printf("D - Delete a reminder\n");
        printf("V - View all reminders for the year\n");
        printf("Q - Quit\n");
        printf("Enter your choice: ");
        scanf(" %c", &choice);
        switch (choice) {
            case 'N':
            case 'n':
                month++;
                if (month > 12) {
                    month = 1;
                    year++;
                }
                break;
            case 'P':
            case 'p':
                month--;
                if (month < 1) {
                    month = 12;
                    year--;
                }
                break;
            case 'B':
            case 'b':
                printf("Enter year: ");
                scanf("%d", &year);
                printf("Enter month (1-12): ");
                scanf("%d", &month);
                break;
            case 'S':
            case 's':
                if (num_reminders < MAX_REMINDERS) {
                    struct Reminder newReminder;
                    printf("Enter the day you want to set a reminder for (1-%d): ", daysInMonth(month, year));
                    scanf("%d", &newReminder.day);
                    newReminder.month = month;
                    newReminder.year = year;
                    printf("Enter reminder message: ");
                    scanf(" %[^\n]s", newReminder.message);
                    printf("Repeat yearly? (1 for Yes, 0 for No): ");
                    scanf("%d", &newReminder.repeat_yearly);
                    reminders[num_reminders++] = newReminder;
                } else {
                    printf("Maximum number of reminders reached!\n");
                }
                break;
            case 'D':
            case 'd':
                if (num_reminders > 0) {
                    int dayToDelete;
                    printf("Enter the day of the reminder you want to delete: ");
                    scanf("%d", &dayToDelete);
                    for (int i = 0; i < num_reminders; i++) {
                        if (reminders[i].day == dayToDelete && reminders[i].month == month && reminders[i].year == year) {
                            for (int j = i; j < num_reminders - 1; j++) {
                                reminders[j] = reminders[j + 1];
                            }
                            num_reminders--;
                            break;
                        }
                    }
                } else {
                    printf("No reminders to delete.\n");
                }
                break;
            case 'V':
            case 'v':
                if (num_reminders > 0) {
                    printf("Reminders for %d-%02d:\n", year, month);
                    for (int i = 0; i < num_reminders; i++) {
                        if (reminders[i].year == year && reminders[i].month == month) {
                            printf("Day %d: %s\n", reminders[i].day, reminders[i].message);
                        }
                    }
                } else {
                    printf("No reminders available.\n");
                }
                break;
            case 'Q':
            case 'q':
                printf("Exiting program.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }

        // Save reminders to file after each operation
        saveReminders(reminders, num_reminders);

    } while (choice != 'Q' && choice != 'q');

    return 0;
}
