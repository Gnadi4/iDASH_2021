# iDASH_2021 track 2 challenge code

![2021-11-22_21-09-57](https://user-images.githubusercontent.com/29485153/142859463-a97c245a-9f19-4dc4-aec7-ec7093763515.png)

## Introduction(Challenge)

This description is the solution code of team Data Science Dept. in SeoulTech for the track 2 of idash-2021 competition. 

## Code description

### preprocessor.cpp

In the preprocessing code, putting test dataset as a single line fasta format, you can get a data.csv after preprocessing

### model.cpp

In the model container, putting data.csv, the result of preprocessing, you can get a result.csv, the evaluation of each class for each sample.

### docker file link

https://hub.docker.com/repository/docker/gnadi4/model
https://hub.docker.com/repository/docker/gnadi4/preprocessing

## 챌린지 소개

iDASH 챌린지 소개 페이지 : [http://www.humangenomeprivacy.org/2021/competition-tasks.html](http://www.humangenomeprivacy.org/2021/competition-tasks.html)

데이터 다운로드 URL : [http://www.humangenomeprivacy.org/2021/competition-tasks.html](http://www.humangenomeprivacy.org/2021/competition-tasks.html)

-일정

![https://s3-us-west-2.amazonaws.com/secure.notion-static.com/a416b92e-3b47-4f21-a557-2b9e5ad20e23/Untitled.png](https://s3-us-west-2.amazonaws.com/secure.notion-static.com/a416b92e-3b47-4f21-a557-2b9e5ad20e23/Untitled.png)

### Introduction

- 해당 과제는 주어진 바이러스 게놈(즉, COVID19 게놈)을 4가지 다른 균주 중 하나로 분류하는 안전한 방법을 개발하는 것

### Track Description

- 각 팀에게 각각의 균주 정보와 함께 4개의 다른 균주(총 8000개의 게놈)에 대한 2000개의 전체 게놈이 포함된 챌린지 데이터 세트 제공 → 참가자를 위한 학습 데이터
- 챌린지 기간이 끝나면 정확도와 시간 측면에서 균주 당 500 개의 게놈(총 2000개의 게놈)이 포함된 hold-out 데이터 셋을 사용하여 솔루션을 평가
- **암호화된 게놈 입력 데이터에 대한 분류 작업을 안전하고 효율적으로 수행 할 수 있는 분류기를 개발하는것이 이번 챌린지의 과제**
- 암호화 전에 입력 게놈을 전처리 해야 할 수 도 있음
    - 해당 코드를 통해 전처리 진행
- 게놈 데이터가 상당히 크기 때문에 참가자가 원하는 전처리 수행가능
- 또한 다음의 조건이 해당 시간에 충족되는 한 작년과 달리 암호화 되기 전에 입력 게놈의 전처리에 대한 제약이 없음(도커를 통한 환경 제약 추가)
    1. 전처리의 메모리 사용량은 평가(evaluation) 데이터 셋의 게놈 2000개에 대해 1GB를 초과하지 않음
    2. 전처리 시간 사용량은 1CPU 코어에서 2000개의 게놈에 대해 1분을 초과하지 않음 - Intel Xeon CPU에서 OS에 대한 간섭없이 테스트를 수행. 즉, 방법을 평가하는 동안 다른 프로그램이 실행되지 않음

### Challenge Dataset and Files

- 챌린지 데이터 세트에는 공개적으로 사용할 수 있는 완전한 게놈이 포함
- 4개의 다른 바이러스 균주에 대한 게놈 서열을 포함하는 하나의 fasta 파일이 있음
- FASTA 포맷은 염기서열 혹은 단백질서열을 문자열로 표현한 것
- 각 바이러스 게놈의 첫 줄은 ID (균주 식별자)

![https://s3-us-west-2.amazonaws.com/secure.notion-static.com/a1bfb446-7dca-488a-897f-b72244a0fe56/2021-05-26_16-20-55.png](https://s3-us-west-2.amazonaws.com/secure.notion-static.com/a1bfb446-7dca-488a-897f-b72244a0fe56/2021-05-26_16-20-55.png)

### Encryption Requirement

- HE 방식의 보안 수준은 128비트 이상으로 설정 되어야함

### Evaluation Environment

- 모든 제출은 Docker 컨테이너를 사용하여 평가됨. 컨테이너는 평가에서 CPU 코어 4개 (Intel Xeon Platinum 8180 CPU @ 2.50GHz), 32GB 메모리 및 500GB 스토리지로 제한
- 제출 파일을 도커 형태로 제출

## Homomorphic Encryption-based Secure Viral Strain Classification

Team name : Data Science Dept. in SeoulTech

### Method

  We used the open version of the HEaaN library$^{1}$ the implementation of CKKS$^{2}$ scheme, and logistic regression for multi-label classification in this task. Our solution consists of three steps: preprocessing, training, and inference

![2021-11-24_22-21-02.png](https://s3-us-west-2.amazonaws.com/secure.notion-static.com/9ed64233-9753-4f4d-aba7-430bacb91262/2021-11-24_22-21-02.png)

### **2.1 Notation**

(skip)

### 2.2 Preprocessing

  To vectorize a viral genome, we firstly extract all subsequences of a predefined length from full genomes given for training. We consider the subsequences of length 4 to manage the number of features based on the empirical observation: the shorter the subsequence length, the less the number of features. For example, when the subsequence length is set to 1,000, 24,556 unique subsequences are obtained, while if it is set to 4, the number of generated unique subsequences is 980. These 980 subsequences are denoted as the candidate features.

  Given the generated candidate features, a genome sequence can be transformed into a vector with several strategies such as binary encoding, counting, etc. In this work, we employ a counting approach to vectorize given genomes. Specifically, the entire genome sequence is represented as a vector of integers where each element denotes the number of subsequences matched with the corresponding feature.

  With the candidate subsequences, we conduct a feature selection process to further reduce the number of features to be used for building a classification model. For the feature selection, we use the RFE method implemented in the sklearn library. RFE (abbreviation of "Recursive Feature Elimination") is a process of selecting the predefined number of optimal features by removing the most unnecessary features one by one based on the importance of each feature with a linear model (i.e., logistic regression explained in the subsequent section). In this feature selection process, we consider 40, 60, and 120 as the number of features to be selected.

  To find the optimal number of features in terms of predictive performance, randomly selected 70% genomes were used for training, and the remaining 30% genomes were used to measure the performance. We repeated this procedure 100 times for a rigorous comparison. From the experiments, we observed that the selected 120 features through RFE show slightly the better performance.

![2021-11-24_22-33-55.png](https://s3-us-west-2.amazonaws.com/secure.notion-static.com/0ebd5bff-798b-4912-840c-4b648052d6a5/2021-11-24_22-33-55.png)

The final selected 120 features are listed as follows:

![2021-11-24_22-34-33.png](https://s3-us-west-2.amazonaws.com/secure.notion-static.com/d82b6000-e835-423f-82eb-a793abfc2daf/2021-11-24_22-34-33.png)

### **2.3 Training Logistic-regression Model**
