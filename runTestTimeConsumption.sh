#!/bin/bash

pattern=graph
for i in *.txt; do
  case "$i" in 
	$pattern*)
		./gis.0.1.1 < "$i"
        continue;;
	*)
  esac
done
