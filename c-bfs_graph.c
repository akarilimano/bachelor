#include <stdio.h>
//#include <cstdlib> 
#include <postgresql/libpq-fe.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
//#include <iostream.h>
//using namespace std;

void printGraph (PGconn *conn, bool noprint);

int main (int argc, char const *argv[]) {
	PGconn *conn;
	PGresult *res;

	int startNode,goalNode;
	char query[200];
	bool noprint = false;
	
	conn = PQconnectdb("dbname=alexander host=localhost user=alexander password=qwerty12");

	if (PQstatus(conn) == CONNECTION_BAD) {
		puts(PQerrorMessage(conn));
		puts("We were unable to connect to the database");
		exit(0);
	}

	if (argc != 3) {
		if (argc != 4) {
			puts("Please specify startNode and goalNode!");
			exit(1);
		}
		noprint = true;
	}

	startNode = argv[1];
	goalNode = argv[2];

	res = PQexec(conn,"UPDATE Graph SET parent=NULL, queue=NULL");
	sprintf(query,"update graph set parent=-1, queue=0 where a_id=%s",startNode);
	res = PQexec(conn,query);
	puts("Initial State");
	printGraph(conn, noprint);

	PQclear(res);
	PQfinish(conn);
	return 0;
}

void printGraph (PGconn *conn, bool noprint) {
	PGresult *res;
	int rec_count;
	int row;
	int col;
	int col_count;

	if (noprint)
		return;

	res = PQexec(conn,"select * from graph order by a_id");

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		puts("We did not get any data!");
		exit(0);
	}

	rec_count = PQntuples(res);
	col_count = PQnfields(res);

	//printf("We received %d records.\n", rec_count);

	puts("=============================");
	for (col=0; col<col_count; col++) {
		printf("%s\t", PQfname(res, col));
	}
	puts("");

	puts("=============================");
	for (row=0; row<rec_count; row++) {
		for (col=0; col<col_count; col++) {
			printf("%s\t", PQgetvalue(res, row, col));
		}
		puts("");
	}
	puts("=============================");

	PQclear(res);
}