#!/bin/bash

pattern=graph
for i in *.txt; do
  case "$i" in 
	$pattern*)
		echo $i
		./gis.0.1.1 < $i
		sleep 0.1
        continue;;
	*)
  esac
done


