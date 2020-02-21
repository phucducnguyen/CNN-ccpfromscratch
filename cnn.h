#pragma once
#include "layer.h"
#include "conv_layer.h"
#include "fc_layer.h"
#include "relu_layer.h"
#include "pool_layer.h"

class CNN{
    public:
    CNN(){};
    CNN(int conv,int pool,int fc,int inputDepth,int inputHeight,int inputWidth, int numOutput, int numOfConvFilter, int numOfHiddenNeurons);
    CNN(std::string file);
    void Forward(const Matrix3D &input);
    void Back();
    float Cost(); //Matrix3D expectedOutput
    Matrix3D CostPrime();

    //GET METHOD
    Matrix3D getOuputVolumn(){return this->outputVolumn;};
    int getConv(){return this->conv;};
    int getPool(){return this->pool;};
    int getFC(){return this->fc;};


    //SET METHOD
    void SetExpectedOutput(Matrix3D expectedOutput);
    void setConv(int conv){this->conv = conv;};
    void setPool(int pool){this->pool = pool;};
    void setFc(int fc){this->fc = fc;};
    void setInputDepth(int inputDepth){this->inputDepth = inputDepth;};
    void setInputHeight(int inputHeight){this->inputHeight = inputHeight;};
    void setInputWidth(int inputWidth){this->inputWidth = inputWidth;};
    void setNumOfOutput(int numberOfOutput){this->numberOfOutput = numberOfOutput;};
    void setLearningRate(float learningRate);

    void writeCNN(std::string file);
    void logCNN(std::string file,int epoch);
    //void 

    private:
    Matrix3D inputVolumn;
    Matrix3D outputVolumn;
    Matrix3D expectedOutput;
    std::vector <layer*> layers;
    int conv;
    int pool;
    int fc;
    int inputDepth;
    int inputHeight;
    int inputWidth;
    int numberOfOutput;
    float cost;
    float learningRate;
};

CNN::CNN(int conv,int pool,int fc,int inputDepth,int inputHeight,int inputWidth, int numOfOutput, int numOfConvFilter, int numOfHiddenNeurons){
    //layer* A = new Conv_layer(inputDepth,inputHeight,inputWidth,3,3,2,2); // inputDepth,inputHeight, inputWidth,  filterHeight, filterWidth, Stride, numFilter   inputDepth,inputHeight,inputWidth,3,3,2,2
    //layer* B = new Relu_layer("textfile/relu_input_layer.txt",2,3,3);
    //layer* C = new Pool_layer("textfile/pooling_input_layer.txt",6,6,3,2,2);
    //layer* D = new FC_layer("textfile/FC_input_layer.txt",7,7,3,10);
    this->conv = conv;
    this->pool = pool;
    this->fc = fc;
    this->inputHeight = inputHeight;
    this->inputWidth = inputWidth;
    this->inputDepth = inputDepth;
    int tempHeight = inputHeight;
    int tempWidth = inputWidth;
    int tempDepth = inputDepth;
    this->numberOfOutput = numOfOutput;
    Matrix3D temp;
    for (int j = 0;j < pool; j++){
        for (int i = 0; i < conv; i++){
            layers.push_back(new Conv_layer(tempDepth,tempHeight,tempWidth,5,5,1,numOfConvFilter));// inputDepth,inputHeight, inputWidth,  filterHeight, filterWidth, Stride, numFilter 
            tempHeight = (*layers[layers.size()-1]).getOutputHeight();
            tempWidth = (*layers[layers.size()-1]).getOutputWidth();
            tempDepth = (*layers[layers.size()-1]).getOutputDepth();
            //(*layers[0]).getOutputVolume();
            //Matrix3D temp = (*layers[0]).getOutputVolume();
            layers.push_back(new Relu_layer (tempDepth,tempHeight,tempWidth)); //int depth, int height, int width
            tempHeight = (*layers[layers.size()-1]).getOutputHeight();
            tempWidth = (*layers[layers.size()-1]).getOutputWidth();
            tempDepth = (*layers[layers.size()-1]).getOutputDepth();
        }
        layers.push_back(new Pool_layer (tempDepth,tempHeight,tempWidth, 2, 2)); //int depth, int height, int width,int stride, int spatialExtent
        tempHeight = (*layers[layers.size()-1]).getOutputHeight();
        tempWidth = (*layers[layers.size()-1]).getOutputWidth();
        tempDepth = (*layers[layers.size()-1]).getOutputDepth();
    }
    for (int k = 0;k < fc; k++){
        layers.push_back(new FC_layer (tempDepth,tempHeight,tempWidth, numOfHiddenNeurons)); //int depth, int height, int width, int numFilter
        tempHeight = (*layers[layers.size()-1]).getOutputHeight();
        tempWidth = (*layers[layers.size()-1]).getOutputWidth();
        tempDepth = (*layers[layers.size()-1]).getOutputDepth();
        layers.push_back(new Relu_layer (tempDepth,tempHeight,tempWidth)); //int depth, int height, int width
        tempHeight = (*layers[layers.size()-1]).getOutputHeight();
        tempWidth = (*layers[layers.size()-1]).getOutputWidth();
        tempDepth = (*layers[layers.size()-1]).getOutputDepth();
    }
    layers.push_back(new FC_layer (tempDepth,tempHeight,tempWidth, numberOfOutput)); //int depth, int height, int width, int numFilter
}

