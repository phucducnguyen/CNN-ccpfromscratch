
The Program runs on G++ version 7.x.x or higher.

Modify the struct_train.txt to configure the training and struct_test.txt to configure the testing CNN.

TO GENERATE Input_Train_Images.txt, Modify NumberOfImages and COMPILE: g++ mainUTF.cpp

TO COMPILE THE CNN: g++ -w train.cpp -static-libstdc++

The CNN will give three outputs in /output directory ( CNN_numberOfIterations_numberOfImages)

This model will automatically use the MNIST training data (train-images.idx3-ubyte && train-labels.idx1.ubyte)

