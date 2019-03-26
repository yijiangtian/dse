#!/bin/bash

TESTNAME="MultiArraySquare"

echo "Debug info: ${TESTNAME} database entry"
mongo mydb --quiet --eval 'db.dsedata.find({}, {_id:0})'
value=`mongo mydb --quiet --eval 'db.dsedata.find({}, {_id:0})' | jq -r '.solution[0].value'`
name=`mongo mydb --quiet --eval 'db.dsedata.find({}, {_id:0})' | jq -r '.solution[0].name'`

# single anwser
expname="value"
expvalue="119"

if [ "${name}" == "${expname}" ] && [ "${value}" == "${expvalue}" ]; then
  echo "${TESTNAME} passed!";
  mongo mydb --quiet --eval 'db.dsedata.deleteMany({})'
  echo "Cleared database"
else
  echo "${TESTNAME} failed!";
  echo "Result was: ${name} = ${value}";
  echo "Expected  : ${expname} = ${expvalue}";
fi
