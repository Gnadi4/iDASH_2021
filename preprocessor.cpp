#include <cmath>
#include <complex>
#include <cstdlib>

#include <unistd.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <istream>
#include <sstream>
#include <ext/hash_map>
using namespace std;
class Preprocessor{
public:
	static void preprocess();
	static int getIdx(string piece);
	
private:
	static const int n_domain = 4;
	static const int n_step = 4;
	static const int n_case = 256;
};

int main(){
	int pid = getpid();
        char target[30], buf[4096];

	clock_t start, end;
	start = clock();

	cout << "========================" << endl;
	cout << "preprocessing start.." << endl;
	

	Preprocessor::preprocess();
	end = clock();
	cout << "done" << endl;
	cout << "========================" << endl;

	cout << "preprocessing takes " << (double)(end-start) / CLOCKS_PER_SEC << " sec"<< endl;
	FILE* res = fopen("./data.csv","r");
	fseek(res,0,SEEK_END);
	float result_size = ftell(res);
	printf("result saved to ./data.csv and size is %fMB\n", (result_size/(1<<20)));
	
	//memory usage check
	sprintf(target, "/proc/%d/status", pid);
	FILE* f = fopen(target, "r");
	fread(buf, 1, 4095, f);
	buf[4095] = '\0';
	fclose(f);
	float mem;
	char* ptr = strstr(buf, "VmPeak:");
	sscanf(ptr, "%*s %f", &mem);
	printf("Max memory usage is %fMB\n", mem / (1 << 10));
}


void Preprocessor::preprocess(){
	

	ifstream file("./data.fa");
	if (file.fail()){
		cout << "data file not exist" << endl;
		return;
	}
	ofstream outfile("./data.csv");


	string line;
	
	while(getline(file,line)){
		if(line.at(0) != '>') continue;
		getline(file,line);
		int count[n_case] = {0};
		
		int prev[3] = {-2,-2,-2};
			for(int i=0; i < line.length() - n_step + 1; i++){
				string now = line.substr(i,n_step);
				int idx = getIdx(now);
				if(idx != -1 && idx != prev[0] && idx != prev[1] && idx != prev[2]){
					count[idx]+=1;
				}else{
					idx = -1;
				}
				prev[0] = prev[1];
				prev[1] = prev[2];
				prev[2] = idx;

			}
	
		//write to csv file by 1 line	
		outfile << count[0];
		for(int i=1; i <n_case; i++){
			outfile << ',' << count[i] ;
		}
		outfile << "\n";
	}	
	
	file.close();
	outfile.close();	
	
}

//calculate index of gene pattern from 'aaaa' to 'tttt'
int Preprocessor::getIdx(string piece){
	if (piece.length() != 4){
		cout << "piece length is not 4" << endl;
	}
	int res = 0;
	for(int i=0;i<piece.length();i++){
		int var = -1;
		switch(piece.at(piece.length() - i - 1)){
			case 'A': case 'a':
				var = 0;
				break;
			case 'C': case 'c':
				var = 1;
				break;
			case 'G': case 'g':
				var = 2;
				break;
			case 'T': case 't':
				var = 3;
				break;

			}
		if (var == -1){
			return -1;
		}else{
			res = res + var * pow(n_domain,i);
		}
	}
	return res;
}


