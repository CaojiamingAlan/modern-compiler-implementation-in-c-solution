#!/bin/bash
for i in {1..50}
do
	echo ${i}
	./parsetest ../testcases/test${i}.tig
done
