#pragma once

#include <cmath>
#include <stdlib.h>
#include "layer.h"

class Conv_layer : public layer {
    public:
    Conv_layer(){}
    Conv_layer(int inputHeight,int inputWidth,int inputDepth,int filterHeight,int filterWidth,int Stride,int numFilters);
    Conv_layer(int inputHeight,int inputWidth,int inputDepth, std::istream& ins);
    Conv_layer(const char * inputfile,const char *filterfile,
        int inputHeight,int inputWidth,
        int inputDepth,int filterHeight,
        int filterWidth,int Stride,int numFilters);
    void ForwardPropergate(); 
    void BackPropergate(const Matrix3D& delta);
    float Convolution(int filter,int indexH,int indexW);// perform Convolution

    Matrix3D WeightedDelta() const;
    void reverseConv(Matrix3D& weightedDelta, int indexD, int indexH, int indexW)const;
    void incrementDw(int indexD,int indexH,int indexW);

    void setInputVolumn(const char * inputfile);
    void setLearningRate(float learningRate){this->learningRate = learningRate;};
    //void setInputFilter(Matrix3D &filter);
    void setInputFilter(const char * filterfile);

    void _randomizeFilter();
    void _iniBias();
    void updateFilterBias();
    void printFilter();
    /*
    int getOutputHeight(){return outputHeight;};
    int getOutputWidth(){return outputWidth;};
    int getOutputDepth(){return outputDepth;};
    */
    int getFilterHeight()const{return filterHeight;};
    int getFilterWidth()const{return filterWidth;};
    int getFilterDepth()const{return filterDepth;};
    int getStride()const{return Stride;};
    int getNumFilters()const{return numFilters;};

    Matrix3D getFilter(int filterIndex) const {return filter_storage[filterIndex];};
    float getBias(int biasIndex) const {return bias_storage[biasIndex];};
    private:
    Matrix3D reluOutput;// [10][10][10];
    std::vector<Matrix3D> filter_storage;
    int filterHeight;
    int filterWidth;
    int filterDepth;
    int numFilters;
    int Stride;

    std::vector<float> bias_storage;
    std::vector<float> dJdB;
    std::vector<Matrix3D> dJdW;
    float learningRate = 0.002;

};

std::ostream& operator<< (std::ostream& outs,const Conv_layer& source){
    for (int i=0; i<source.getNumFilters(); i++){
        //outs << "\tCONV Filter["<<i<<"]"<<std::endl;
        outs << source.getFilter(i) << std::endl;
        //outs << "\tCONV Bias["<<i<<"]"<<std::endl;
        outs << source.getBias(i)<<std::endl<<std::endl;
    }
    return outs;
}
std::ostream& logWeights (std::ostream& outs,const Conv_layer& source){
    for (int i=0; i<source.getNumFilters(); i++){
        outs << "\tCONV Filter["<<i<<"]"<<std::endl;
        outs << source.getFilter(i) << std::endl;
        outs << "\tCONV Bias["<<i<<"]"<<std::endl;
        outs << source.getBias(i)<<std::endl<<std::endl;
    }
    return outs;
}

///////Random function
int randNum(int min, int max){
  int x = rand() % min +  max;
  return x;
};


Conv_layer::Conv_layer(int input_Depth,int input_Height,int input_Width,int filter_Height,int filter_Width,int Stride,int num_Filters){
    
    this->inputHeight=input_Height;
    this->inputWidth=input_Width;
    this->inputDepth=input_Depth;
    this->filterHeight=filter_Height;
    this->filterWidth=filter_Width;
    this->filterDepth=input_Depth;
    this->Stride=Stride;
    this->numFilters=num_Filters;

    inputVolumn = Matrix3D(inputDepth,inputHeight,inputWidth);
    Matrix3D fil (filterDepth,filterHeight,filterWidth);
    for (size_t i= 0; i<num_Filters;i++){
        filter_storage.push_back(fil);
        dJdW.push_back(fil);
        bias_storage.push_back(0);
        dJdB.push_back(0);
    }
    _randomizeFilter();
    _iniBias();

    outputHeight = 1+(inputHeight-filterHeight)/Stride;
    outputWidth = 1+(inputWidth-filterWidth)/Stride;
    outputDepth = numFilters;
    outputVolumn = Matrix3D (outputDepth , outputHeight , outputWidth);
    std::cout<<"***CONV*** outputHeight: "<<outputHeight<<"\t"<<"outputWidth: "<<outputWidth<<"\t"<<"outputDepth: "<<outputDepth<<std::endl;
    std::cout<< "**** CONV_LAYER IS CONSTRUCTED ***"<<std::endl;
}

