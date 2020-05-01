#!/bin/bash

counter=1
while [ $counter -le 2 ]
do
  curl -u login:password 127.0.0.1:8000/frames
done
