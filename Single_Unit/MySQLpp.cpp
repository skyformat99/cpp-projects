#include "MySQLpp.h"

// * * * * * * * * * * * * * * * *
// * Constructors
// * * * * * * * * * * * * * * * *

MySQLpp :: MySQLpp ()
{
  conn = NULL;
  connected = false;
  debugging = false;
  logging = false;
}

MySQLpp :: MySQLpp (string host, string user, string pwd, 
    string db, int port)
{
  conn = NULL;
  connected = false;
  debugging = false;
  logging = false;

  connectToServer (host,user,pwd,db,port);
}

MySQLpp :: ~MySQLpp ()
{
  if (connected)
    mysql_close(conn);

  if (logging) {
    if (logstream->is_open())
      logstream->close();
  }
}

// * * * * * * * * * * * * * * * *
// * Public Methods
// * * * * * * * * * * * * * * * *

void MySQLpp :: connectToServer (string host, string user, 
    string pwd, string db, int port) throw (char*)
{
  if ((conn = mysql_init(conn)) == NULL)
    throw "Could not initialize MYSQL connection";

  if (!mysql_real_connect(conn, host.c_str(), user.c_str(), pwd.c_str(), 
         db.c_str(), port, NULL, 0)) 
    throw "Could not connect to MYSQL server";

  connected = true;
}

void MySQLpp :: execQuery (string* sql_buf, stringstream* errorMessage)
{
  execQuery(sql_buf,errorMessage,logging);
}

void MySQLpp :: execQuery (string* sql_buf, stringstream* errorMessage,
    bool loggingOverride)
{
  int result;
  result = mysql_real_query(conn,sql_buf->c_str(),sql_buf->length());

  if (result != 0) 
    writeAndThrowErrorMessage(errorMessage,debugging);

  if (loggingOverride) {
    try {
      (*logstream) << "Exec Query: "
                   << *sql_buf << endl;
    } catch (...) {
      throw "Error writing to log stream!";
    }
  }
}

/**
 * Initializes the debugging mechanism, such that when a query fails, a
 * debug message is sent to the database in the given dB, table, and column.
 * Can be overridden at insert time.
 */
void MySQLpp :: initializeDebugging (string debug_db, string debug_table, 
    string debug_column)
{ 
  this->debug_db = debug_db;
  this->debug_table = debug_table;
  this->debug_column = debug_column;

  debugging = true;
}

/**
 * Initializes the logging option given a filestream, already
 * open for writing.
 */
void MySQLpp :: initializeLogging (fstream* logStream)
{ 
  this->logstream = logStream;

  logging = true;
}

/**
 * Initialize the logging option given a string filename. This
 * assumes that the file has not been opened by a filestream previously
 * in this process. 
 */
void MySQLpp :: initializeLogging (string filename) 
{ 
  try {
    logstream = new fstream(filename.c_str(), fstream::out | fstream::app);
  } catch (...) {
    throw "Error opening file for logging";
  } 
}

/**
 * Inserts a single item using the query in the buffer, with the
 * human-readable error message. The default debugging option is used.
 */
int MySQLpp :: insertSingleItem (string* sql_buf, stringstream* errorMessage)
{ 
  return insertSingleItem(sql_buf,errorMessage,debugging);
}

int MySQLpp :: insertSingleItem (string* sql_buf, stringstream* errorMessage,
    bool debugOverride)
{
  return insertSingleItem(sql_buf,errorMessage,debugOverride,logging);
}

/**
 * Inserts a single item using the insert query in the sql_buffer. The 
 * errorMessage stream contains a human-readable error message. The 
 * debugOverride boolean tells whether or not to send the error message to 
 * the database in the default debug table. If logging is enabled, the queries are
 * logged to the log_file.
 */
int MySQLpp :: insertSingleItem (string* sql_buf, stringstream* errorMessage, 
    bool debugOverride, bool loggingOverride) 
{ 
  int result;
  result = mysql_real_query(conn,sql_buf->c_str(),sql_buf->length());

  if (result != 0) 
    writeAndThrowErrorMessage(errorMessage,debugOverride);

  int affectedRows = mysql_affected_rows(conn);

  if (affectedRows != 1) 
    writeAndThrowErrorMessage(errorMessage,debugOverride);

  int insert_id = mysql_insert_id(conn);

  if (loggingOverride) {
    try {
      (*logstream) << "Single Query: " << (*sql_buf) 
                << " ID: " << insert_id << endl;
    } catch (...) {
      throw "Error writing to log stream!";
    }
  }

  return insert_id;
}

