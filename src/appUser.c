#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "../inc/appUser.h"

int login(sqlite3 *db, int id, char* password)
{
    char *sql;
    sqlite3_stmt *res;

    // Create SQL statement
    sql = "SELECT *\
        FROM users\
        WHERE ID = ?";

    // Execute SQL statement
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    if (rc == SQLITE_OK)
    {
        sqlite3_bind_int(res, 1, id);
    }
    else
    {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }

    int step = sqlite3_step(res);

    if (step == SQLITE_ROW)
    {
        if( strcmp ( (char *)sqlite3_column_text(res, 2), password ) == 0 )
        {

            printf("User : [");
            printf("%s ", sqlite3_column_text(res, 0));
            printf("%s", sqlite3_column_text(res, 1));
            printf("] \n");

            sqlite3_finalize(res);

            return 1;
        }
    }

    sqlite3_finalize(res);

    printf("Login failed \n");

    return 0;
}


void insertLoginTime(sqlite3 *db, int UID, char *currentTime)
{
    char *sql;
    sqlite3_stmt *res;

    // Create SQL statement
    sql = "INSERT INTO audit(UserId, LoginTime)\
        VALUES( ?, ? )";

    // Execute SQL statement
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, NULL);

    if( rc == SQLITE_OK )
    {
        sqlite3_bind_int(res, 1, UID);
        sqlite3_bind_text(res, 2, currentTime, 16, SQLITE_STATIC);
    }
    else
    {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }

    rc = sqlite3_step(res);

    if ( rc != SQLITE_DONE ) {
        printf("SQL query error! Code: %d\n", rc);
    }

    sqlite3_finalize(res);
}

void insertLogoutTime(sqlite3 *db, int UID, char *currentTime, char *loginTime)
{
    char *sql;
    sqlite3_stmt *res;

    // Create SQL statement
    sql = "SELECT *\
        FROM audit\
        WHERE UserId = ? and LoginTime = ?";

    // Execute SQL statement
    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    if( rc == SQLITE_OK )
    {
        sqlite3_bind_int(res, 1, UID);
        sqlite3_bind_text(res, 2, loginTime, 16, SQLITE_STATIC);
    }
    else
    {
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }

    int step = sqlite3_step(res);

    if (step == SQLITE_ROW)
    {
        char *OID = (char *)sqlite3_column_text(res, 0);

        int convertOID = atoi(OID);

        // Create SQL statement
        sql = "UPDATE audit\
            SET LogoutTime = ?\
            WHERE OID = ? ";

        // Execute SQL statement
        int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

        if( rc == SQLITE_OK )
        {
            sqlite3_bind_text(res, 1, currentTime, 16, SQLITE_STATIC);
            sqlite3_bind_int(res, 2, convertOID);
        }
        else
        {
            fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        }

        int step = sqlite3_step(res);
    }

    sqlite3_finalize(res);

}