#include <vector>
#include <cmath>
#include <stdlib.h>
#include "layer.h"

class Relu_layer : public layer{
    public:
    Relu_layer(){}
    Relu_layer(Matrix3D input);//,int depth,int height,int width);
    Relu_layer(const char * inputfile,int depth,int height,int width);
    Relu_layer(int depth, int height, int width);
    void ForwardPropergate();
    void BackPropergate(const Matrix3D& nextLayerDelta);
    Matrix3D reluPrime();
    private:
    /*
    Matrix3D inputVolumn;
    Matrix3D outputVolumn;
    
    int inputHeight;
    int inputWidth;
    int inputDepth; 
    */
};
Relu_layer::Relu_layer(int depth, int height, int width){
    this->inputHeight = height;
    this->inputWidth = width;
    this->inputDepth = depth;
    this->outputHeight = height;
    this->outputWidth = width;
    this->outputDepth = depth;
    std::cout<<"***RELU*** outputHeight: "<<outputHeight<<"\t"<<"outputWidth: "<<outputWidth<<"\t"<<"outputDepth: "<<outputDepth<<std::endl;
    std::cout<< "**** RELU_LAYER IS CONSTRUCTED ***"<<std::endl;
}

Relu_layer::Relu_layer(Matrix3D input){
    assert(input.getHeight() !=0 && input.getWidth() !=0 && input.getDepth() !=0);

    this->inputDepth=input.getDepth();
    this->inputHeight=input.getHeight();
    this->inputWidth=input.getWidth();

    inputVolumn = input;
    inputVolumn.printMatrix();

    
}
Relu_layer::Relu_layer(const char * inputfile,int depth,int height,int width){
    this->inputDepth=depth;
    this->inputHeight=height;
    this->inputWidth=width;

    Matrix3D temp(inputfile,depth,height,width);
    inputVolumn = temp;
    inputVolumn.printMatrix();
}
void Relu_layer::ForwardPropergate(){
    outputVolumn = inputVolumn;
    //std::cout<< "RELU  InputHeight: "<<inputHeight<<" InputWidth: "<<inputWidth<<" InputDepth: "<<inputDepth<<std::endl;

    for(int i=0;i<inputDepth;i++){
        for(int j=0;j<inputHeight;j++){
            for(int k=0;k<inputWidth;k++){
                //std::cout<< "Relu i: "<<i<<" j: "<<j<<" k: "<<k<<std::endl;
                if(outputVolumn.getElement(i,j,k) <= 0) outputVolumn.setElement(i,j,k,0);
                //std::cout<<outputVolumn.getElement(i,j,k)<<' ';
            }
            //std::cout<<std::endl;
        }
        //std::cout<<std::endl;
    }
    std::cout<<"***** RELU OUTPUT START: ******"<<std::endl;
    outputVolumn.printMatrix();
    std::cout<<"***** RELU OUTPUT END: ******"<<std::endl;
}

Matrix3D Relu_layer::reluPrime(){

    Matrix3D temp (inputDepth,inputHeight,inputWidth);//(inputVolumn);
    for(int i=0;i<inputDepth;i++){
        for(int j=0;j<inputHeight;j++){
            for(int k=0;k<inputWidth;k++){
                //std::cout<< "Relu i: "<<i<<" j: "<<j<<" k: "<<k<<std::endl;
                if(inputVolumn.getElement(i,j,k) > 0){
                    temp.setElement(i,j,k,1);
                }
                //std::cout<<outputVolumn.getElement(i,j,k)<<' ';
            }
        }
    }
    return temp;
}

void Relu_layer::BackPropergate(const Matrix3D& nextLayerDelta){

    delta = nextLayerDelta.elementMult( reluPrime() );
    /*
    std::cout<< "nextLayerDelta: "<<std::endl;
    nextLayerDelta.printMatrix();
    std::cout<< "ReluPrime: "<<std::endl;
    reluPrime().printMatrix();
    std::cout<< "Delta: "<<std::endl;
    delta.printMatrix();
    */
}