#!/bin/bash
for i in {1..49}
do
	echo ${i}
	./parsetest ../testcases/test${i}.tig
done
