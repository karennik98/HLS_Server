#!/bin/bash

counter=1
while [ $counter -le 2 ]
do
  curl -u hello:zarzand 127.0.0.1:8000/record
done
