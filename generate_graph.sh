#!/bin/bash

function PSQL() {
	psql -t -A -c "$1"
}
#DEBUG
function PSQL_D() {
	psql -c "$1"
}

PSQL "DROP TABLE IF EXISTS Graph;"
echo "Table Graph dropped"

PSQL "CREATE TABLE Graph (A_ID bigint, B_ID bigint, PARENT bigint, QUEUE bigint,PRIMARY KEY (A_ID,B_ID));" #,FOREIGN KEY (PARENT) references Graph(A_ID));"
echo "Table Graph created"

while read LINE; do
	EDGE=(`echo "$LINE"`)
	PSQL "INSERT INTO Graph(A_ID,B_ID,PARENT,QUEUE) values(${EDGE[0]},${EDGE[1]},-1,-1);"
done
PSQL_D "SELECT * FROM Graph;"
#echo "Table Graph dropped"
