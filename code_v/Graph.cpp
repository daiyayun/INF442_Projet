/*
 * Graph.cpp
 *
 *  Created on: May 1, 2017
 *		Author: yayundai & zejianli
*/

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>


#include "Graph.hpp"

//construct a graph from a data file
Graph::Graph(string path){
	using namespace std;
	string line;
	ifstream myfile(path.c_str());
	if(myfile.is_open()){
		while(getline(myfile,line)){
			//write data into the class
			istringstream iss(line);
			string s;
			vector<int> tuple;
			while(getline(iss, s, ' ')){
				//tuple.push_back(stoi(s));
				tuple.push_back(atoi(s.c_str()));
			}
			relation.push_back(tuple);
		}
		myfile.close();
	}
	else std::cerr<<"Unable to open file"<<endl;
}

//order a relation with a given permutation
void Graph::order(const vector<int>& perm){
	using namespace std;	
	sort(this->relation.begin(),this->relation.end(),Compare(perm));
}

//save the relation to a given path
void Graph::saveTo(string path){
	using namespace std;
	int l=getArity();
	ofstream myfile(path.c_str());
	if(myfile.is_open()){
		vector<vector<int> >::iterator it;
		for(it = relation.begin(); it != relation.end(); it++){
			int i=0;
			for(vector<int>::iterator jt=it->begin();jt!=it->end();jt++){
				myfile<<(*jt);
				if(i<l-1)
					myfile<<" ";
				i++;
			}
			myfile << endl;
		}
		myfile.close();
	}
	else cerr<<"Unable to open file";
}

// void Graph::saveRelation(vector<vector<int> >& r, string& path){
// 	using namespace std;
// 	int l=r[0].size();
// 	ofstream myfile(path.c_str());
// 	if(myfile.is_open()){
// 		vector<vector<int> >::iterator it;
// 		for(it = r.begin(); it != r.end(); it++){
// 			int i=0;
// 			for(vector<int>::iterator jt=it->begin();jt!=it->end();jt++){
// 				myfile<<(*jt);
// 				if(i<l-1)
// 					myfile<<" ";
// 				i++;
// 			}
// 			myfile << endl;
// 		}
// 		myfile.close();
// 	}
// 	else cerr<<"Unable to open file";
// }


//join two relations
Graph Graph::join(Graph* r1, vector<string> v1, Graph* r2, vector<string> v2){
	using namespace std;
	vector<vector<int> > x = findCommon(v1, v2);
	int l1 = v1.size();//number of variables in graph r1
	int l2 = v2.size();//number of variables in graph r2
	vector<int> perm1(l1);
	vector<int> perm2(l2);
	for(int i = 0; i < l1; i++) perm1[i] = 0;
	for(int i = 0; i < l2; i++) perm2[i] = 0;

	//create a permutation array for each relation, and order the two relations
	int n = x[0].size();//number of common variables
	for(int i = 0; i < n; i++){
		perm1[x[0][i]] = (i+1);
		perm2[x[1][i]] = (i+1);
	}
	int count = n;
	for(int i = 0; i < l1; i++){
		if(perm1[i] == 0){
			count++;
			perm1[i] = count;		
		} 
	}
	count = n;
	for(int i = 0; i < l2; i++){
		if(perm2[i] == 0){
			count++;
			perm2[i] = count;
		}
	}
	r1->order(perm1);
	r2->order(perm2);

	// join the two relations:
	// iterate over the two relations in parallel starting at the first tuple in each
	// and call the currently considered tuples t and t'. 
	// If t and t' coincide on X, add all combinations of tuples that agree with t and t' on X,
	// add the result to the output and jump in R and R' to the first tuples that disagree with t and t 0 on X;
	// If π(t) and π(t') are different two cases can occur: 
	//   If π(t) is lexicographically smaller than π(t'), go to the next tuple t;
	//   Otherwise, go to the next tuple t'.

	vector<vector<int> >::iterator it1 = r1->relation.begin();
	vector<vector<int> >::iterator it2 = r2->relation.begin();
	Graph g;//the joined graph
	while((it1!=r1->relation.end())&&(it2!=r2->relation.end())){
		bool flag = customCompare((*it1),(*it2),x);
		if(flag){           //If t and t' coincide on X
			int pos = 1;
			while(((it2+pos)!=r2->relation.end())&&customCompare(*it1,*(it2+pos),x)){
				pos++;
			}

			while((it1!=r1->relation.end())&&(it2!=r2->relation.end())&&customCompare(*it1,*it2,x)){
				for(int i = 0; i < pos; i++){
					vector<int> v;
					for(int j = 0; j < l1; j++){
						v.push_back((*it1)[j]);
					}

					for(int j = 0; j < l2; j++){
						// if(!contains(x[1],j)){
						if(find(x[1].begin(),x[1].end(),j)==x[1].end()){
							v.push_back((*(it2+i))[j]);
						}
						
					}
					g.relation.push_back(v);
				}
				it1++;

			}
			it2+=pos;
		} else{           //If π(t) and π(t') are different
			vector<int> v1tmp, v2tmp, perm;
			for(int i=1;i<=n;i++)
				perm.push_back(i);
			for(int i=0;i<n;i++){
				v1tmp.push_back((*it1)[x[0][i]]);
				v2tmp.push_back((*it2)[x[1][i]]);
			}
			if(Compare(perm)(v1tmp,v2tmp)){
				it1++;
			} else {
				it2++;
			}


		}
	}
	return g;
};



