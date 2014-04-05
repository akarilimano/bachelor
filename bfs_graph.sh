#!/bin/bash

function usage {
	echo Usage: "$0" [-s START_NODE] [-g GOAL_NODE] | [-h | --help]
}
function PSQL() {
	psql -t -A -c "$1"
}
#DEBUG
function PRINT_GRAPH {
	psql -q -c "SELECT * FROM Graph order by a_id;"
} 

function print_path {
	echo "Path:"
	# NODE=`PSQL 'SELECT DISTINCT parent FROM Graph WHERE a_id='"$GOAL_NODE"`
	# echo -n "$NODE"
	for (( ; "$NODE" != "$START_NODE" ; ))
	do
		NODE=`PSQL 'SELECT DISTINCT parent FROM Graph WHERE a_id='"$NODE"`
		echo "$NODE"
	done | tac | tr '\n' '-'
	echo "$GOAL_NODE"
}

START_NODE=0
GOAL_NODE=0

while [ "$1" != "" ] 
do
	case $1 in 
		-h | --help ) 
			usage
			exit
		;;
		-s )
			shift
			START_NODE="$1"
		;;
		-g )
			shift
			GOAL_NODE="$1"
		;;
		* )
			usage
			exit 1
		;;
	esac
	shift
done

psql -c "UPDATE Graph SET parent=NULL, queue=NULL;"
psql -c "UPDATE Graph SET parent=-1, queue=0 WHERE a_id="$START_NODE";"

echo "Initial State"
PRINT_GRAPH "SELECT * FROM Graph;"

NEXT=0
NODE=$START_NODE
NEWQUEUE=0

while [[ `PSQL 'select queue from graph WHERE queue='"$NEXT"';'` != "" ]]
do
	NODE=`PSQL 'SELECT DISTINCT a_id from Graph WHERE queue='"$NEXT"';'`
	echo "Next number in queue="$NEXT
	echo "-NODE="$NODE
	if [[ "$NODE" == "$GOAL_NODE" ]]
	then
		echo "FINDED"
		PRINT_GRAPH
		print_path # > pass
		exit
	fi
	NEXTNODES=(`PSQL 'SELECT b_id FROM Graph WHERE queue='"$NEXT"';'| tr "\n" " "`)
	echo "--NEXTNODES="${NEXTNODES[@]}
	for NEXTNODE in ${NEXTNODES[@]}
	do
		if [[ `PSQL 'SELECT DISTINCT parent FROM Graph WHERE a_id='"$NEXTNODE"';'` != "" ]]
		then
			echo "---NEXTNODE "$NEXTNODE" had been already queued"
			continue
		fi
		NEWQUEUE=$(($NEWQUEUE + 1))
		echo "---NEXTNODE "$NEXTNODE" is queued "$NEWQUEUE
		PSQL 'UPDATE Graph SET queue='"$NEWQUEUE"',parent='"$NODE"' WHERE queue is NULL and a_id='"$NEXTNODE"';'
		# в будущем можно сделать по возвращаемому значению, если не обновилось, не увеличивать NEWQUEUE
		PRINT_GRAPH "SELECT * FROM Graph order by a_id;"
	done
	NEXT=$(($NEXT + 1))
done

echo "NOT FINDED"
PRINT_GRAPH "SELECT * FROM Graph order by a_id;"

#echo "Table Graph dropped"
