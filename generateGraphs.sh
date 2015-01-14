#!/bin/bash

counterOut=$1
while [ $counterOut -gt 0 ]
do
	counterIn=$2
	while [ $counterIn -ge 0 ]
	do
	./graphgen "-v" "$counterOut" "-e" "$counterIn"
	   counterIn=$(( $counterIn - 1 ))
	done
	counterOut=$(( $counterOut - 1 ))
done
