# iDASH_2021 track 2 challenge code

![image](https://user-images.githubusercontent.com/29485153/143258076-1d0d5225-72ca-40d5-bd59-9ad684f82e82.png)

iDASH 챌린지 소개 페이지 : [http://www.humangenomeprivacy.org/2021/competition-tasks.html](http://www.humangenomeprivacy.org/2021/competition-tasks.html)

### 챌린지 일정

![image](https://user-images.githubusercontent.com/29485153/143258301-762a100e-7107-49da-bee4-84d02319fb73.png)

### Introduction

- 해당 과제는 주어진 바이러스 게놈(즉, COVID19 게놈)을 4가지 다른 균주 중 하나로 분류하는 안전한 방법을 개발하는 것입니다

### Track Description

- 각 팀에게 각각의 균주 정보와 함께 4개의 다른 균주에 대한 2000개(총 8000개의 게놈)의 전체 게놈이 포함된 챌린지 데이터 세트 제공됩니다
- 챌린지 기간이 끝나면 정확도와 시간 측면에서 균주 당 500 개의 게놈(총 2000개의 게놈)이 포함된 hold-out 데이터 셋을 사용하여 솔루션을 평가하게 됩니다
- **암호화된 게놈 입력 데이터에 대한 분류 작업을 안전하고 효율적으로 수행 할 수 있는 분류기를 개발하는것이 이번 챌린지의 과제라고 할 수 있습니다**
- 저희팀은 데이터를 암호화하기 전에 preprocessor.cpp 코드를 통해 전처리 진행하였습니다
- 또한 아래의 조건이 충족되는 한 암호화 되기 전에 입력 게놈의 전처리에 대한 제약이 없기 때문에 조건을 맞추기 위해 도커를 통해 환경을 제약 하였습니다
    1. 전처리의 메모리 사용량은 evaluation 데이터 셋의 게놈 2000개에 대해 1GB를 초과하지 않아야함
    2. 전처리 시간 사용량은 1CPU 코어에서 2000개의 게놈에 대해 1분을 초과하지 않음 - Intel Xeon CPU에서 OS에 대한 간섭없이 테스트를 수행. 즉, 방법을 평가하는 동안 다른 프로그램이 실행되지 않음

### Challenge Dataset and Files

- 챌린지 데이터 세트에는 공개적으로 사용할 수 있는 완전한 게놈이 포함되어있습니다
- 4개의 다른 바이러스 균주에 대한 게놈 서열을 포함하는 하나의 FASTA 파일이 제공됩니다
    - FASTA 포맷은 염기서열 혹은 단백질서열을 문자열로 표현한 것
- 각 바이러스 게놈의 형태는 아래와 같습니다

![image](https://user-images.githubusercontent.com/29485153/143258322-c1a0bb1f-a495-4919-842d-3c72a51edaf7.png)

### Encryption Requirement

- HE 방식의 보안 수준은 128비트 이상으로 설정 되어야합니다

### Evaluation Environment

- 모든 제출은 Docker 컨테이너를 사용하여 평가됨. 컨테이너는 평가에서 CPU 코어 4개 (Intel Xeon Platinum 8180 CPU @ 2.50GHz), 32GB 메모리 및 500GB 스토리지로 제한됩니다
- 제출 파일을 도커 형태로 제출해서 제한 사항을 맞춰주었습니다

### D**ocker hub link**

아래의 도커 허브 내에 README 파일을 통해 도커 실행 방법을 알 수 있습니다

모델에 대한 도커 : [https://hub.docker.com/repository/docker/gnadi4/model](https://hub.docker.com/repository/docker/gnadi4/model) 

전처리기에 대한 도커 : [https://hub.docker.com/repository/docker/gnadi4/preprocessing](https://hub.docker.com/repository/docker/gnadi4/preprocessing)

---

---

### ***제출 코드의 구현방식 및 동작에 대한 설명은 아래와 같습니다(영문)

# Homomorphic Encryption-based Secure Viral Strain Classification

### Team name : Data Science Dept. in SeoulTech

## Method

  We used the open version of the HEaaN library$^{1}$ the implementation of CKKS$^{2}$ scheme, and logistic regression for multi-label classification in this task. Our solution consists of three steps: preprocessing, training, and inference

![image](https://user-images.githubusercontent.com/29485153/143258374-a724e7bd-1153-4ab0-86e0-0942cb3f9ebc.png)

## **2.1 Notation**

![image](https://user-images.githubusercontent.com/29485153/143258396-b0bd03a1-20a5-4bbf-b3ef-3602951db46a.png)

## 2.2 Preprocessing

  To vectorize a viral genome, we firstly extract all subsequences of a predefined length from full genomes given for training. We consider the subsequences of length 4 to manage the number of features based on the empirical observation: the shorter the subsequence length, the less the number of features. For example, when the subsequence length is set to 1,000, 24,556 unique subsequences are obtained, while if it is set to 4, the number of generated unique subsequences is 980. These 980 subsequences are denoted as the candidate features.

  Given the generated candidate features, a genome sequence can be transformed into a vector with several strategies such as binary encoding, counting, etc. In this work, we employ a counting approach to vectorize given genomes. Specifically, the entire genome sequence is represented as a vector of integers where each element denotes the number of subsequences matched with the corresponding feature.

  With the candidate subsequences, we conduct a feature selection process to further reduce the number of features to be used for building a classification model. For the feature selection, we use the RFE method implemented in the sklearn library. RFE (abbreviation of "Recursive Feature Elimination") is a process of selecting the predefined number of optimal features by removing the most unnecessary features one by one based on the importance of each feature with a linear model (i.e., logistic regression explained in the subsequent section). In this feature selection process, we consider 40, 60, and 120 as the number of features to be selected.

  To find the optimal number of features in terms of predictive performance, randomly selected 70% genomes were used for training, and the remaining 30% genomes were used to measure the performance. We repeated this procedure 100 times for a rigorous comparison. From the experiments, we observed that the selected 120 features through RFE show slightly the better performance.

![image](https://user-images.githubusercontent.com/29485153/143258424-668bc3eb-f0ed-4997-b7ac-0c50b58e774f.png)
The final selected 120 features are listed as follows:

![image](https://user-images.githubusercontent.com/29485153/143258442-9f12dc24-ee81-41bc-99e9-41c8768b8a84.png)

## **2.3 Training Logistic-regression Model**

  As a classification model, we employ logistic regression. It outputs class probabilities given a vectorized input (i.e., count vector). Given an input x, a logistic regression model produces the logit values with k weight vectors associated with each class as:
  
![image](https://user-images.githubusercontent.com/29485153/143258463-5b98a421-aec8-49c7-a4a8-5bd3750cfa6e.png)
  
  These continuous logit values are transformed into class probabilities with a softmax function as follows:

![image](https://user-images.githubusercontent.com/29485153/143258504-ffe1611c-9bf4-4fc7-a1c3-d1c5d7971133.png)

  The softmax transformation requires a lot of computational costs with encrypted data. To avoid this issue, we assume that the logit value represents class preference since each logit value corresponds to the distance between a decision hyperplane and a data point. We empirically confirmed that using the logit values does not degrade predictive performance.

  We evaluated our selected 120 features and a logistic regression model again with different random seeds. In this experiment, the given data were divided into 70% for training and 30% for validation. The results are summarized as Table 2:

![image](https://user-images.githubusercontent.com/29485153/143258529-b9d02442-3cb9-4bbb-9e80-1eb9a3921436.png)

From these results, we confirmed again that our approach consistently shows almost perfect classification performance over different data splits.

## **2.4 Inference Phase**

This part describes how to obtain the classification result through preprocessing of input test data, encryption of the preprocessed data, logit calculation with the encrypted input and the trained model, and decryption of the result.

### **2.4.1 Preprocessing**

Input data is represented as [Figure 3] and the detailed preprocessing steps are  provided hereafter. The input fasta file contains 2000 genome samples to be classified. We suppose Xi, is the i-th sample as in the right side of [Figure 3]. Each Xi consists of 𝑙 letters out of A, C, G, T, N, etc, where 𝑙 is around 28000. To build up the input vector, we count the number of occurrences of each element in Sseq from Xi , where Sseqcontains all possible 4 letter combinations of A, C, G, and T. The result of counting is placed in a vector of 256 elements, where each position is mapped to a specific combination of the letters. For simplicity, we assign the position of each combination based on its lexicographic order in Sseq. We suppose the resultant vector made with Xi as mi. This process is depicted in [Figure 4]. In the figure, countXi(seq) for any seq ∈Sseq refers to the counting result for seq in Xi. Please be aware that the overlapped sequence is ignored in counting. E. g.,  as in [Figure 5-ⓐ], “ACGT ” appears 2 times inXi without overlap, resulted in countXi(ACGT)=2, whereas “GTAG” appears 2 times but ‘G’ is overlapped, thus countXi(GTAG)=1 as shown in [Figure 5-ⓑ]. Also, we do not count any sequence that contains ‘N’ and the other letters than ‘A’, ‘C’, ‘G’, and ,’T’, such as ‘TAGN’.

The process of obtaining mi from Xi is the main routine for pre-processing. We denote it as fcount(Xi). The detailed process of fcount(Xi) is given in [Figure 7]. The subroutines used in fcount(Xi) are introduced  in [Figure 6].

![image](https://user-images.githubusercontent.com/29485153/143258552-ae8e7c1f-dfe8-4a32-8734-dbe4b9da70d0.png)

![image](https://user-images.githubusercontent.com/29485153/143258574-e2c3f015-3ba7-4f9e-9838-265ee6e2800f.png)

For the efficiency of preprocessing, fcount(Xi) scans Xi only once. For each seq cut from the beginning, the lexicographical order of seq is obtained through fidx(seq). To avoid counting overlapping seq, it stores the three previous seq in p0,p1, and p2, respectively. If seq is an element of Sseq and not overlapped, store it in the array as an index obtained through fidx(seq).

![image](https://user-images.githubusercontent.com/29485153/143258597-c3570d6c-e20c-44cd-8e8c-4381f358ab16.png)

The entire process of preprocessing is to perform fcount(Xii) for all Xis . [Figure 8] represents the results of the entire preprocessing process when a part of the sample data is provided as an input. This matrix is exported in .csv file format without header. [Figure 9] is an example of result csv.sudo

![image](https://user-images.githubusercontent.com/29485153/143258619-8321023d-b786-4c76-9647-953b74bef7d0.png)

![image](https://user-images.githubusercontent.com/29485153/143258653-90efd99d-e390-4f07-a373-e5bda91ecab2.png)

### **2.4.2 Encryption of pre-processed data**

We use the open version of the HEaaN library [1], the implementation of CKKS[2] scheme where parameters are N=212, log q= 101, log p =45 with (uniformly random) signed binary secret. We use this parameter  to meet both security and time constraints of the competition calculation. We use 2048 slots. As a result of the training step in Section 2.3, we have 4 model vectors of w1, w2, w3, and w4. By performing the inner product operation with the input vector respectively, we can obtain the logit value for each class.

The result of section 2.4.1 is mi whose length is 256. Because mi needs to be multiplied by w1~ , respectively, we make a new vector that contains mi by 4 times in a duplicated manner. Then, it is encrypted. Since the size of the new vector is 1024, we can put two mis in a ciphertext. This process is depicted in [Figure 10].  One additional operation conducted before encryption is that every element in the new vector is multiplied by 0.001 to make the range of its value in [0, 1].

Finally, these 1000 ciphertexts will be delivered to the next step to compute the logit value.

![image](https://user-images.githubusercontent.com/29485153/143258688-0e99c585-8e6c-4b2e-a442-e2af4fc9b54b.png)

### **2.4.3 Computing logit value with encrypted inputs**

The 1000 ciphertexts generated in section 2.4.2 are processed in this part. Please remember that there are 120 features that were selected for training. The main task in this step is how the values of the selected features are extracted from each input ciphertext to compute the logit value for each inputmi. Because we know which slot’s value in a ciphertext needs to be multiplied with which coefficient value in the model, we can construct a plaintext polynomial that has the model coefficient values in the right positions.

[Figure 11] shows an example of model vectors w1~ w4. The ‘Feature’ row shows what features’ count values should be multiplied to the model coefficients. Since mi is a vector of 256, we put the coefficient values for each model in the certain positions  of a vector of 256 elements either so that they can be multiplied with the correct count values in mi. This process is shown in [Figure 12].

![image](https://user-images.githubusercontent.com/29485153/143258716-5eca2957-9044-4729-a22b-af41f8803b2f.png)

Because each ciphertext contains 2 mis, we make a model coefficient vector using the model vectors as in [Figure 13]. After converting the coefficient vector into a plaintext polynomial, it is multiplied with each input ciphertext.

![image](https://user-images.githubusercontent.com/29485153/143258748-203431cc-cbc7-4dea-8b07-15b28da5bb9b.png)

![image](https://user-images.githubusercontent.com/29485153/143258769-e10a5602-a989-4b2b-bb96-52f07f75577a.png)

In addition, the number of intercept in the trained logistic regression model is 4, one for each class. If the intercepts for each class are assumed to be {intercept1, intercept2,intercept3, intercept4}, as shown in [Figure 14], since there are two mis in a ciphertext, o we have to make a polynomial including a pair of all intercepts in the following order: {intercept1, intercept2, intercept3, intercept4, intercept1, intercept2, intercept3, intercept4} to process them at once . Also, zero values are put in the slots where the values of the intercepts are not stored. It is added to the result in the next step after left rotations are completed.

![image](https://user-images.githubusercontent.com/29485153/143258807-4e983652-ff9e-4171-942b-4963674c4666.png)

In order to store the logit values in the slot positions of multiples from 0 to 256th index, after multiplication, a set of left rotate operations are performed as in [Figure 15].

![image](https://user-images.githubusercontent.com/29485153/143258834-0c6e8be8-6300-4961-9464-d4eb63e4166f.png)

### **2.4.4 Obtaining the classification result**

Finally, as shown in [Figure 16], the result of logistic regression is obtained as a result of the previous step. The logit value for class i is in the 256*(i-1)th slot or 1024+256*(i-1)th slot in the ciphertext. Thisprocess is repeated for all every ciphertext from the step in Section 2.4.3.

![image](https://user-images.githubusercontent.com/29485153/143258848-f8158478-f455-498b-b645-255c76554549.png)

As described in the Section 2.3, the values can be transformed into class probabilities with a softmax function. However, the softmax transformation requires a lot of computational costs with encrypted data. Moreover, from the result in an unencrypted state, we checked that the values without executing softmax consistently give almost perfect classification performance over different data splits. So without running the softmax function, the ciphertext is delivered to be decrypted, and after decryption, it is stored in the form of .csv as shown in [Figure 17] for convenience in checking the result. The first line of the output csv file is the name of each class( [B.1.427] [B.1.1.7] [P.1] [B.1.526] ) and after line, the order of result.csv file follows the order of the Xi.

![image](https://user-images.githubusercontent.com/29485153/143258861-cdfc33f5-c165-4bae-9dcf-0a2abc5c9b7f.png)

## **3 Experimental Result**

We use the open version of the HEaaN library [1], the implementation of CKKS[2] scheme, and the parameters are N=212, log q= 101, log p =45 with (uniformly random) signed binary secret. log qis smaller than 103 which satisfies more than 128-bit security according to [Table 3].

![image](https://user-images.githubusercontent.com/29485153/143258880-3318565c-7d74-4076-ba57-cfa925d5e45b.png)

Our implementation is executed on Ubuntu 20.04 , Intel Xeon Platinum 8259CL CPU @ 2.50GHz processor with 8 threads. In addition, out of 7461 samples from which 539 duplicates were removed from a given Challenge.fa file,70% of them were used for the training data set(70%), and the other 30% were used for the test data set . The detailed results are as follows.

### Result

The preprocess stage measurement was carried out using only 1 CPU core. The result is shown in [Figure 18].

![image](https://user-images.githubusercontent.com/29485153/143258902-5871c669-db86-4292-bb62-70717ed5ac6b.png)

To evaluate our model, we used 10-fold cross validation in an encrypted state. The result is shown in [Figure 19].

![image](https://user-images.githubusercontent.com/29485153/143258923-e4a2f16c-3546-4d61-95c9-c35a1f4be93f.png)

The round trip of the encryption process is shown in [Figure 20]. The total memory usage is 1261MB, of which 1072MB is used to keep the ciphertext. The total storage usage is the size of docker.

![image](https://user-images.githubusercontent.com/29485153/143258934-2727a931-e54d-424f-ab07-7eef7ac74db0.png)

As mentioned earlier, our experimental environments Intel Xeon Platinum 8259CL CPU @ 2.50GHz processor with 8 threads. However, the actual testing environment is Intel Xeon Platinum 8180 CPU @ 2.50GHz with 4 cpu cores. As shown in Figure 20, In single core, the Geekbench 4 core score difference between both environments is 9%. In multi-core, the performance gap is increased to 45%.

![image](https://user-images.githubusercontent.com/29485153/143258959-474a1f19-7dda-48fd-b9cd-d62f809b9df5.png)

Considering the difference of the execution environment and the number of samples classified, we expect the running time to classify 2000 samples as shown in [Table 4].

![image](https://user-images.githubusercontent.com/29485153/143258970-abc47943-281e-4250-a4ef-d8d9f88b29bd.png)

**References**

[1] suncrypto/HEAAN git hub page [https://github.com/snucrypto/HEAAN](https://github.com/snucrypto/HEAAN)

[2] [CKKS17] Cheon, J. H., Kim, A., Kim, M., & Song, Y. (2017, December). Homomorphic encryption for arithmetic of approximate numbers. In International Conference on the Theory and Application of Cryptology and Information Security (pp. 409-437). Springer, Cham.

[3] SECURITY OF HOMOMORPHIC ENCRYPTION July 13-14, 2017, hosted at Microsoft Research in Redmond. http://homomorphicencryption.org/white_papers/security_homomorphic_encryption_white_paper.pdf

[4] GadgetVersus site, compare Intel Xeon Platinum 8259CL vs Intel Xeon Platinum 8180.

https://gadgetversus.com/processor/intel-xeon-platinum-8259cl-vs-intel-xeon-platinum-8180/
