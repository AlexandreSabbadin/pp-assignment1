# Parallel Programming - Assignment 1

How to run the program:

```bash
module load openmpi
mpicc -O3 message.c -o message
srun -n 2 message
```

To test the program I ran it with `-n` on *dione* cluster and with `-N` on *titan*. For each message size I did 5 tries then took the smallest result. All the results are compiled into the following table. Message sizes are in bytes and times in seconds.

| Message size | -n 2     | -n 4     | -n 8     | -n 16    | -N 2     | -N 4     | -N 8     | -N 16    |
|--------------|----------|----------|----------|----------|----------|----------|----------|----------|
| 100000       | 0,000042 | 0,000145 | 0,000277 | 0,000572 | 0,000055 | 0,000102 | 0,000191 | 0,000379 |
| 1000000      | 0,00025  | 0,00114  | 0,00229  | 0,004672 | 0,000341 | 0,000666 | 0,001331 | 0,002678 |
| 10000000     | 0,003395 | 0,01115  | 0,022335 | 0,046413 | 0,003784 | 0,007105 | 0,013697 | 0,026894 |
| 1000000000   | 0,616538 | 1,560054 | 3,243933 | 6,622622 | 0,658033 | 1,605449 | 3,481459 | 7,129041 |

Console logs for `srun -n 4 --mem=2G message`:

![output](https://user-images.githubusercontent.com/85174595/192505212-dd118cc9-afe1-4843-b480-0acdc031c41b.PNG)
