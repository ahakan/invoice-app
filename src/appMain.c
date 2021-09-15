// C program to use the above created header file
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include "../inc/sqlite3.h"
#include "../inc/appUser.h"
#include "../inc/appInvoice.h"

#define CURRENT_MAX_SIZE 17
#define CONVERT_CHAR_MAX_SIZE 3

sqlite3 *db;
char *dbFileName = "upload_system.db";

static char currentTime[CURRENT_MAX_SIZE];
static char oldTime[CURRENT_MAX_SIZE];
int UID;
int isLogin = 0;
int isLogout = 0;
int isHandle = 1;

int selectedOperation;


void waitFor (unsigned int secs) {
    unsigned int retTime = time(0) + secs;   // Get finishing time.
    while (time(0) < retTime);               // Loop until it arrives.
}


char *getCurrentTime()
{
    int hours, minutes, seconds, day, month, year;

    // `time_t` is an arithmetic time type
    time_t now;

    // `time()` returns the current time of the system as a `time_t` value
    time(&now);

    struct tm *local = localtime(&now);

    hours = local->tm_hour;         // get hours since midnight (0-23)
    minutes = local->tm_min;        // get minutes passed after the hour (0-59)

    day = local->tm_mday;            // get day of month (1 to 31)
    month = local->tm_mon + 1;      // get month of year (0 to 11)
    year = local->tm_year + 1900;   // get year since 1900

    // Convert the month
    char sMonth[CONVERT_CHAR_MAX_SIZE];
    if (month < 10)
        snprintf(sMonth, CONVERT_CHAR_MAX_SIZE, "0%d", month);
    else
        snprintf(sMonth, CONVERT_CHAR_MAX_SIZE, "%d", month);

    // Convert the day
    char sDay[CONVERT_CHAR_MAX_SIZE];
    if (month < 10)
        snprintf(sDay, CONVERT_CHAR_MAX_SIZE, "0%d", day);
    else
        snprintf(sDay, CONVERT_CHAR_MAX_SIZE, "%d", day);

    // Convert the hour
    char sHours[CONVERT_CHAR_MAX_SIZE];
    if (hours < 10)
        snprintf(sHours, CONVERT_CHAR_MAX_SIZE, "0%d", hours);
    else
        snprintf(sHours, CONVERT_CHAR_MAX_SIZE, "%d", hours);

    // Convert the minute
    char sMinutes[CONVERT_CHAR_MAX_SIZE];
    if (minutes < 10)
        snprintf(sMinutes, CONVERT_CHAR_MAX_SIZE, "0%d", minutes);
    else
        snprintf(sMinutes, CONVERT_CHAR_MAX_SIZE, "%d", minutes);

    snprintf(currentTime, CURRENT_MAX_SIZE, "%d-%s-%s %s:%s", year, sMonth, sDay, sHours, sMinutes);

    return currentTime;
}


// Connect the DB
void connectDB()
{
    int rc = sqlite3_open(dbFileName, &db);

    if( rc )
    {
        printf("Can not open DB.\n");
        exit(0);
    }
    // printf("Connected DB successfully\n");

    sqlite3_busy_timeout(db, 100);
}


void handle_sigint(int sig)
{
    printf("\n");

    if ( isHandle == 1 )
        insertLogoutTime(db, UID, getCurrentTime(), oldTime);

    sqlite3_close_v2(db);

    exit(0);
}


int main()
{
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigint);
    signal(SIGQUIT, handle_sigint);
    signal(SIGKILL, handle_sigint);
    signal(SIGTSTP, handle_sigint);
    signal(SIGABRT, handle_sigint);

    while( 1 )
    {
        int loopVariable=1;

        // Login
        while( loopVariable )
        {
            int id;
            char *password;

            // If user logged out update LogoutTime
            if( isLogout == 1 )
            {
                if( insertLogoutTime(db, UID, getCurrentTime(), oldTime) == 1 )
                {
                    isLogin = 0;
                    isLogout = 0;
                    isHandle = 0;

                    // Close the DB
                    sqlite3_close_v2(db);
                }
                else
                {
                    // Connect the DB
                    connectDB();
                }
            }

            if ( isLogin != 1)
            {
                // Enter user id and password
                printf("Enter User Id:");
                scanf("%d", &id);
                UID = id;

                printf("Enter Password:");
                scanf("%s", password);
            }

            // Check id and password length
            if( id > 0 && strlen(password) > 0 )
            {
                // Connect the DB
                connectDB();

                // Login
                isLogin = login(db, id, password);
            }

            // Delay
            waitFor(2);

            // If login success
            if( isLogin == 1 )
            {
                // Insert Login time
                if( insertLoginTime(db, id, getCurrentTime()) == 1 )
                {
                    snprintf(oldTime, CURRENT_MAX_SIZE, "%s", currentTime);
                    loopVariable = 0;
                }
                else
                {
                    // Close the DB
                    sqlite3_close_v2(db);

                    // Connect the DB
                    connectDB();
                }
            }
            else
            {
                // Close the DB
                sqlite3_close_v2(db);
            }
        }

        while( 1 )
        {
            char *operationList = "Select Operation\n1 - Invoice List\n2 - Logout\n";

            printf("%s", operationList);

            scanf("%d", &selectedOperation);

            printf("\n");

            if( selectedOperation == 1 )
            {
                while( 1 )
                {
                    // Scan directory
                    scanDir();

                    char ch;

                    scanf("%s", &ch);

                    // If pressed ESC
                    if( ch == 27 )
                    {
                        printf("Pressed ESC\n");
                        break;
                    }
                    else
                    {
                        // Check invoice and if invoice doesn't exist wait 2 seconds
                        if( printInvoice( atoi((const char *)&ch) ) == 0 )   // char to int
                        {
                            // Delay
                            waitFor(2);
                        }
                        else
                        {
                            // Delay for if invoice exist
                            waitFor(10);
                        }
                    }
                }
            }
            else if( selectedOperation == 2 )
            {
                // Log out
                isLogout = 1;
                loopVariable = 1;

                break;
            }
        }
    }

    // Close the DB
    sqlite3_close_v2(db);

    return 0;
}