Conv_layer::Conv_layer(int inputDepth,int inputHeight,int inputWidth, std::istream& ins){
    this->inputHeight=inputHeight;
    this->inputWidth=inputWidth;
    this->inputDepth=inputDepth;
    ins >> this->filterDepth;
    ins >> this->filterHeight;
    ins >> this->filterWidth;
    ins >> this->Stride;
    ins >> this->numFilters;
    //std::cout <<inputHeight<<" " << inputWidth<<" " << inputDepth<<" " << filterDepth<<" " << filterHeight<<" " << filterWidth<<" " << Stride<<" " << numFilters<<std::endl;
    inputVolumn = Matrix3D(inputDepth,inputHeight,inputWidth);
    Matrix3D fil (filterDepth,filterHeight,filterWidth);
    for (size_t i= 0; i<numFilters;i++){
        dJdW.push_back(fil);
        dJdB.push_back(0);
    }

    Matrix3D temp (filterDepth,filterHeight,filterWidth);
    float tempIns=0;
    for(int filter=0; filter < numFilters; filter++){
        for (int i=0; i<filterDepth; i++){
            for (int j=0; j<filterHeight; j++){
                for (int k=0; k<filterWidth; k++){
                    ins >> tempIns;
                    temp.setElement(i,j,k,tempIns);
                }
            }
        }
        filter_storage.push_back(temp);
        ins >> tempIns;
        bias_storage.push_back(tempIns);
    }
    outputHeight = 1+(inputHeight-filterHeight)/Stride;
    outputWidth = 1+(inputWidth-filterWidth)/Stride;
    outputDepth = numFilters;
    outputVolumn = Matrix3D (outputDepth , outputHeight , outputWidth);
    
    for (int i=0; i<numFilters; i++){
        std::cout << "CONV filter ["<<i<<"]"<<std::endl;
        filter_storage[i].printMatrix();
        std::cout << "CONV bias ["<<i<<"]"<<std::endl;
        std::cout << bias_storage[i] << std::endl;
    }
    std::cout<<"***CONV*** outputHeight: "<<outputHeight<<"\t"<<"outputWidth: "<<outputWidth<<"\t"<<"outputDepth: "<<outputDepth<<std::endl;
    std::cout<< "**** CONV_LAYER IS CONSTRUCTED ***"<<std::endl;
}


Conv_layer::Conv_layer(const char * inputfile,const char *filterfile,
                    int input_Depth,int input_Height,
                    int input_Width,                
                    int filter_Height,
                    int filter_Width,
                    int Stride,
                    int num_Filters){
    this->inputHeight=input_Height;
    this->inputWidth=input_Width;
    this->inputDepth=input_Depth;
    this->filterHeight=filter_Height;
    this->filterWidth=filter_Width;
    this->Stride=Stride;
    this->numFilters=num_Filters;

    Matrix3D fil (filterDepth,filterHeight,filterWidth);
    for (size_t i= 0; i<num_Filters;i++){
        filter_storage.push_back(fil);
    }
    setInputVolumn(inputfile);
    std::cout<<"Finish Input "<<std::endl;

    setInputFilter(filterfile);
    std::cout<<"Finish Input and filter Part"<<std::endl;
    
    outputHeight = 1+(inputHeight-filterHeight)/Stride;
    outputWidth = 1+(inputWidth-filterWidth)/Stride;
    outputDepth = numFilters;
    outputVolumn = Matrix3D (outputDepth , outputHeight , outputWidth);
    /*
    // Initialize the bias.
    for (int i=0;i<numFilters;i++){
        bias_storage[i]=1;
    }
    // Initialize the filters.
    
    for(int filter=0;filter<numFilters;filter++){
        for(int depth=0;depth<inputDepth;depth++){
            for(int Height=0;Height<filterHeight;height++){ //filterHeight and Width should be the same
                for(int width=0;width<filterWidth;width++){
                    filter_storage[filter][depth][height][width]= ((std::rand()/float(RAND_MAX) *2)/10);//randNum(-1,1);
                    std::cout<<filter_storage[filter][depth][height][width]<<' ';
                }
                std::cout<<std::endl;
            }    
        }
    }
    */
}

