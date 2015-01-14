#!/bin/bash

pattern=graph
for i in *.txt; do
  case "$i" in 
	$pattern*)
		echo $i
		./gis < $i
		sleep 0.01
        continue;;
	*)
  esac
done


