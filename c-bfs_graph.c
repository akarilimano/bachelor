#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

// #define PARGRES_COMPILE
#define DEBUG_MODE 1

#ifdef PARGRES_COMPILE
	#include <par_libpq-fe.h>
#else
	#include <postgresql/libpq-fe.h>
#endif

#include "graph500-bfs.h"

#ifdef DEBUG_MODE
	#define PRINTF(MY_FORMAT,MY_STRING) \
		printf(MY_FORMAT, MY_STRING);

	void printGraph (PGconn *conn) {
		PGresult *res;
		int res_count;
		int row;
		int col;
		int col_count;

		res = PQexec(conn,"select * from graph order by a_id");

		if (PQresultStatus(res) != PGRES_TUPLES_OK) {
			puts("We did not get any data!");
			exit(0);
		}

		res_count = PQntuples(res);
		col_count = PQnfields(res);

		puts("=============================");
		for (col=0; col<col_count; col++) {
			printf("%s\t", PQfname(res, col));
		}
		puts("");

		puts("=============================");
		for (row=0; row<res_count; row++) {
			for (col=0; col<col_count; col++) {
				printf("%s\t", PQgetvalue(res, row, col));
			}
			puts("");
		}
		puts("=============================");

		PQclear(res);
	}

	#define PRINT_GRAPH(CONN) printGraph(CONN);
#else
	#define PRINTF(MY_FORMAT,MY_STRING) ;
	#define PRINT_GRAPH(CONN) ;
#endif

int main (int argc, char const *argv[]) {
	PGconn *conn;
	PGresult *res,*res_parent;				// результат выполнения запроса
	// long 
	long res_count;				// количество записей в полученном ответе
	// long 
	long i;						// счетчик цикла

	char currentNode[20], 				// текущий обрабатываемый узел
		nextNode[20],					// очередной потомок текущего узла
		startNode[20],goalNode[20];
	char query[200];
	// long
	long
		nextQueueNumber = 0,		// номер в очереди следующего узла
		newQueueNumber = 0;			// номер нового добавляемого в очередь узла
	
	conn = PQconnectdb("dbname=alexander host=localhost user=alexander password=qwerty12");

	if (PQstatus(conn) == CONNECTION_BAD) {
		puts(PQerrorMessage(conn));
		puts("We were unable to connect to the database");
		exit(0);
	}

	if (argc != 2) {
		if (argc != 3) {
			puts("Please specify startNode!");
			exit(1);
		}
	}

	BFS_TREE(startNode,argv[1])			// начинаем обход с вершины startNode
	
	SET_ALL_NO_PARENT(query)
	// PRINTF("Query: %s\n",query)
	
	QUEUE_PUSH_START_NODE(query,startNode,newQueueNumber,startNode)
	// PRINTF("Query: %s\n",query)
	PRINT_GRAPH(conn)

	do {
		HOW_MANY_IN_QUEUE(query,res,res_count,nextQueueNumber)
		// PRINTF("Query: %s\n",query)
		
		if (QUEUE_IS_EMPTY(res_count)) {
			// очередь пуста
			break;
		}
		// очередь не пуста
		QUEUE_TOP(res,currentNode) // значение a_id
		PRINTF("Queue Number to proceed=%ld. ", nextQueueNumber)
		PRINTF("Node Number=%s\n", currentNode)
		
		for (i=0; i < res_count; ++i) {
			strcpy(nextNode,PQgetvalue(res, i, 1)); // значение очередного b_id
			
			PARGRES_SELECT_PARENT(query,res_parent,nextNode)
			PRINTF("Query: %s\n",query)
			PRINTF("---Nextnode Number=%s.\n", nextNode);
			// puts(PQgetvalue(res_parent, 0, 0));
			if (NODE_HAS_NO_PARENT(res_parent)) {	// нет родителя
				++newQueueNumber;		// push
				PARGRES_UPDATE_PARENT(query,newQueueNumber,currentNode,nextNode)
				PRINTF("---Query: %s\n",query)
				PRINTF("---Queued with number=%ld.\n", newQueueNumber)
				PRINT_GRAPH(conn)
			}
			else {
				PRINTF("---Already in queue.\n",NULL)
			}
		}
		++nextQueueNumber;
	} while (true);
	PQclear(res);
	PQfinish(conn);
	return 0;
}