void Conv_layer::ForwardPropergate(){
    assert((inputHeight - filterHeight)%Stride == 0); //assert((inputVolum.getHeight() - filterHeight)%Stride == 0)
    assert((inputWidth - filterWidth)%Stride == 0);
    // Perform convolution for each filter.
    //std::cout<<numFilters <<' '<< 1+(inputHeight-filterHeight)/Stride <<' '<< 1+(inputWidth-filterWidth)/Stride <<std::endl;
    Matrix3D temp(numFilters , 1+(inputHeight-filterHeight)/Stride , 1+(inputWidth-filterWidth)/Stride);
    //outputVolumn.printMatrix();
    for (int filter_index = 0; filter_index < numFilters; filter_index++){
        for (int i=0; i < temp.getHeight(); i++){//1+(inputHeight-filterHeight)/Stride
            for (int j=0; j < temp.getWidth(); j++){//1+(inputWidth-filterWidth)/Stride
                temp.setElement(filter_index,i,j,Convolution(filter_index,i,j));//Convolution(filter_index,i,j)
                
            }
        }
    }
    outputVolumn = temp;
    std::cout<<"***** CONV OUTPUT START: ******"<<std::endl;
    outputVolumn.printMatrix();
    std::cout<<"***** CONV OUTPUT END: ******"<<std::endl;
    /*
    outputDepth=numFilters;
    outputHeight= ((inputHeight-filterHeight)/Stride)+1;
    outputWidth= ((inputWidth-filterWidth)/Stride)+1;
    */
}

void Conv_layer::setInputVolumn(const char * inputfile){
    Matrix3D temp (inputfile,inputDepth,inputHeight,inputWidth);
    inputVolumn=temp;
    inputVolumn.printMatrix();
}


void Conv_layer::setInputFilter(const char * filterfile){
    std::ifstream in_filter_file(filterfile, std::ios::in);
    
    //int num_filter; /////////// importance
    int temp;
    for(int filter=0;filter<numFilters;filter++){
        /*Matrix3D fil(inputDepth,filterHeight,filterWidth);
        filter_storage.push_back(fil);*/
        for(int i=0;i<filterDepth;i++){
            for(int j=0;j<filterHeight;j++){
                for(int k=0;k<filterWidth;k++){
                    in_filter_file >> temp;
                    filter_storage[filter].setElement(i,j,k,temp);
                    std::cout<<filter_storage[filter].getElement(i,j,k)<<' ';
                }
                std::cout<<std::endl;
            }
            std::cout<<std::endl;
        }
    }

    for (int i=0;i<numFilters;i++){
        in_filter_file >> temp;
        bias_storage[i]=temp;
        std::cout<<bias_storage[i]<<' ';
    }
    std::cout<<std::endl;
}

void Conv_layer::_randomizeFilter(){
    for (int filter=0;filter<numFilters;filter++){
        for (int depth=0;depth<filterDepth;depth++){
            for (int height=0;height<filterHeight;height++){
                for (int width=0;width<filterWidth;width++){
                    filter_storage[filter].setElement(depth,height,width,(std::rand()/float(RAND_MAX) *2)/100);//randNum(-1,1)));//set element = 2. ((std::rand()/float(RAND_MAX) *2)/10);//randNum(-1,1);
                    //std::cout<<filter_storage[filter].getElement(depth,height,width)<<" ";
                }
            }
        }
    }
}

void Conv_layer::_iniBias(){
    for (int filter=0; filter<numFilters;filter++){
        //bias_storage [filter]= 0.0;
        bias_storage.push_back(0);
    }
}

void Conv_layer::updateFilterBias(){
    for (int i=0; i<numFilters; i++){
        filter_storage[i] = filter_storage[i] - dJdW[i].multConst(learningRate);
        bias_storage[i] = bias_storage[i] - dJdB[i]*learningRate;
    }
}

void Conv_layer::printFilter(){
    for (int i=0;i<numFilters; i++){
        filter_storage[i].printMatrix();
    }
}


float Conv_layer::Convolution(int filter,int indexH,int indexW){
    //std::cout<<"indexH: "<<indexH<<" indexW: "<<indexW<<std::endl;
    //std::cout<<"INPUTDEPTH:  "<<inputDepth<<std::endl;
    float sum=0;
    for(int i=0;i<filterDepth;i++){
        for(int j=0; j<filterHeight;j++){
            for(int k=0;k<filterWidth;k++){
                //std::cout<<i<<' '<<indexH+j<<' '<<indexW+k<<std::endl;
                //sum +=filter_storage[filter][i][j][k]*inputVolumn[i][indexH+j][indexW+k];
                //std::cout<<"depth: "<<i<<' '<<"height: "<<indexH*Stride+j<<' '<<"width: "<<indexW*Stride+k<<std::endl;
                //std::cout<<"inputheight: "<<inputVolumn.getHeight()<<' '<<"inputWidth: "<<inputVolumn.getWidth()<<' '<<"inputDepth: "<<inputVolumn.getDepth()<<std::endl;
                sum += filter_storage[filter].getElement(i,j,k) * inputVolumn.getElement(i,indexH*Stride+j,indexW*Stride+k);
            }
        }
    }
    //std::cout<<" sum: "<<sum<<" bias: "<< bias_storage[filter]<<std::endl;
    sum += bias_storage[filter];
    //std::cout<<" sum +=: "<<sum<< std::endl;
    //std::cout<<sum<<std::endl;
    return sum;
}

