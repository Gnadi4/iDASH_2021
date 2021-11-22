/*
* Copyright (c) by CryptoLab inc.
* This program is licensed under a
* Creative Commons Attribution-NonCommercial 3.0 Unported License.
* You should have received a copy of the license along with this
* work.  If not, see <http://creativecommons.org/licenses/by-nc/3.0/>.
*/
#include "TestScheme.h"

#include <NTL/BasicThreadPool.h>
#include <NTL/ZZ.h>

#include "Ciphertext.h"
#include "EvaluatorUtils.h"
#include "Ring.h"
#include "Scheme.h"
#include "SchemeAlgo.h"
#include "SecretKey.h"
#include "StringUtils.h"
#include "TimeUtils.h"
#include "SerializationUtils.h"

/*check memory usage */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

using namespace std;
using namespace NTL;

void TestScheme::testClassify(long logq, long logp, long logn){
	cout << "!!! START TEST CLASSIFY !!!" << endl;
	cout <<""<<endl;
	int pid = getpid();
	char target[30], buf[4096];

	srand(time(NULL));
	SetNumThreads(8);
	TimeUtils timeutils;
	Ring ring;
	SecretKey secretKey(ring);
	Scheme scheme(secretKey, ring);
	long m = 256;
        long n_sample = 2000;
	long n = (1 << logn);//n=2048
	long arr_num = n/m;
	long n_bundle = 0;
	if(n_sample%(arr_num/4)==0){
		n_bundle = n_sample/(arr_num/4);
	}else{
		n_bundle = (n_sample/(arr_num/4))+1;
	}

	complex<double>** mvec_samples = EvaluatorUtils::read_csv_sample(n, m, n_sample, n_bundle,0.001);
	complex<double>* mvec_weight = EvaluatorUtils::read_csv_weight(n, m);
	complex<double>* mvec_intercept = EvaluatorUtils::read_csv_intercept(n,m);


	//ciphertext memory check
	sprintf(target, "/proc/%d/status", pid);
	FILE* f = fopen(target, "r");
	fread(buf, 1, 4095, f);
	buf[4095] = '\0';
	fclose(f);

	int before_mem;
	char* ptr = strstr(buf, "VmSize:");
	sscanf(ptr, "%*s %d", &before_mem);


	Ciphertext* ciphers = new Ciphertext[n_bundle];
	Ciphertext ctxt_intercept;

	timeutils.start("encrypt test data");
	NTL_EXEC_RANGE(n_bundle,first,last);
        for(int i=first;i<last;i++){
		scheme.encrypt(ciphers[i], mvec_samples[i], n, logp, logq);
	}
	NTL_EXEC_RANGE_END;
	scheme.encrypt(ctxt_intercept, mvec_intercept, n, logp, logq);
	timeutils.stop("encrypt test data");
	cout <<""<<endl;


	sprintf(target, "/proc/%d/status", pid);
	f = fopen(target, "r");
	fread(buf, 1, 4095, f);
	buf[4095] = '\0';
	fclose(f);	
	int after_enc_mem;	
	ptr = strstr(buf, "VmSize:");
	sscanf(ptr, "%*s %d", &after_enc_mem);
	printf("Ciphertext Use %dMB Memory\n", (after_enc_mem-before_mem) / (1 << 10));
	cout << "" << endl;



	timeutils.start("multiply");
	ZZ* cnstPoly = new ZZ[N];
	ring.encode(cnstPoly,mvec_weight,1<<11,logp);
	NTL_EXEC_RANGE(n_bundle,first,last);
	for(int i=first;i<last;i++){
		scheme.multByPolyAndEqual(ciphers[i], cnstPoly,logp);
	}
	NTL_EXEC_RANGE_END;
	delete[] cnstPoly;
	timeutils.stop("multiply");
	cout <<""<<endl;
		

	//timeutils.start("addRotKey");
	for(int j=0;j<8;j++){
		scheme.addLeftRotKey(secretKey, (1<<j));
	}
	//timeutils.stop("addRotKey");
	//cout <<""<<endl;

	timeutils.start("Rotate and Sum");
	NTL_EXEC_RANGE(n_bundle,first,last);
	for(int i=first;i<last;i++){
		Ciphertext ctxt_tmp;
		for(int j=0;j<8;j++){
			scheme.leftRotateFast(ctxt_tmp, ciphers[i], (1<<j));
			scheme.addAndEqual(ciphers[i], ctxt_tmp);
		}
		scheme.addAndEqual(ciphers[i], ctxt_intercept);
	}
	NTL_EXEC_RANGE_END;
	timeutils.stop("Rotate and Sum");
	cout <<""<<endl;

	complex<double>** mvec_result = new complex<double>* [n_sample];
	timeutils.start("decrypt");
	complex<double>** mvec_result_tmp = new complex<double>* [n_bundle];
	NTL_EXEC_RANGE(n_bundle,first,last);
        for(int i=first;i<last;i++){
		mvec_result_tmp[i] = scheme.decrypt(secretKey, ciphers[i]);
	}
	NTL_EXEC_RANGE_END;

	NTL_EXEC_RANGE(n_sample,first,last);
        for(int i=first;i<last;i++){
		mvec_result[i] = new complex<double>[4];
		mvec_result[i][0] = mvec_result_tmp[(int)i/2][4*m*(i%2)];
		mvec_result[i][1] = mvec_result_tmp[(int)i/2][4*m*(i%2)+m];
		mvec_result[i][2] = mvec_result_tmp[(int)i/2][4*m*(i%2)+m*2];
		mvec_result[i][3] = mvec_result_tmp[(int)i/2][4*m*(i%2)+m*3];
	}
	NTL_EXEC_RANGE_END;

	timeutils.stop("decrypt");
	cout <<""<<endl;

	EvaluatorUtils::to_csv(mvec_result, n_sample, 4);
	
	cout <<""<<endl;
	cout << "Final prediction reuslt is saved in run/result.csv" << endl;
	

	sprintf(target, "/proc/%d/status", pid);
	f = fopen(target, "r");
	fread(buf, 1, 4095, f);
	buf[4095] = '\0';
	fclose(f);

	ptr = strstr(buf, "VmPeak:");
	int mem;
	sscanf(ptr, "%*s %d", &mem);
	printf("Max Memory Usage is %dMB\n", mem / (1 << 10));
}
