#To compile use following commands:

1. For Lossless join test ->
----------------------------
g++ -o lj lj.cpp
----------------------------

2. For 3NF LJ DP synthesis ->
----------------------------
g++ -o 3nf 3nfsyn.cpp
----------------------------

3. For BCNF LJ synthesis ->
----------------------------
g++ -o bcnf bcnfsyn.cpp
----------------------------

#Format of test case and testing
a. A test case is to be written in a file (say file.txt).
b. First line contains comma separated list of attributes for a relation
c. Following any number of lines may contain comma separated list for decompositions.
d. Following any number of lines contain list of functional dependencies.

(Please see sample files and test cases in the 'testcases' folder)
(Expected answers are available in testcases/answers.txt)

#To run code
1. LJ test:
----------------------------
./lj file.txt
----------------------------

2. 3NF LJ DP synthesis
----------------------------
./3nf file.txt
----------------------------

3. BCNF LJ synthesis
----------------------------
./bcnf file.txt
----------------------------

#For using written test cases:
./lj testcases/ljt1.txt
./lj testcases/ljt2.txt
./lj testcases/ljt3.txt
./3nf testcases/3nft1.txt
./3nf testcases/3nft2.txt
./3nf testcases/3nft3.txt
./3nf testcases/3nft4.txt
./bcnf testcases/bcnft1.txt
./bcnf testcases/bcnft2.txt