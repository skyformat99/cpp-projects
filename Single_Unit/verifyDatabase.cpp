#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "NumRecipe.h"
#include "Timer.h"

/** Required for MYSQL calls */
#ifdef WIN32
#include <windows.h>
#endif

#include "mysql.h"

//#define PE          /** Use PE test set */
#define SPATIALGAP  /** Use SpatialGap test set */

#define MAX_SQL_BUFFER 512
#define MAX_TANK_NAME 10

/** If true, prints a line for every index skip in PSTH matching */
#define PRINT_SKIPS true

/** Max number of skips allowed during one PSTH comparison run */
#define MAX_SKIPS 10

static double maxarg1,maxarg2;
#define FMAX(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ?\
(maxarg1) : (maxarg2))
static double minarg1,minarg2;
#define FMIN(a,b) (minarg1=(a),minarg2=(b),(minarg1) < (minarg2) ?\
(minarg1) : (minarg2))

#define DEBUG true

typedef struct _vd_long_result {
  int err;
  long numRows;
  long numFields;
  long ** result;

  _vd_long_result () {
    result = NULL;
  }

  ~_vd_long_result () {
    if (result != NULL) 
      free_l_matrix(result,0,numRows,0,numFields);
  }  
} V_LONG_RES;

/** Queries the connection and returns a single long value */
int mysql_query_single_long (MYSQL*, char*, long*);

/** Queries the connection and fills long matrix [nrow][nfields] */
V_LONG_RES* mysql_query_multi_long (MYSQL*, char*);

/** Queries the connection and fills one char* matrix with nrows of strings */
char** mysql_query_multi_char (MYSQL*, char*, int*, int*);

/** Determines whether two psth char strings match */
bool psthMatch (char*, char*);

/** Returns the lowercase representation of this character array */
char* to_lower (char*);

/**
 * Main starting method. No runtime arguments.
 */
