#!/bin/bash

TESTNAME="BasicArray"
echo "Debug info: ${TESTNAME} database entry"
mongo mydb --quiet --eval 'db.dsedata.find({}, {_id:0})'
ans=`mongo mydb --quiet --eval 'db.dsedata.find({}, {_id:0})' | jq -r '.solution[0].value'`
expected="7"

if [ "$ans" == "$expected" ];
then
    echo "${TESTNAME} passed!";
    mongo mydb --quiet --eval 'db.dsedata.deleteMany({})'
    echo "Cleared database"
else
    echo "${TESTNAME} failed!";
    echo "Result was: ${ans}";
    echo "Expected  : ${expected}";
fi;