/**
 * Inserts a large-item using the query in the buffer, with the
 * human-readable error message. The default debugging option is used.
 */
void MySQLpp :: insertMultiItem (string* sql_buf, stringstream* errorMessage)
{
  insertMultiItem(sql_buf,errorMessage,debugging);
}

void MySQLpp :: insertMultiItem (string* sql_buf, stringstream* errorMessage, 
    bool debugOverride)
{
  insertMultiItem(sql_buf,errorMessage,debugOverride,logging);
}

/**
 * Inserts a large-item using the query in the buffer, with the
 * human-readable error message. The debugging option is declared with 
 * the debugOverride argument. This insert does not look for a last-insert-id,
 * and therefore does not have any return arguments. The logging is truncated at 
 * 200 characters to keep the log human-readable.
 */
void MySQLpp :: insertMultiItem (string* sql_buf, stringstream* errorMessage, 
    bool debugOverride, bool loggingOverride)
{ 
  int result;
  result = mysql_real_query(conn,sql_buf->c_str(),sql_buf->length());

  if (result != 0) 
    writeAndThrowErrorMessage(errorMessage,debugOverride);

  if (loggingOverride) {
    try {
      (*logstream) << "Single Large Query (Truncated at 200): "
                   << sql_buf->substr(0,200) << endl;
    } catch (...) {
      throw "Error writing to log stream!";
    }
  }

  return;
} 

void MySQLpp :: querySingleColumn (string* query, vector<string>* vec) 
{
  int result;
  result = mysql_real_query(conn,query->c_str(),query->length());

  if (result != 0) 
    throw "Error querying database!";

  MYSQL_RES *res;
  // Verify that the result is present, and that fields are present
  if ((res = mysql_store_result(conn)) == NULL) {
    if (mysql_field_count(conn) == 0) {
      throw "No fields returned from query!";
    } else {
      throw "MYSQL ERROR!";
    }
  }

  long nrow, nfield;

  // Get number of rows, fields in result set
  nrow = mysql_num_rows(res);
  nfield = mysql_num_fields(res);

  if (nfield > 1)
    throw "Error: fields > 1 for single-column query";

  if (nrow == 0) {
    mysql_free_result(res);  
    return;
  }

  MYSQL_FIELD *f;
  // Get the fields
  f = mysql_fetch_fields(res);

  if (f == NULL) 
    throw "Error fetching fields from query!";

  MYSQL_ROW row;
  row = mysql_fetch_row(res);
  if (row == NULL) 
    throw "Error fetching row from query!";

  stringstream ss;

  int i;
  for (i=0;i<nrow;i++) {
    mysql_data_seek(res,i);

    row = mysql_fetch_row(res);
    if (row == NULL) 
      throw "Error fetching row from query!";

    ss.str("");
    ss << row[0];

    vec->push_back(ss.str().c_str());
  }

  mysql_free_result(res); 
}      

// * * * * * * * * * * * * * * * *
// * Private Methods
// * * * * * * * * * * * * * * * *

/**
 * Given a human-readable error message and a boolean option to send
 * this message to the dB or not, the dB is pinged & the error message
 * sent, the log is written (if enabled), and the exception is thrown.
 */
void MySQLpp :: writeAndThrowErrorMessage (stringstream* errorMessage, 
    bool debugToDatabase) throw (char*)
{
  stringstream debugMsg;
  int result;

  if (debugToDatabase) {
    result = mysql_ping(conn);
    if (result != 0) {
      debugMsg.str("");
      debugMsg << "Error pinging MYSQL server on error write: " 
               << errorMessage->str();

      throw debugMsg.str().c_str();
    }

    debugMsg.str("");
    debugMsg << "INSERT INTO " << debug_db << "." << debug_table
             << "(" << debug_column << ") VALUES ('" 
             << errorMessage->str() << "')";
    mysql_real_query(conn,debugMsg.str().c_str(),debugMsg.str().length());
  }

  debugMsg.str("");
  debugMsg << "Error during SQL insertSingle: " << errorMessage->str();

  if (logging) {
    try {
      (*logstream) << debugMsg.str() << endl;
    } catch (...) {
      throw "Error writing to log stream!";
    }
  }

  throw debugMsg.str().c_str();
}