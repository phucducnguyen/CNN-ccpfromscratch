#include "training.h"
#include <stdio.h>
#include <string.h>

int main(){
    std::srand(std::time(NULL));
    std::ifstream inputFile("input/input_struct_train.txt", std::ios::in);
    std::string temp;
    int conv;
    int pool;
    int fc;
    int image_depth;
    int image_height;
    int image_width;
    int numberOfConvFilters;
    int numberOfFCNeurons;
    int Number_Of_Outputs;
    int Epoch;
    int Number_Of_Image_Samples;
    float LearningRate;
    std::string Input_Images_File_Name;
    std::string Parameter_file_name;
    std::string Log_file_name;
    std::string Average_cost_file;

    
    inputFile >> temp;
    if (temp.compare("Number_Of_Conv_Layers_Per_Pool:") != 0){
        std::cout << "Error: expected Number_Of_Conv_Layers_Per_Pool: but read " << temp<<std::endl;
        return 0;
    }
    inputFile >> conv;
    inputFile >> temp;
    if (temp.compare("Number_Of_Pooling_Layers:") != 0){
        std::cout << "Error: expected Number_Of_Pooling_Layers: but read " << temp<<std::endl;
        return 0;
    }
    inputFile >> pool;
    inputFile >> temp;
    if (temp.compare("Number_Of_ANN_Hidden_Layers:") != 0){
        std::cout << "Error: expected Number_Of_ANN_Hidden_Layers: but read " << temp<<std::endl;
        return 0;
    }
    inputFile >> fc;
    inputFile >> temp;
    if (temp.compare("Image_Depth:") != 0){
        std::cout << "Error: expected Image_Depth: but read " << temp<<std::endl;
        return 0;
    }
    inputFile >> image_depth;
    inputFile >> temp;
    if (temp.compare("Image_Height:") != 0){
        std::cout << "Error: expected Image_Height: but read " << temp<<std::endl;
        return 0;
    }
    inputFile >> image_height;
    inputFile >> temp;
    if (temp.compare("Image_Width:") != 0){
        std::cout << "Error: expected Image_Width: but read " << temp<<std::endl;
        return 0;
    }
    inputFile >> image_width;
/////////////////////
    inputFile >> temp;
    if (temp.compare("Number_Of_Filters_Per_Conv_Layer:") != 0){
        std::cout << "Error: expected Number_Of_Filters_Per_Conv_Layer: but read " << temp<<std::endl;
        return 0;
    }
    inputFile >> numberOfConvFilters;
    inputFile >> temp;
    if (temp.compare("Number_Of_ANN_Hidden_Layer_Neurons:") != 0){
        std::cout << "Error: expected Number_Of_ANN_Hidden_Layer_Neurons: but read " << temp<<std::endl;
        return 0;
    }
    inputFile >> numberOfFCNeurons;
//////////////////////////////
    inputFile >> temp;
    if (temp.compare("Number_Of_Outputs:") != 0){
        std::cout << "Error: expected Number_Of_Outputs: but read " << temp<<std::endl;
        return 0;
    }
    inputFile >> Number_Of_Outputs;
    inputFile >> temp;
    if (temp.compare("Epoch:") != 0){
        std::cout << "Error: expected Epoch: but read " << temp<<std::endl;
        return 0;
    }
    inputFile >> Epoch;
    inputFile >> temp;
    if (temp.compare("Number_Of_Image_Samples:") != 0){
        std::cout << "Error: expected Number_Of_Image_Samples: but read " << temp<<std::endl;
        return 0;
    }
    inputFile >> Number_Of_Image_Samples;
    inputFile >> temp;
    if (temp.compare("LearningRate:") != 0){
        std::cout << "Error: expected LearningRate: but read " << temp<<std::endl;
        return 0;
    }
    inputFile >> LearningRate;
    inputFile >> temp;
    if (temp.compare("Input_Train_Images_File_Name:") != 0){
        std::cout << "Error: expected Input_Train_Images_File_Name: but read " << temp<<std::endl;
        return 0;
    }
    inputFile >> Input_Images_File_Name;
    
    inputFile >> temp;
    if (temp.compare("Parameter_file_name:") != 0){
        std::cout << "Error: expected Parameter_file_name: but read " << temp<<std::endl;
        return 0;
    }
    inputFile >> Parameter_file_name;
    inputFile >> temp;
    if (temp.compare("Log_file_name:") != 0){
        std::cout << "Error: expected Log_file_name: but read " << temp<<std::endl;
        return 0;
    }
    inputFile >> Log_file_name;
    inputFile >> temp;
    if (temp.compare("Average_cost_file:") != 0){
        std::cout << "Error: expected Average_cost_file: but read " << temp<<std::endl;
        return 0;
    }
    inputFile >> Average_cost_file;


//////////////////////////////////////////////////////////////
    CNN A(conv,pool,fc,image_depth,image_height,image_width,Number_Of_Outputs,numberOfConvFilters,numberOfFCNeurons);//(int conv,int pool,int fc,int inputDepth,int inputHeight,int inputWidth, int numOfOutput)
    TrainerCNN train(A,Parameter_file_name,Log_file_name,Average_cost_file); //CNN& A,std::string parameterFile, std::string logFile, std::string average_costFile
    try{
        train.InputFileTrain(Input_Images_File_Name,Epoch,Number_Of_Image_Samples,LearningRate); //int epoch, int numberOfImageSamples, learningRate
    }
    catch(std::string e){
        std::cout << e << std::endl;
        return 0;
    }
    A.writeCNN(Parameter_file_name);
///////////////////////////////////////////////////////////////
    //std::cout <<"Hello, this is after catch!!"<<std::endl;
    return 0;
}