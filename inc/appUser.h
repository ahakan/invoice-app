

int login(sqlite3 *db, int id, char* password);

int insertLoginTime(sqlite3 *db, int UID, char *currentTime);

int insertLogoutTime(sqlite3 *db, int UID, char *currentTime, char *loginTime);
