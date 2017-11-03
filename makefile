all:
	mpicc -Wall CommandLineArgs.c DEBUG.c GetKCentroids.c InitializeKM.c Source.c ClusterizeKM.c QuickSortCode.c Search.c Stack.c StackR.c -o source -lm
