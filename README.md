# Shared_Memory_External_Sort

### CS553 Cloud Computing Practical Assignment 3
**Illinois Institute of Technology**  

**Students**:
* Heyt Gala hgala@hawk.iit.edu
* Vijay Bharvad vbharvad@hawk.iit.edu


## How to check and test your implementation

You can run all the checks and tests for your implementation by calling:
```
$ gcc -o mysort mysort.c
$ make
$ gensort -a 1000000 mysort1GB.txt -- this will generate dataset of 1 GB with input file name as 'mysort1GB.txt'
$ gensort -a 4000000 mysort4GB.txt -- this will generate dataset of 4 GB with input file name as 'mysort4GB.txt'
$ gensort -a 160000000 mysort16GB.txt -- this will generate dataset of 16 GB with input file name as 'mysort16GB.txt'
$ gensort -a 640000000 mysort64GB.txt -- this will generate dataset of 64 GB with input file name as 'mysort64GB.txt'

$ ./mysort mysort1GB.txt outputmysort1GB.txt 8 -- in-memory sorting command for 1GB input file 
$ valsort outputmysort1GB.txt >> mysort1GB.log 2>&1 -- validate output file and append output of it in log file
$ time sort mysort1GB.txt -o linoutputsort1GB.txt -- Linux sort command to sort data and displays execution time
$ valsort linoutputsort1GB.txt >> linsort1GB.log 2>&1 -- Linux sort validation command to validate linux output file and append output of it in linux log file

$ ./mysort mysort4GB.txt outputmysort4GB.txt 8 -- in-memory sorting command for 4GB input file 
$ valsort outputmysort4GB.txt >> mysort4GB.log 2>&1 -- validate output file and append output of it in log file
$ time sort mysort4GB.txt -o linoutputsort4GB.txt -- Linux sort command to sort data and displays execution time
$ valsort linoutputsort4GB.txt >> linsort4GB.log 2>&1 -- Linux sort validation command to validate linux output file and append output of it in linux log file

```
