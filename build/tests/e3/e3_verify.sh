#!/bin/bash

if [ $# -ne 4 ]; then
        echo "Usage: $0 BINARY INPUT_FILE EXPECTED_OUTPUT_FILE [DIRECTORY_FOR_RESULTS]"
        exit 1
fi

# check if input files exist
if [[ ! -f $1 || ! -f $2 || ! -f $3 ]]; then
        files=("$1" "$2" "$3")
        for name in "${files[@]}"; do
                if [[ ! -f $name ]]; then
                        echo "File not found: $name"
                fi
        done
        exit 1
fi
BIN=$1
INPUTFILE=$2
VALIDOUTPUTFILE=$3

# run program and capture output
OUTPUT=`./$BIN < $INPUTFILE 2>&1 1>/dev/null`
if [[ 0 != $? ]]; then
        exit 1
fi

# save .dot file and convert it to an image
NAME=$4/$(basename $INPUTFILE | cut -d. -f1)
OUTPUT=e3.dot
dot ${OUTPUT} -Tpng -o "${NAME}.png"
NAME=$(dirname $VALIDOUTPUTFILE)/$(basename $VALIDOUTPUTFILE | cut -d. -f1)
if [[ `stat -c %Y "$VALIDOUTPUTFILE"` -ge `stat -c %Y "${NAME}.png"` ]]; then
   dot $VALIDOUTPUTFILE -Tpng -o "${NAME}.png"
fi

REFGRAPH=`dot -Tplain $VALIDOUTPUTFILE`
GRAPH=`dot -Tplain $OUTPUT`

#count number of nodes
REF_N_NODES=`grep ^node <<< $REFGRAPH | wc -l`
G_N_NODES=`grep ^node <<<$GRAPH | wc -l`
DIFF_NODES=`echo $G_N_NODES - $REF_N_NODES | bc`

#count number of edges
REF_N_EDGES=`grep ^edge <<< $REFGRAPH | wc -l`
G_N_EDGES=`grep ^edge <<< $GRAPH | wc -l`
DIFF_EDGES=`echo $G_N_EDGES - $REF_N_EDGES | bc`

REFLABELS=$(grep ^node <<< $REFGRAPH | awk '{print $7}' | sed  -e "s/\"//g" | sort)
GLABELS=$(grep ^node <<< $GRAPH | awk '{print $7}' | sed  -e "s/\"//g" | sort)
diff -s <(echo "$REFLABELS") <(echo "$GLABELS") >/dev/null
DIFF=$?

#GDIFF=$(./$(dirname $0)/checkdots.py $VALIDOUTPUTFILE $OUTPUT ; echo $?)
GDIFFGVTREE=$(./$(dirname $0)/checkdots.py $VALIDOUTPUTFILE e3.dot ; echo $?)

if [ $GDIFFGVTREE -eq 0 ]; then
        exit 0
else
        exit 1
fi

#if [ $DIFF_NODES -eq 0 -a $DIFF_EDGES -eq 0 -a $DIFF -eq 0 -a $GDIFF -eq 0 ]; then
#       exit 0
#else
#       exit 1
#fi