Graph::~Graph(){}

//find the common variables in two varaible lists

vector<vector<int> > findCommon(vector<string>& v1, vector<string>& v2){
	int n1=v1.size();
	int n2=v2.size();
	string s1,s2;
	vector<vector<int> > result;
	vector<int> pos1,pos2;
	for(int i=0;i<n1;i++){
		s1=v1[i];
		for(int j=0;j<n2;j++){
			s2=v2[j];
			if(s1.compare(s2)==0){
				pos1.push_back(i);
				pos2.push_back(j);
			}
		}
	}
	result.push_back(pos1);
	result.push_back(pos2);
	return result;
};

//find the number of different elements in total
int unionSize(vector<string> v1, vector<string> v2){
	return joinedVar(v1,v2).size();
}

//get the combined list of variables
vector<string> joinedVar(vector<string>& v1, vector<string>& v2){
	vector<string> result;
	int n1=v1.size();
	int n2=v2.size();
	string s1,s2;
	bool contains;
	for(int i=0;i<n1;i++){
		result.push_back(v1[i]);
	}
	for(int i=0;i<n2;i++){
		s2=v2[i];
		contains=false;
		for(int j=0;j<n1;j++){
			s1=v1[j];
			if(s1.compare(s2)==0){
				contains=true;
				break;
			}
		}
		if(!contains){
			result.push_back(s2);
		}
	}
	return result;
}

//compare the restrictions of two tuples onto X, the set of common variables 

bool customCompare(vector<int> v1, vector<int> v2, vector<vector<int> > commonPos){
	int n=commonPos[0].size();
	for(int i = 0; i < n; i++){
		if(v1[commonPos[0][i]] != v2[commonPos[1][i]])
			return false;
	}
	return true;

};


vector<int> unfold(vector<vector<int> >& mat){
	int n=mat.size();
	if(n==0)
		return vector<int>(0);
	int m=mat[0].size();
	vector<int> res(n*m);
	for(int i=0;i<n;i++){
		for(int j=0;j<m;j++){
			res[i*m+j]=mat[i][j];
		}
	}
	return res;
}

vector<vector<int> > fold(vector<int>& unfolded, const int& blockSize){
	int size=unfolded.size()/blockSize;
	vector<vector<int> > res;
	for(int i=0;i<size;i++){
		vector<int> line;
		for(int j=0;j<blockSize;j++){
			line.push_back(unfolded[i*blockSize+j]);
		}
		res.push_back(line);
	}
	return res;
}

uint32_t myhash(uint32_t a)
{
   a = (a+0x7ed55d16) + (a<<12);
   a = (a^0xc761c23c) ^ (a>>19);
   a = (a+0x165667b1) + (a<<5);
   a = (a+0xd3a2646c) ^ (a<<9);
   a = (a+0xfd7046c5) + (a<<3);
   a = (a^0xb55a4f09) ^ (a>>16);
   return a;
}


