#include <iostream>
#include "takeInput.h"

int main() {
    //std::cout << "Hello, ф or \u2588 \u2593 \u2592 \u2591 \n";
                                //█:192-255     ▓:128-191   ▒:64-127  ░:0-63
    std::ofstream TrainingImageFile("input/Input_Train_Images.txt", std::ios::out); //| std::ios::app
    std::ofstream TestingImageFile("input/Input_Test_Images.txt", std::ios::out); //| std::ios::app

    std::string dataTrainingImages = "train-images.idx3-ubyte";
    std::string dataTrainingLabels = "train-labels.idx1-ubyte";

    int numberOfImageSamples = 100;
    std::vector < std::vector < double > > TrainingImages;
    std::vector < double > TrainingLabels;
    read_Mnist(dataTrainingImages, TrainingImages); //vector< vector<double> >
    read_Mnist_Label(dataTrainingLabels, TrainingLabels); // vector <double>
    int images_found=0;
    std::cout<<"Number_Of_Images: "<<numberOfImageSamples<<std::endl;
    TrainingImageFile<<"Number_Of_Images: "<<numberOfImageSamples<<std::endl<<std::endl;

    for (int image = 0; image < 60000 && images_found < numberOfImageSamples; image ++){
        if (TrainingLabels[image] == 1 || TrainingLabels[image] ==0){
            //printf("Current Label = %f\n", TrainingLabels[image]);
            int count = 0;
            std::cout<<"Image_ID: "<<images_found<<std::endl;
            TrainingImageFile<<"Image_ID: "<<images_found<<std::endl;
            printf("Current_Label: %d\n", (int)TrainingLabels[image]); //t\tExpectedOutputMatrix = 
            TrainingImageFile<<"Current_Label: "<<(int)TrainingLabels[image]<<std::endl;
            for (int i=0; i<28;i++){
                for (int j=0; j<28; j++){
                    int value = TrainingImages[image][count];
                    TrainingImageFile.width(3);
                    //TrainingImageFile.fill('0');
                    TrainingImageFile<<value<<" ";
                    //printf("Current Label = %f\t\tExpectedOutputMatrix = \n", TrainingLabels[image]);
                    count++;
                }
                //std::cout<<std::endl;
                TrainingImageFile << std::endl;
            }
            images_found++;
            //std::cout<<std::endl<<std::endl;
            TrainingImageFile << std::endl<<std::endl;
        }
    }

    //////////////////Testing Image/////////////////////
    std::string dataTestingImages = "t10k-images.idx3-ubyte";
    std::string dataTestingLabels = "t10k-labels.idx1-ubyte";
    int numberOfTestingSamples = 100;
    std::vector < std::vector < double > > TestingImages;
    std::vector < double > TestingLabels;
    read_Mnist(dataTestingImages, TestingImages); //vector< vector<double> >
    read_Mnist_Label(dataTestingLabels, TestingLabels); // vector <double>
    images_found=0;
    std::cout<<"Number_Of_Images: "<<numberOfTestingSamples<<std::endl;
    TestingImageFile<<"Number_Of_Images: "<<numberOfTestingSamples<<std::endl<<std::endl;

    for (int image = 0; image < 10000 && images_found < numberOfTestingSamples; image ++){
        if (TestingLabels[image] == 1 || TestingLabels[image] ==0){
            //printf("Current Label = %f\n", TestingLabels[image]);
            int count = 0;
            std::cout<<"Image_ID: "<<images_found<<std::endl;
            TestingImageFile<<"Image_ID: "<<images_found<<std::endl;
            printf("Current_Label: %d\n", (int)TestingLabels[image]); //t\tExpectedOutputMatrix = 
            TestingImageFile<<"Current_Label: "<<(int)TestingLabels[image]<<std::endl;
            for (int i=0; i<28;i++){
                for (int j=0; j<28; j++){
                    int value = TestingImages[image][count];
                    TestingImageFile.width(3);
                    //TestingImageFile.fill('0');
                    TestingImageFile<<value<<" ";
                    //printf("Current Label = %f\t\tExpectedOutputMatrix = \n", TestingLabels[image]);
                    count++;
                }
                //std::cout<<std::endl;
                TestingImageFile << std::endl;
            }
            images_found++;
            //std::cout<<std::endl<<std::endl;
            TestingImageFile << std::endl<<std::endl;
        }
    }
}