int main (int argc, char* argv[]) {

  // Declare timer
  Timer *t = new Timer();

  // Create and init MYSQL connections
  MYSQL *local = NULL, *remote = NULL;
  if ((local = mysql_init(local)) == NULL) {
    fprintf(stderr,"Could not init local MYSQL connection\n");
    exit(1);
  }

  if ((remote = mysql_init(remote)) == NULL) {
    fprintf(stderr,"Could not init remote MYSQL connection\n");
    exit(1);
  }


  // Now initialize connections
  if (!mysql_real_connect(local, "localhost", "aae", "aae", 
         "aae", 0, NULL, 0)) {
    fprintf(stderr,"Could not connect to local MYSQL server\n");
    exit(1);
  }

#ifdef PE
  if (!mysql_real_connect(remote, "172.18.232.4", "joe", "joepass", 
         "pe", 0, NULL, 0)) {
#endif 

#ifdef SPATIALGAP
  if (!mysql_real_connect(remote, "172.18.232.4", "joe", "joepass", 
         "spatialgap", 0, NULL, 0)) {
#endif
    fprintf(stderr,"Could not connect to remote MYSQL server\n");
    exit(1);
  }

  // Declare tanks on which to operate
#ifdef PE
  const int numTanks = 11;
  char tanks[numTanks][MAX_TANK_NAME] = {"070807A", "070807B", 
   "070807C", "070807D", "070807E", "070814A", "070814B", "070814C", 
   "070814D", "070814E", "070814F"};
#endif

#ifdef SPATIALGAP
  const int numTanks = 3;
  char tanks[3][MAX_TANK_NAME] = {"060404A", "060404B", "060404C"};
#endif

  int i, j, k, result, L_ind, R_ind, numSkips;
  char sql_buf[MAX_SQL_BUFFER];
  long L_tankID, R_tankID;
  V_LONG_RES *L_channels, *R_channels;
  V_LONG_RES *L_blocks, *R_blocks;
  V_LONG_RES *L_sweeps, *R_sweeps;
  bool channelsMatch;
  bool blocksMatch;
  bool sweepsMatch;
  char **L_psth, **R_psth;
  int L_numPSTH, R_numPSTH;
  bool psthMatches;

  for (i=0;i<numTanks;i++) {

    // Start timer
    t->reset();

    // * * * * * * * * * * * * * *
    // Get local, remote tank ID
    sprintf_s(sql_buf,MAX_SQL_BUFFER,
      "SELECT t.id FROM aae.tank t WHERE t.name = '%s'",
      tanks[i]);

    result = mysql_query_single_long(local,sql_buf,&L_tankID);

    if (result == 1) {
      printf("Tank %s not found in local dB!\n",tanks[i]);
      continue;
    }
    if (result == -1) continue;

    sprintf_s(sql_buf,MAX_SQL_BUFFER,
      "SELECT e.id FROM experimentlist e WHERE e.Tank = '%s'",
      tanks[i]);

    result = mysql_query_single_long(remote,sql_buf,&R_tankID);

    if (result == 1) {
      printf("Tank %s not found in remote dB!\n",tanks[i]);
      continue;
    }
    if (result == -1) continue;

    if (DEBUG) printf("Tank %s localID: %ld remoteID: %ld\n",tanks[i],L_tankID,R_tankID);

    // * * * * * * * * * * * * * *
    // Get channels, sorts for this tank
    sprintf_s(sql_buf,MAX_SQL_BUFFER,
      "SELECT u.id, u.channel, u.sort FROM aae.unit u WHERE u.tank_id = %d ORDER BY u.channel, u.sort",
      L_tankID);

    L_channels = mysql_query_multi_long(local,sql_buf);

    if (L_channels->err == 1) {
      printf("No channels found for tank %s in local dB\n",tanks[i]);
      continue;
    }
    if (L_channels->err == -1) continue;

    sprintf_s(sql_buf,MAX_SQL_BUFFER,
      "SELECT u.id, u.Chan, u.Sort FROM unitinfo u WHERE u.Tank = '%s' ORDER BY u.Chan, u.Sort",
      tanks[i]);

    R_channels = mysql_query_multi_long(remote,sql_buf);

    if (R_channels->err == 1) {
      printf("No channels found for tank %s in remote dB\n",tanks[i]);
      continue;
    }
    if (R_channels->err == -1) continue;

    // Now verify that channels are the same across both
    if (L_channels->numRows != R_channels->numRows) {
      printf("Number of channels does not match in tank %s (L: %d R: %d)\n",
        tanks[i],L_channels->numRows,R_channels->numRows);
      continue;
    }

    channelsMatch = true;
    for (j=0;j<L_channels->numRows;j++) {
      if (! ((L_channels->result[j][1] == R_channels->result[j][1]) &&
             (L_channels->result[j][2] == R_channels->result[j][2])) ) {
        channelsMatch = false;
        break;
      }
    }
    if (!channelsMatch) {
      printf("Channel numbers/sorts do not match in tank %s\n",tanks[i]);
      continue;
    }

    if (DEBUG) printf("  numChannels: %d <Matched>\n",L_channels->numRows);


    // * * * * * * * * * * * * * *
    // Get blocks for this tank
    sprintf_s(sql_buf,MAX_SQL_BUFFER,
      "SELECT b.id, b.block FROM aae.block b WHERE b.tank_id = %d ORDER BY b.block",
      L_tankID);

    L_blocks = mysql_query_multi_long(local,sql_buf);

    if (L_blocks->err == 1) {
      printf("No blocks found for tank %s in local dB\n",tanks[i]);
      continue;
    }
    if (L_blocks->err == -1) continue;

    sprintf_s(sql_buf,MAX_SQL_BUFFER,
      "SELECT b.rowNum, b.Block FROM blocklist b WHERE b.Tank = '%s' ORDER BY b.Block",
      tanks[i]);

    R_blocks = mysql_query_multi_long(remote,sql_buf);

    if (R_blocks->err == 1) {
      printf("No blocks found for tank %s in remote dB\n",tanks[i]);
      continue;
    }
    if (R_blocks->err == -1) continue;

    // Now verify that blocks match up
    if (L_blocks->numRows != R_blocks->numRows) {
      printf("Mismatch on number of blocks for tank %s\n",tanks[i]);
      continue;
    }

    blocksMatch = true;
    for (j=0;j<L_blocks->numRows;j++) {
      if (L_blocks->result[j][1] != R_blocks->result[j][1]) {
        blocksMatch = false;
        break;
      }
    }
    if (!blocksMatch) {
      printf("Mismatch in block numbers for tank %s\n",tanks[i]);
      continue;
    }

    if (DEBUG) printf("  numBlocks: %d <Matched>\n",L_blocks->numRows);


    // * * * * * * * * * * * * * *
    // Now compare sweep numbers and parameter values for all blocks
    sprintf_s(sql_buf,MAX_SQL_BUFFER,
      "SELECT s.id, s.sweepNum+1, c.param FROM aae.sweep_list s, aae.condition_list c, aae.block b WHERE s.id = c.sweep_list_id AND s.block_id = b.id AND b.tank_id = %d  ORDER BY b.block, s.sweepNum, c.param, c.id",
      L_tankID);

    L_sweeps = mysql_query_multi_long(local,sql_buf);

    if (L_sweeps->err == 1) {
      printf("No sweeps found for tank %s in local dB\n",tanks[i]);
      continue;
    }
    if (L_sweeps->err == -1) continue;

    sprintf_s(sql_buf,MAX_SQL_BUFFER,
      "SELECT c.rowNum, c.sweepNum, CAST(c.paramvalS AS SIGNED) FROM condl_%s c WHERE c.ExpID = %d ORDER BY c.Block, c.sweepNum, c.paramvalS, c.rowNum",
      to_lower(tanks[i]),R_tankID);

    R_sweeps = mysql_query_multi_long(remote,sql_buf);

    if (R_sweeps->err == 1) {
      printf("No sweeps found for tank %s in remote dB\n",tanks[i]);
      continue;
    }
    if (R_sweeps->err == -1) continue;

    // Now compare results
    if (L_sweeps->numRows != R_sweeps->numRows) {
      printf("Number of rows mismatch on sweeps data tank %s",tanks[i]);
      continue;
    }

    sweepsMatch = true;
    for (j=0;j<L_sweeps->numRows;j++) {
      if (! ((L_sweeps->result[j][1] == R_sweeps->result[j][1]) && 
             (L_sweeps->result[j][2] == R_sweeps->result[j][2])) ) {
        sweepsMatch = false;
        break;
      }
    }
    if (!sweepsMatch) {
      printf("Mismatch on sweep numbers / values on tank %s (%d)\n",tanks[i],j);
      continue;
    }

    if (DEBUG) printf("  numSweeps: %d <Matched>\n",L_sweeps->numRows);

    // * * * * * * * * * * * * * *
    // And lastly verify that all of the PSTH data matches
    sprintf_s(sql_buf,MAX_SQL_BUFFER,
      "SELECT u.psth FROM aae.unit_data u, aae.sweep_list s, aae.block b, aae.unit un WHERE u.sweep_list_id = s.id AND s.block_id = b.id AND u.unit_id = un.id AND un.tank_id = b.tank_id AND b.tank_id = %d ORDER BY un.channel, un.sort, b.block, s.sweepNum",
      L_tankID);

    L_psth = mysql_query_multi_char(local,sql_buf,&result,&L_numPSTH);

    if (result == 1) {
      printf("No psth's found for tank %s in local dB\n",tanks[i]);
      continue;
    }
    if (result == -1) continue;

    sprintf_s(sql_buf,MAX_SQL_BUFFER,
      "SELECT u.pstList FROM unitdata_%s u, unitinfo un WHERE u.unitID = un.id AND un.Tank = '%s' ORDER BY un.Chan, un.Sort, u.Block, u.repNum",
      to_lower(tanks[i]),tanks[i]);

    R_psth = mysql_query_multi_char(remote,sql_buf,&result,&R_numPSTH);

    if (result == 1) {
      printf("No psth's found for tank %s in remote dB\n",tanks[i]);
      continue;
    }
    if (result == -1) continue;

    if (DEBUG) printf("  numPSTH: (%d, %d) <Loaded>\n",L_numPSTH,R_numPSTH);

    psthMatches = true;
    L_ind = 0;
    R_ind = 0;
    numSkips = 0;
    while (L_ind < L_numPSTH && R_ind < R_numPSTH) {
      if (psthMatch(L_psth[L_ind],R_psth[R_ind])) {
        L_ind++; R_ind++;
        continue;
      } else {

        if (L_numPSTH > R_numPSTH) {
          if (numSkips < MAX_SKIPS) {
            L_ind++; numSkips++;
            if (PRINT_SKIPS) printf("  ** Skipped L_ind %d\n",L_ind-1);
            continue;
          } else {
            printf("  *** Too many skips at index L: %d R: %d\n",L_ind,R_ind);
            psthMatches = false;
            break;
          }

        } else if (R_numPSTH > L_numPSTH) {
          if (numSkips < MAX_SKIPS) {
            R_ind++; numSkips++;
            if (PRINT_SKIPS) printf("  ** Skipped R_ind %d\n",R_ind-1);
            continue;
          } else {
            printf("  *** Too many skips at index L: %d R: %d\n",L_ind,R_ind);
            psthMatches = false;
            break;
          }

        } else { // have to be equal
          printf("  *** Mismatch on PSTH at index L: %d R: %d\n",L_ind,R_ind);
          psthMatches = false;
          break;
        }
      }
    }
    if (psthMatches)
      t->print("  PSTHs match!");

    printf("\n");

    // ON MISMATCHES USE THIS QUERY LOCALLY (replace <TANK>):
/**
SELECT
  acd.channel, acd.sort, acd.block, acd.sweepNum, ud.psth
FROM 
  (SELECT 
    bun.channel, bun.sort, bun.unit_id, bun.block,
    s.sweepNum, s.id AS sweep_id
  FROM 
    (SELECT 
      un.channel, un.sort,
      un.id AS unit_id, b.block,
      b.id AS block_id 
    FROM
      aae.block b USE INDEX (i_block_tank_id),
      aae.tank t USE INDEX (i_tank_name),
      aae.unit un USE INDEX (i_unit_tank_id)
    WHERE
      b.tank_id = t.id AND
      un.tank_id = t.id AND
      t.name = '<TANK>') AS bun,
    aae.sweep_list s USE INDEX (i_block_id)
  WHERE
    s.block_id = bun.block_id) AS acd,
  aae.unit_data ud USE INDEX (i_unit_sweep)
WHERE
  ud.unit_id = acd.unit_id AND
  ud.sweep_list_id = acd.sweep_id;
**/

    // AND THIS REMOTELY (replace ):
/**
SELECT un.Chan, un.Sort, u.Block, u.repNum, u.pstList 
FROM pe.unitdata_<TANK> u, pe.unitinfo un 
WHERE u.unitID = un.id AND un.Tank = '<TANK>' 
ORDER BY un.Chan, un.Sort, u.Block, u.repNum
**/

    // * * * * * * * * * * * * * *
    // Free data at the end
    delete L_channels;
    delete R_channels;

    delete L_blocks;
    delete R_blocks;

    delete L_sweeps;
    delete R_sweeps;

    for (j=0;j<L_numPSTH;j++)
      free((char*)L_psth[j]);
    free((char*)L_psth);

    for (j=0;j<R_numPSTH;j++)
      free((char*)R_psth[j]);
    free((char*)R_psth);
  }

  // Close out connections
  mysql_close(local);
  mysql_close(remote);

  exit(0);
}

/**
 * Returns -1 on error, 0 on no error when result is populated, 1 if no
 * rows are found matching query.
 */
int mysql_query_single_long (MYSQL* conn, char* sql_buf, long *result) {
  V_LONG_RES* tempResult;

  tempResult = mysql_query_multi_long (conn, sql_buf);

  if (tempResult->err == -1) return -1;
  if (tempResult->err == 1) return 1;

  if (tempResult->err == 2) {
    delete tempResult;
    return -1;
  }

  if (tempResult->numRows != 1 || tempResult->numFields != 1) {
    fprintf(stderr,"Error: single query not single!\n");
    return -1;
  }

  *result = tempResult->result[0][0];
  delete tempResult;

  return 0;
}

V_LONG_RES* mysql_query_multi_long (MYSQL* conn, char* sql_buf) {

  int i, j;
  MYSQL_RES *res;
  MYSQL_FIELD *f;
  MYSQL_ROW row;
  long nrow, nfield;
  V_LONG_RES* val = new V_LONG_RES;

  // Make the query; if result is non-zero, print error
  if (mysql_real_query(conn,sql_buf,strlen(sql_buf)) != 0) {
    fprintf(stderr,"MYSQL ERROR: %s\n",mysql_error(conn));
    val->err = -1;
    return val;
  }

  // Verify that the result is present, and that fields are present
  if ((res = mysql_store_result(conn)) == NULL) {
    if (mysql_field_count(conn) == 0) {
      fprintf(stderr,"No Fields Returned\n");
      val->err = -1;
      return val;
    } else {
      fprintf(stderr,"MYSQL ERROR: %s\n",mysql_error(conn));
      val->err = -1;
      return val;
    }
  }

  // Return 1 if no rows are found.
  if (mysql_num_rows(res) == 0) {
    val->err = 1;
    return val;
  }

  // Get number of rows, fields in result set
  nrow = mysql_num_rows(res);
  nfield = mysql_num_fields(res);

  // Get the fields
  f = mysql_fetch_fields(res);

  if (f == NULL) {
    fprintf(stderr,"Error fetching fields for tank\n");
    val->err = -1;
    return val;
  }

  for (i=0;i<nfield;i++) {
    if (f[i].type != FIELD_TYPE_LONG &&
        f[i].type != FIELD_TYPE_SHORT &&
        f[i].type != FIELD_TYPE_TINY &&
        f[i].type != FIELD_TYPE_LONGLONG &&
        f[i].type != FIELD_TYPE_INT24) {
      fprintf(stderr,"Error on multi long - type not long!\n");
      val->err = -1;
      return val;
    }
  }

  // Declare output array - *** MUST BE FREE'D BY CALLER ***
  val->numRows = nrow;
  val->numFields = nfield;
  val->result = l_matrix(0,nrow,0,nfield);

  // Seek to the first row
  mysql_data_seek(res,0);

  // Get data, fill result
  for (i=0;i<nrow;i++) {
    row = mysql_fetch_row(res);

    for (j=0;j<nfield;j++) {

      if (sscanf_s(row[j],"%ld",&(val->result[i][j])) != 1) {
        fprintf(stderr,"Error scanning long value from row\n");
        val->err = -2;
        return val;
      }

    } // end field iteration

  } // end row iteration

  // Free the result set
  mysql_free_result(res);

  val->err = 0;
  return val;
}

char** mysql_query_multi_char (MYSQL* conn, char* sql_buf, 
         int* err, int* numRows) {

  int i;
  MYSQL_RES *res;
  MYSQL_FIELD *f;
  MYSQL_ROW row;
  long nrow, nfield;

  // Make the query; if result is non-zero, print error
  if (mysql_real_query(conn,sql_buf,strlen(sql_buf)) != 0) {
    fprintf(stderr,"MYSQL ERROR: %s\n",mysql_error(conn));
    *err = -1;
    return NULL;
  }

  // Verify that the result is present, and that fields are present
  if ((res = mysql_store_result(conn)) == NULL) {
    if (mysql_field_count(conn) == 0) {
      fprintf(stderr,"No Fields Returned\n");
      *err = -1;
      return NULL;
    } else {
      fprintf(stderr,"MYSQL ERROR: %s\n",mysql_error(conn));
      *err = -1;
      return NULL;
    }
  }

  // Return 1 if no rows are found.
  if (mysql_num_rows(res) == 0) {
    *err = 1;
    return NULL;
  }

  // Get number of rows, fields in result set
  nrow = mysql_num_rows(res);
  nfield = mysql_num_fields(res);

  if (nfield > 1) {
    fprintf(stderr,"Should only have one (text) field\n");
    *err = -1;
    return NULL;
  }    

  // Get the fields
  f = mysql_fetch_fields(res);

  if (f == NULL) {
    fprintf(stderr,"Error fetching fields for tank\n");
    *err = -1;
    return NULL;
  }

  if (f[0].type != FIELD_TYPE_STRING &&
      f[0].type != FIELD_TYPE_VAR_STRING &&
      f[0].type != MYSQL_TYPE_BLOB) {
    fprintf(stderr,"Error on multi char - type not string! (%d)\n",f[0].type);
    *err = -1;
    return NULL;
  }

  // Declare first dimension of outputs
  *numRows = nrow;
  char** out = (char**)malloc((size_t)nrow*sizeof(char*));

  // Seek to the first row
  mysql_data_seek(res,0);

  // Get data, fill result
  for (i=0;i<nrow;i++) {
    row = mysql_fetch_row(res);

    out[i] = (char*)malloc((size_t)(strlen(row[0])+1)*sizeof(char));

    strcpy_s(out[i],strlen(row[0])+1,row[0]);

  } // end row iteration

  // Free the result set
  mysql_free_result(res);

  *err = 0;
  return out;
}

/**
 * Returns true if PSTH's match, false otherwise.
 */
bool psthMatch (char* a, char* b) {
  // First copy the incoming strings out of their current arrays
  char *c = new char[strlen(a)+1];
  char *d = new char[strlen(b)+1];
  sprintf_s(c,strlen(a)+1,"%s",a);
  sprintf_s(d,strlen(b)+1,"%s",b);

  char *token1, *token2, *next_token1, *next_token2;
  char delim[] = " \t\n";

  token1 = strtok_s(c,delim,&next_token1);
  token2 = strtok_s(d,delim,&next_token2);

  if (token1 == NULL && token2 != NULL) return false;
  if (token1 != NULL && token2 == NULL) return false;

  while ((token1 != NULL) && (token2 != NULL)) {
    if (atol(token1) != atol(token2)) return false;

    token1 = strtok_s(NULL,delim,&next_token1);
    token2 = strtok_s(NULL,delim,&next_token2);

    if (token1 == NULL && token2 != NULL) return false;
    if (token1 != NULL && token2 == NULL) return false;    
  }

  delete[] c;
  delete[] d;

  return true;
}

/**
 * Returns the lower-case string representation of the input string.
 */
char* to_lower (char* in) {
  int i, len = strlen(in);

  char *out = new char[len];

  for (i=0;i<len;i++) {
    out[i] = (char)tolower(in[i]);
  }

  return out;
}