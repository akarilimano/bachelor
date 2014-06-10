#ifndef __GRAPH_BFS_H__
#define __GRAPH_BFS_H__ 1

#define TABLE_NAME "graph"
#define A_ID_NAME "a_id"
#define B_ID_NAME "b_id"
#define PARENT_NAME "parent"
#define QUEUE_NAME "queue"

#define NO_PARENT "-1"
#define NULL_QUEUE "NULL"

// #define UPD_Q1 "UPDATE %s SET %s=%s, %s=%s"
// #define UPD_Q2 "UPDATE %s SET %s=%s, %s=%s where %s=%s"
// #define SEL_NEXT "select distinct %s,%s from %s where %s=%ld"
// #define Q "select %s from %s where %s=%s"

#define BFS_TREE(ROOT,VAL1) strcpy(ROOT,VAL1);

// update graph set parent=-1, queue=NULL
#define SET_ALL_NO_PARENT(QUERY) { \
	sprintf(QUERY,"update %s set %s=%s, %s=%s",TABLE_NAME,PARENT_NAME,NO_PARENT,QUEUE_NAME,NULL_QUEUE); \
	PQexec(conn,QUERY); \
}

// update graph set parent=startNode,queue=0 where a_id=startNode
#define QUEUE_PUSH_START_NODE(QUERY,VAL1,VAL2,VAL3) { \
	sprintf(QUERY,"update %s set %s=%s, %s=%ld where %s=%s",TABLE_NAME,PARENT_NAME,VAL1,QUEUE_NAME,VAL2,A_ID_NAME,VAL3); \
	PQexec(conn,QUERY); \
}

// select distinct a_id,b_id from graph where queue=nextQueueNumber
#define HOW_MANY_IN_QUEUE(QUERY,RESULT,RESULT_COUNT,VAL1) { \
	sprintf(QUERY,"select distinct %s,%s from %s where %s=%ld",A_ID_NAME,B_ID_NAME,TABLE_NAME,QUEUE_NAME,VAL1); \
	RESULT = PQexec(conn,QUERY); \
	RESULT_COUNT = PQntuples(res); \
}

#define QUEUE_IS_EMPTY(RESULT_COUNT) RESULT_COUNT==0

#define QUEUE_TOP(RESULT,NODE) { \
	strcpy(NODE,PQgetvalue(RESULT, 0, 0)); \
}

// select distinct parent from graph where a_id=nextNode
#define PARGRES_SELECT_PARENT(QUERY,RESULT,VAL1) { \
	sprintf(QUERY,"select distinct %s from %s where %s=%s",PARENT_NAME,TABLE_NAME,A_ID_NAME,VAL1); \
	RESULT = PQexec(conn,QUERY); \
}

#define NODE_HAS_NO_PARENT(RESULT) strcmp(PQgetvalue(RESULT, 0, 0),NO_PARENT)==0

// update graph set queue=newQueueNumber,parent=currentNode where parent=-1 and a_id=nextNode
#define PARGRES_UPDATE_PARENT(QUERY,VAL1,VAL2,VAL3) { \
	sprintf(QUERY,"update %s set %s=%ld, %s=%s where %s=%s and %s=%s",TABLE_NAME, QUEUE_NAME,VAL1,PARENT_NAME,VAL2,PARENT_NAME,NO_PARENT,A_ID_NAME,VAL3); \
	PQexec(conn,QUERY); \
}

#endif