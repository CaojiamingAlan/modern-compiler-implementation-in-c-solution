#!/bin/bash
for i in {1..49}
do
	echo parsing test${i}
	./lextest ../testcases/test${i}.tig
	echo 
done
