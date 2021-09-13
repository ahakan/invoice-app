

int login(sqlite3 *db, int id, char* password);

void insertLoginTime(sqlite3 *db, int UID, char *currentTime);

void insertLogoutTime(sqlite3 *db, int UID, char *currentTime, char *loginTime);