Matrix3D Conv_layer::WeightedDelta() const {
    Matrix3D temp (inputDepth,inputHeight,inputWidth);
    for (int i=0; i<outputDepth; i++){
        for (int j = 0; j < outputHeight; j++){
            for (int k = 0; k < outputWidth; k++){
                reverseConv(temp,i,j,k);
            }
        }
    }
    std::cout<<"Weighted delta: "<<std::endl;
    temp.printMatrix();
    std::cout<<"Weighted delta End"<<std::endl;
    return temp;
}

void Conv_layer::reverseConv(Matrix3D& weightedDelta ,int indexD, int indexH, int indexW)const{
    float product;
    for(int i=0;i<filterDepth;i++){
        for(int j=0; j<filterHeight;j++){
            for(int k=0;k<filterWidth;k++){
                product = delta.getElement(indexD,indexH,indexW) * filter_storage[indexD].getElement(i,j,k);
                product += weightedDelta.getElement(i,indexH*Stride+j,indexW*Stride+k);
                weightedDelta.setElement(i,indexH*Stride+j,indexW*Stride+k, product );
            }
        }
    }
    //std::cout<<"Weighted delta: "<<std::endl;
    //weightedDelta.printMatrix();
    //std::cout<<"Weighted delta End"<<std::endl;
}

void Conv_layer::BackPropergate(const Matrix3D& delta){
    assert (delta.getDepth() == outputDepth && delta.getHeight() == outputHeight && delta.getWidth() == outputWidth);

    this->delta = delta;
    int count;
    float sum;
    for (int i=0; i< delta.getDepth(); i++){
        sum =0;
        for(int j=0; j<delta.getHeight(); j++){
            for (int k=0; k<delta.getWidth(); k++){
                sum += delta.getElement(i,j,k);
            }
        }
        sum = sum / (delta.getHeight() * delta.getWidth());
        dJdB[i] = sum;
    }
    /*
    for(int i=0; i< numFilters; i++){
        dJdW[i] = Matrix3D(filterDepth,filterHeight,filterWidth);
    }*/
    for (int i=0; i< delta.getDepth(); i++){
        count = 0;
        for(int j=0; j<delta.getHeight(); j++){
            for (int k=0; k<delta.getWidth(); k++){
                if (delta.getElement(i,j,k)!=0){
                    count++;
                    incrementDw(i,j,k);
                }
            }
        }
        /*
        std::cout<<"Delta: "<<std::endl;
        delta.printMatrix();
        std::cout<<"dJdW at ["<<i<<"]: "<<std::endl;
        dJdW[i].printMatrix();
        std::cout<< " COUNT: "<<count<<std::endl;
        */
        if (count != 0){
            dJdW[i]=dJdW[i].multConst(1.0/count);
        }
        
        std::cout<<"dJdW after average at ["<<i<<"]: "<<std::endl;
        dJdW[i].printMatrix();
        
    }
    std::cout<<"filter before update: "<<std::endl;
    printFilter();
    updateFilterBias();
    std::cout<<"filter after update: "<<std::endl;
    printFilter();
}

void Conv_layer::incrementDw(int indexD,int indexH,int indexW){
    float  product;
    float value = delta.getElement(indexD,indexH,indexW);
    /*
    std::cout<<"delta at "<<indexD<<" "<<indexH<<" "<<indexW<<" : "<<value<<std::endl;
    std::cout<<"Delta (private member): "<<std::endl;
    delta.printMatrix();
    std::cout<<"AFTER dJdW at ["<<indexD<<"]: "<<std::endl;
    dJdW[indexD].printMatrix();
    */
    for(int i=0;i<filterDepth;i++){
        for(int j=0; j<filterHeight;j++){
            for(int k=0;k<filterWidth;k++){
                product = value*inputVolumn.getElement(i,indexH*Stride+j,indexW*Stride+k);
                product += dJdW[indexD].getElement(i,j,k);
                dJdW[indexD].setElement(i,j,k,product);
            }
        }
    }
    //std::cout<<"AFTER dJdW at ["<<indexD<<"]: "<<std::endl;
    //dJdW[indexD].printMatrix();
}