Team Member Names: Austin Eubank, Frank Conlon, Zohreh Safari

Installation: Use the make file to compile the program.

Running: To run type ./source .

Command Line Options:
--dim: number of dimensions, must be integer greater than 0 (default dim = 128)
--ndata: number of data points, must be integer greater than 0 and divisible by
         4 (default ndata = 10000)
--max: maximum size of double in all data points, must be floating point number
       greater than 0 (default max = 1000.00)
--k: number of clusters, must be integer greater than 0 (default k = 128)
--seed: seed multiplier must be integer greater than 0 (default seed = 1)

Note: The seed multiplier is applied to the array { 2, 4, 8, 16 } when data
points are being generated and to the query seed (666) when the query is being
generated. This is to allow more flexibility during testing of the program.

Note: Command line arguments cannot be skipped.

Frank Ibem's clusters.py can be used. The program will create the file
data.txt iff dim = 2. If this file has been created, then run clusters.py
(included) in the program directory to view a png of the kmeans and its
cluster assignments.

Output: The program will output the parameters used to construct the kmeans
structure and the query point. It will also output the result(s) of the
kmeans search algorithm and the results of the brute force search algorithm.
The brute force algorithm will select only one point. If this point is one
of the search results, then the program will print, "THE RESULT IS CORRECT".
If the point is not one of the search results, then the program will print,
"THE RESULT IS NOT CORRECT". The program will also output the number of points
checked by the kmeans search algorithm.
