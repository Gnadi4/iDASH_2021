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