void CNN::Forward(const Matrix3D &input){
    this->inputVolumn = input;
    Matrix3D temp = input;
    for (int i = 0; i<layers.size(); i++){
        (*layers[i]).setInputVolumn(temp);
        (*layers[i]).ForwardPropergate();
        temp = (*layers[i]).getOutputVolume();
    }
    outputVolumn = (*layers[layers.size()-1]).getOutputVolume();
    //outputVolumn.printMatrix();
}

float CNN::Cost(){
    //std::cout<<std::endl<<" outVolumn - expectedOutput: " << std::endl;
    //(outputVolumn - expectedOutput).printMatrix();
    //std::cout<<"(outputVolumn - expectedOutput).vectorNorm(): "<<(outputVolumn - expectedOutput).vectorNorm()<<std::endl;
    cost = ((outputVolumn - this->expectedOutput).vectorNorm())/2 ;
    //J(W,b;x,y)=12∥hW,b(x)−y∥2.
    return cost;
}
// −(yi−a(nl)i)
Matrix3D CNN::CostPrime(){
    Matrix3D deltaPrime (numberOfOutput,1,1);
    deltaPrime = (outputVolumn - expectedOutput);
    return deltaPrime;
}

void CNN::Back(){
    std::cout<<"BACK PROPAGATION"<<std::endl;
    Matrix3D temp = CostPrime();
    //std::cout<<"CostPrime: "<<std::endl;
    //temp.printMatrix();
    for (int i = layers.size()-1; i>=1; i--){
        (*layers[i]).BackPropergate(temp);
        if (FC_layer* t = dynamic_cast <FC_layer*> (layers[i])){
            temp = (*t).WeightedDelta();
        }
        else if (Conv_layer* t = dynamic_cast <Conv_layer*> (layers[i])){
            temp = (*t).WeightedDelta();
        }
        else {
            temp = (*layers[i]).getDelta();
        }
    }
    (*layers[0]).BackPropergate(temp);
}


void CNN::SetExpectedOutput(Matrix3D expectedOutput){
    assert(expectedOutput.getDepth() == numberOfOutput && expectedOutput.getHeight() == 1 && expectedOutput.getWidth() == 1);
    this->expectedOutput = expectedOutput;
}

void CNN::setLearningRate(float learningRate){
    for (int i = 0; i<layers.size(); i++){
        if (FC_layer* t = dynamic_cast <FC_layer*> (layers[i])){
            (*t).setLearningRate(learningRate);
        }
        else if (Conv_layer* t = dynamic_cast <Conv_layer*> (layers[i])){
            (*t).setLearningRate(learningRate);
        }
    }
}

void CNN::writeCNN(std::string file){
    std::ofstream outputFile(file, std::ios::out);
    outputFile << conv <<" "<< pool <<" "<< fc <<" "<<inputDepth<<" "<<inputHeight<<" "<<inputWidth  <<" "<<numberOfOutput<<std::endl;
    //outputFile <<"Conv: "<< conv <<" Pool: "<< pool <<" FC: "<< fc <<" inputHeight: "<<inputHeight<<" inputWidth: "<<inputWidth<<" inputDepth: "<<inputDepth 
        //<<" numberOfOutput: "<<numberOfOutput<<std::endl;
    
    for (int i=0;i<layers.size();i++){
        if (Conv_layer* t = dynamic_cast <Conv_layer*> (layers[i])){
            //outputFile << "\t\tCONV Layer:"<<std::endl;
            outputFile << t->getFilterDepth() <<" "<< t->getFilterHeight() <<" "<< t->getFilterWidth() <<" "<< t->getStride() <<" "<< t->getNumFilters() <<std::endl;
            outputFile << *t <<std::endl;
        }
        if (Pool_layer* t = dynamic_cast <Pool_layer*> (layers[i])){
            //outputFile << "\t\tPOOL Layer:"<<std::endl;
            outputFile << t->getStride() <<" "<< t->getSpatialExtent() <<std::endl<<std::endl;
        }
        if (FC_layer* t = dynamic_cast <FC_layer*> (layers[i])){
            //outputFile << "\t\tFC Layer:"<<std::endl;
            outputFile << t->getFilterDepth() <<" "<< t->getFilterHeight() <<" "<< t->getFilterWidth() <<" "<< t->getNumFilters() <<std::endl;
            outputFile << *t <<std::endl;
        }
    }
}

