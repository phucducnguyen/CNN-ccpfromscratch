#pragma once
#include "matrix3d.h"
class layer{
    public:
    layer(){}
    virtual void ForwardPropergate(){};
    virtual void BackPropergate(const Matrix3D& delta){};

    int getOutputHeight(){return outputHeight;};
    int getOutputWidth(){return outputWidth;};
    int getOutputDepth(){return outputDepth;};

    void setInputVolumn(Matrix3D &input);
    //virtual Matrix3D WeightedDelta() const{};//return Matrix3D (2,2,2);};
    
    Matrix3D& getOutputVolume();
    Matrix3D getDelta() const;

    protected:
    Matrix3D inputVolumn;
    int inputHeight;
    int inputWidth;
    int inputDepth;
    
    Matrix3D outputVolumn;
    int outputHeight;
    int outputWidth;
    int outputDepth;

    Matrix3D delta;

};

void layer::setInputVolumn(Matrix3D &arg){
    
    assert( arg.getHeight() == inputHeight && arg.getWidth() == inputWidth && arg.getDepth() == inputDepth);
    inputVolumn = arg;
    //std::cout<<"PRINT START"<<std::endl;
    //std::cout<<"INPUTVOLUM  inputHeight: "<<inputVolumn.getHeight()<<" inputWidth: "<<inputVolumn.getWidth()<<" inputDepth: "<<inputVolumn.getDepth()<<std::endl;
    //std::cout<< "INPUT OF CLASS InputHeight: "<<inputHeight<<" InputWidth: "<<inputWidth<<" InputDepth: "<<inputDepth<<std::endl;  
    inputVolumn.printMatrix();
    //std::cout<<"PRINT END"<<std::endl;
}

Matrix3D& layer::getOutputVolume(){
    return outputVolumn;
}

Matrix3D layer::getDelta()const{
    return delta;
}