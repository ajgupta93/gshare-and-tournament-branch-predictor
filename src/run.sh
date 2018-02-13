make
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --$1
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --$1
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --$1
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --$1
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --$1
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --$1
