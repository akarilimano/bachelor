#include <stdio.h>
//#include <cstdlib> 
#include <string.h>
#include <stdio.h>
//#include <iostream.h>
//using namespace std;

// #define PARGRES_COMPILE
#define DEBUG_MODE 1

#ifdef PARGRES_COMPILE
	#include <par_libpq-fe.h>
#else
	#include <postgresql/libpq-fe.h>
#endif

#include "graph500-bfs.h"

void printGraph (PGconn *conn);

int main(int argc, char const *argv[]) {
	PGconn *conn;
	PGresult *res;

	int a,b;
	char query[200];
	char *edgeList;  // file to read edges from

	if (argc != 2) {
		puts("Please specify file name!");
		exit(1);
	}
	
	edgeList = argv[1];
	FILE *in=freopen(edgeList,"r",stdin);
	//while (fgetc(stdin)) != EOF)
	conn = PQconnectdb("dbname=alexander host=localhost user=alexander password=qwerty12");

	if (PQstatus(conn) == CONNECTION_BAD) {
		puts(PQerrorMessage(conn));
		puts("We were unable to connect to the database");
		exit(0);
	}

	//res = PQexec(conn,"update people set phonenumber=\'5055559999\' where id=3");
	res = PQexec(conn,"drop table if exists graph");
	puts("Table Graph dropped");

	res = PQexec(conn,"create table graph (id bigserial primary key,a_id bigint, b_id bigint, parent bigint, queue bigint)");// with (fragattr = id)");
	
	if (PQresultStatus(res) != PGRES_COMMAND_OK) { // Something's wrong...
		puts("Queries failed\n");
		PQclear(res);
		exit(1);
	}
	puts("Table Graph created");

	res = PQexec(conn,"begin transaction");
	while (scanf("%d %d",&a,&b) != EOF ) {
		sprintf(query,"insert into graph(a_id,b_id,parent,queue) values(%d,%d,NULL,NULL)",a,b);
		res = PQexec(conn,query);
	}
	res = PQexec(conn,"commit");

	printGraph(conn);

	PQclear(res);

	PQfinish(conn);

	return 0;
}

void printGraph (PGconn *conn){
	PGresult *res;
	int rec_count;
	int row;
	int col;
	int col_count;

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