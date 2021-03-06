/*
 * test_Hyper.cpp
 *
 *  Created on: May 26, 2017
 *		Author: yayundai & zejianli
*/
#include <ctime>
#include <string>
#include <vector>
#include <iostream>
#include "Graph.hpp"
#include "mpi.h"

using namespace std;

int main(int argc, char **argv){
	//creat a new graph from a data file
	string fileName;
	if(argc==2){
		fileName=argv[1]; 
	} else{
		//the names of the default files to be read
		fileName="test";
	}	


	const string path = "../"+fileName+".dat";
	const string pathJoined="../"+fileName+"_HJoined_triangles.dat";
	Graph g(path);

	MPI_Init(&argc, &argv);
	int numtasks, taskid;
	MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	if(taskid==0){
		cerr<<"Testing join..."<<endl;
		cerr<<"Finding triangles in "<< fileName << endl;		
	}

    clock_t t = clock();	
	Graph* triangles = Graph::HyperCubeJoin(&g);
	t=clock()-t;
	if(taskid==0){
	    cout <<endl
             <<"execution time: "
             <<(t*1000)/CLOCKS_PER_SEC
             <<"ms\n\n";
		cerr<<"Join done."<<endl;
		
		triangles->saveTo(pathJoined);
		cerr<<"Written to "+pathJoined<<endl;		
	}
	MPI_Finalize();
	return 0;
}