#include "training.h"
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <ctime>

int main(){

    std::ifstream inputFile("input/input_struct_test.txt", std::ios::in);

    std::string Input_Parameter_file_name;
    std::string Testing_Output_file_name;
    std::string Input_Test_Images;
    std::string temp;
    int temp_int;
    int image_id;

    inputFile >> temp;
    if (temp.compare("Image_ID_to_test:") != 0){
        std::cout << "Error: expected Image_ID_to_test: but read " << temp<<std::endl;
        return 0;
    }
    inputFile >> image_id;
    inputFile >> temp;
    if (temp.compare("Input_Parameter_file_name:") != 0){
        std::cout << "Error: expected Input_Parameter_file_name: but read " << temp<<std::endl;
        return 0;
    }
    inputFile >> Input_Parameter_file_name;

    inputFile >> temp;
    if (temp.compare("Output_Testing_file_name:") != 0){
        std::cout << "Error: expected Output_Testing_file_name: but read " << temp<<std::endl;
        return 0;
    }
    inputFile >> Testing_Output_file_name;

    inputFile >> temp;
    if (temp.compare("Input_Test_Images_File_Name:") != 0){
        std::cout << "Error: expected Input_Test_Images_File_Name: but read " << temp<<std::endl;
        return 0;
    }
    inputFile >> Input_Test_Images;
    
    
    //std::ofstream logFile(Testing_Output_file_name, std::ios::out); //| std::ios::app
    //auto start = std::chrono::system_clock::now();
    //std::time_t start_time = std::chrono::system_clock::to_time_t(start);
    //logFile << "Testing start time: " <<std::ctime(&start_time)<<std::endl;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CNN A(Input_Parameter_file_name);
    TrainerCNN train(A);
    //logFile << "Percentage % Correct(10k test Images): " <<train.MnistTesting2()<<"%"<<std::endl<<std::endl;
    train.ImageIDTesting(Input_Test_Images,Testing_Output_file_name,image_id);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //auto end = std::chrono::system_clock::now();
    //std::chrono::duration<double> elapsed_seconds = end - start;
    //std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    //logFile << "Testing finish time: " << std::ctime(&end_time);
    //logFile << "Testing duration: "<< elapsed_seconds.count() << "s"<< std::endl;
    //logFile.close();

    return 0;
}