void CNN::logCNN(std::string file, int epoch){
    std::ofstream outputFile(file, std::ios::out | std::ofstream::app);
    outputFile <<"\tCurrent Epoch: "<< epoch<<std::endl<<std::endl;
        
    for (int i=0;i<layers.size();i++){
        if (Conv_layer* t = dynamic_cast <Conv_layer*> (layers[i])){
            outputFile << "\t\tCONV Layer:"<<std::endl;
            outputFile << "Filter Depth: "<<t->getFilterDepth() <<" Filter Height: "<< t->getFilterHeight() <<" Filter Width: "<< t->getFilterWidth()<<std::endl;
            outputFile << "Stride: "<< t->getStride() <<" Number of Filter:"<< t->getNumFilters() <<std::endl;
            logWeights(outputFile,*t);
            outputFile << std::endl;
        }
        if (Pool_layer* t = dynamic_cast <Pool_layer*> (layers[i])){
            outputFile << "\t\tPOOL Layer:"<<std::endl;
            outputFile << "Stride: "<<t->getStride() <<" Filter size: "<< t->getSpatialExtent() <<std::endl<<std::endl;
        }
        if (FC_layer* t = dynamic_cast <FC_layer*> (layers[i])){
            outputFile << "\t\tFC Layer:"<<std::endl;
            outputFile << "Filter Depth: "<<t->getFilterDepth() <<" Filter Height: "<< t->getFilterHeight() <<" Filter Width: "<<t->getFilterWidth()<<std::endl;
            outputFile << "Number of Filter: "<< t->getNumFilters() <<std::endl;
            logWeights(outputFile,*t);
            outputFile << std::endl;
        }
    }
    outputFile << std::endl << std::endl;
    outputFile.close();
}


CNN::CNN(std::string file){
    std::ifstream inputFile(file, std::ios::in);
    inputFile >> this->conv >> this->pool >> this->fc >> this->inputDepth >> this->inputHeight >> this->inputWidth >> this->numberOfOutput;
    std::cout << conv <<" "<< pool <<" "<< fc <<" "<<inputDepth  <<" "<<inputHeight<<" "<<inputWidth<<" "<<numberOfOutput<<std::endl;
    int tempHeight = inputHeight;
    int tempWidth = inputWidth;
    int tempDepth = inputDepth;
    //Matrix3D temp;
    //Conv_layer(inputHeight,inputWidth,inputDepth, inputFile);
    
    for (int j = 0;j < pool; j++){
        for (int i = 0; i < conv; i++){
            layers.push_back(new Conv_layer(tempDepth,tempHeight,tempWidth, inputFile));// inputDepth,inputHeight, inputWidth,  filterHeight, filterWidth, Stride, numFilter 
            tempHeight = (*layers[layers.size()-1]).getOutputHeight();
            tempWidth = (*layers[layers.size()-1]).getOutputWidth();
            tempDepth = (*layers[layers.size()-1]).getOutputDepth();
            //(*layers[0]).getOutputVolume();
            //Matrix3D temp = (*layers[0]).getOutputVolume();
            layers.push_back(new Relu_layer (tempDepth,tempHeight,tempWidth)); //int depth, int height, int width
            tempHeight = (*layers[layers.size()-1]).getOutputHeight();
            tempWidth = (*layers[layers.size()-1]).getOutputWidth();
            tempDepth = (*layers[layers.size()-1]).getOutputDepth();
        }
        layers.push_back(new Pool_layer (tempDepth,tempHeight,tempWidth, inputFile)); //int depth, int height, int width,int stride, int spatialExtent
        tempHeight = (*layers[layers.size()-1]).getOutputHeight();
        tempWidth = (*layers[layers.size()-1]).getOutputWidth();
        tempDepth = (*layers[layers.size()-1]).getOutputDepth();
    }
    for (int k = 0;k < fc; k++){
        layers.push_back(new FC_layer (tempDepth,tempHeight,tempWidth, inputFile)); //int depth, int height, int width, int numFilter
        tempHeight = (*layers[layers.size()-1]).getOutputHeight();
        tempWidth = (*layers[layers.size()-1]).getOutputWidth();
        tempDepth = (*layers[layers.size()-1]).getOutputDepth();
        layers.push_back(new Relu_layer (tempDepth,tempHeight,tempWidth)); //int depth, int height, int width
        tempHeight = (*layers[layers.size()-1]).getOutputHeight();
        tempWidth = (*layers[layers.size()-1]).getOutputWidth();
        tempDepth = (*layers[layers.size()-1]).getOutputDepth();
    }
    layers.push_back(new FC_layer (tempDepth,tempHeight,tempWidth, inputFile)); //int depth, int height, int width, int numFilter
}
