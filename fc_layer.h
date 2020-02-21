#include <cmath>
#include <stdlib.h>
#include "layer.h"

class FC_layer : public layer{
    public:
    FC_layer(){};
    FC_layer(int depth, int height, int width, int numFilter);
    FC_layer(int depth, int height, int width, std::istream& file);
    FC_layer(const char * inputfile,
        int inputHeight,int inputWidth,
        int inputDepth,int numFilters);
    void ForwardPropergate();
    void BackPropergate(const Matrix3D& delta);

    float _specialDot(int filter);//, int row, int col, int depth);
    void _randomizeFilter();
    void _iniBias();
    void setInputVolumn(const char * inputfile);
    void setLearningRate(float learningRate){this->learningRate = learningRate;};
    //void setInputVolumn(Matrix3D &arg);
    Matrix3D WeightedDelta() const; //const
    void updateFilterBias();

    void printFilter();

    int getFilterHeight()const{return filterHeight;};
    int getFilterWidth()const{return filterWidth;};
    int getFilterDepth()const{return filterDepth;};
    int getNumFilters()const{return numFilters;};
    Matrix3D getFilter(int filterIndex) const {return filter_storage[filterIndex];};
    float getBias(int biasIndex) const {return bias_storage[biasIndex];};
    private:
    //float fcOutput [10][10][10];
    //std::vector < float [100][100][100]> *filterList;
    std::vector<Matrix3D> filter_storage; //[num_filter][depth][heigh][width]
    int filterHeight;
    int filterWidth;
    int filterDepth;
    int numFilters;
    std::vector<float> bias_storage;
    std::vector<float> dJdB;
    std::vector<Matrix3D> dJdW;
    float learningRate = 0.002;
};

std::ostream& operator<< (std::ostream& outs,const FC_layer& source){
    for (int i=0; i<source.getNumFilters(); i++){
        //outs << "\tFC Filter["<<i<<"]"<<std::endl;
        outs << source.getFilter(i) << std::endl;
        //outs << "\tFC Bias["<<i<<"]"<<std::endl;
        outs << source.getBias(i)<<std::endl<<std::endl;
    }
    return outs;
}
std::ostream& logWeights (std::ostream& outs,const FC_layer& source){
    for (int i=0; i<source.getNumFilters(); i++){
        outs << "\tFC Filter["<<i<<"]"<<std::endl;
        outs << source.getFilter(i) << std::endl;
        outs << "\tFC Bias["<<i<<"]"<<std::endl;
        outs << source.getBias(i)<<std::endl<<std::endl;
    }
    return outs;
}

FC_layer::FC_layer(int depth, int height, int width, int numFilter){
    this->inputHeight=height;
    this->inputWidth=width;
    this->inputDepth=depth;
    this->numFilters=numFilter;

    this->filterHeight=height;
    this->filterWidth=width;
    this->filterDepth=depth;
    inputVolumn = Matrix3D (depth,height,width);

    this->outputHeight = 1;
    this->outputWidth = 1;
    this->outputDepth = numFilter;
    outputVolumn = Matrix3D (numFilters,1,1);

    Matrix3D fil (depth,height,width);
    for (size_t i= 0; i<numFilter;i++){
        filter_storage.push_back(fil);
        dJdW.push_back(fil);
        bias_storage.push_back(0);
        dJdB.push_back(0);

    }
    
    _randomizeFilter();
    _iniBias();
    std::cout<<"***FC*** inputHeight: "<<inputHeight<<"\t"<<"inputWidth: "<<inputWidth<<"\t"<<"inputDepth: "<<inputDepth<<std::endl;
    std::cout<<"***FC*** outputHeight: "<<outputHeight<<"\t"<<"outputWidth: "<<outputWidth<<"\t"<<"outputDepth: "<<outputDepth<<std::endl;
    std::cout<< "**** FC_LAYER IS CONSTRUCTED ***"<<std::endl;
}

FC_layer::FC_layer(int depth, int height, int width, std::istream& ins){
    this->inputDepth=depth;
    this->inputHeight=height;
    this->inputWidth=width;
    

    ins >> this->filterDepth;
    ins >> this->filterHeight;
    ins >> this->filterWidth;
    ins >> this->numFilters;
    assert (depth == filterDepth && height == filterHeight && width == filterWidth );

    inputVolumn = Matrix3D (depth,height,width);

    this->outputHeight = 1;
    this->outputWidth = 1;
    this->outputDepth = numFilters;
    outputVolumn = Matrix3D (numFilters,1,1);

    Matrix3D fil (depth,height,width);
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
    
    for (int i=0; i<numFilters; i++){
        std::cout << "FC filter ["<<i<<"]"<<std::endl;
        filter_storage[i].printMatrix();
        std::cout << "FC bias ["<<i<<"]"<<std::endl;
        std::cout << bias_storage[i] << std::endl;
    }
    std::cout<<"***FC*** inputHeight: "<<inputHeight<<"\t"<<"inputWidth: "<<inputWidth<<"\t"<<"inputDepth: "<<inputDepth<<std::endl;
    std::cout<<"***FC*** outputHeight: "<<outputHeight<<"\t"<<"outputWidth: "<<outputWidth<<"\t"<<"outputDepth: "<<outputDepth<<std::endl;
    std::cout<< "**** FC_LAYER IS CONSTRUCTED ***"<<std::endl;
}

FC_layer::FC_layer(const char * inputfile,
        int input_Height,int input_Width,
        int input_Depth,int num_Filters){
    this->inputHeight=input_Height;
    this->inputWidth=input_Width;
    this->inputDepth=input_Depth;

    this->numFilters=num_Filters;
    setInputVolumn(inputfile);
    Matrix3D fil (input_Depth,input_Height,input_Width);
    for (size_t i= 0; i<num_Filters;i++){
        filter_storage.push_back(fil);
    }
    //Need to Insert FILTER
    _randomizeFilter();
    _iniBias();
}

void FC_layer::_randomizeFilter()
{
    for (int filter=0;filter<numFilters;filter++){
        for (int depth=0;depth<inputDepth;depth++){
            for (int height=0;height<filterHeight;height++){
                for (int width=0;width<filterWidth;width++){
                    filter_storage[filter].setElement(depth,height,width,(std::rand()/float(RAND_MAX) *2)/100);//(std::rand()/float(RAND_MAX) *2)/10)
                    //std::cout<<filter_storage[filter].getElement(depth,height,width)<<" ";
                }
            }
        }
    }
}
/*
{
    
    std::cout<< "input volumn height: " <<inputVolumn.getHeight()<< " input volumn width: " << inputVolumn.getWidth() << " input volumn depth: " << inputVolumn.getHeight()<< std::endl;
    for (int filter=0;filter<numFilters;filter++){
        //std::cout<<"this is the 1st FOR loop: "<<std::endl;
        for (int depth=0;depth<inputDepth;depth++){
            //std::cout<<"this is the 2nd FOR loop: "<<std::endl;
            for (int height=0;height<filterHeight;height++){
                //std::cout<<"this is the third FOR loop: "<<std::endl;
                for (int width=0;width<filterWidth;width++){
                    //std::cout<< "depth: " <<depth<< " height: " << height << " width: " << width << std::endl;
                    filter_storage[filter].setElement(depth,height,width,2);//set element = 2. ((std::rand()/float(RAND_MAX) *2)/10);//randNum(-1,1);
                    std::cout<<filter_storage[filter].getElement(depth,height,width)<<" "; //"this is filter: "<<
                }
                std::cout<<std::endl;
            }
            std::cout<<std::endl;
        }
        std::cout<<std::endl;
    }
}*/
void FC_layer::_iniBias(){
    for (int filter=0; filter<numFilters;filter++){
        //bias_storage [filter]= 0.0;
        bias_storage.push_back(0);
    }
}

void FC_layer::ForwardPropergate(){
    float temp;
    //std::cout<< "FCLAYER  OUTPUT VOlme:  height: "<<outputVolumn.getHeight()<<" width: "<<outputVolumn.getWidth()<<" depth: "<<outputVolumn.getDepth()<<std::endl;
    //std::cout<< " numFIlters: "<< numFilters <<std::endl;
    //std::cout << " OutputVolumn.Matrix size: "<< inputVolumn.matrix[0].size() <<std::endl;
    for (int filter=0; filter<numFilters; filter++){
        temp = _specialDot(filter) + bias_storage[filter];
        outputVolumn.setElement(filter,0,0,temp);//,height,width,depth);
        //std::cout<<Convolution(i,j,filter_index)<<' ';
        //std::cout<<outputVolumn.getElement(filter,0,0)<<std::endl;//filter_index<<i<<j<<' '<<
        //std::cout<<temp<<std::endl;//filter_index<<i<<j<<' '<<       _specialDot(filter)<<" + "<< bias_storage[filter]<<" = "<< 
    }
    //std::cout<<std::endl;
    std::cout<<"***** FC OUTPUT START: ******"<<std::endl;
    outputVolumn.printMatrix();
    std::cout<<"***** FC OUTPUT END: ******"<<std::endl;
}



void FC_layer::BackPropergate(const Matrix3D& delta){
    //assume inputDelta == WeightedDelta
    /*
    std::cout<< "delta depth: "<<delta.getDepth()<<" delta height: "<<delta.getHeight()<<" delta width: "<<delta.getWidth()<<std::endl;
    std::cout<< "outputDepth depth: "<<outputDepth<<std::endl;
    */
    assert (delta.getDepth() == outputDepth && delta.getHeight() == 1 && delta.getWidth() == 1);
    
    this->delta = delta;
    for (int i=0; i< delta.getDepth(); i++){
        dJdB[i] = this->delta.getElement(i,0,0);
    }

    std::cout<< "dJdW start "<<std::endl;
    for( int i=0;i<numFilters;i++){
        std::cout<< "NumFilter: "<<i<<std::endl;
        dJdW[i] = inputVolumn.multConst(this->delta.getElement(i,0,0));
        dJdW[i].printMatrix();
    }
    std::cout<< "dJdW end "<<std::endl;
    
    //std::cout<<"filter before update: "<<std::endl;
    //printFilter();
    updateFilterBias();
    //std::cout<<"filter after update: "<<std::endl;
    //printFilter();
    std::cout<<"INPUT FC VOLUME: "<<std::endl;
    inputVolumn.printMatrix();
    std::cout<<"OUTPUT FC VOLUME: "<<std::endl;
    outputVolumn.printMatrix();
    std::cout<<"DELTA VOLUME: "<<std::endl;
    delta.printMatrix();
}



float FC_layer::_specialDot(int filter){//, int row, int col, int depth){
    /*assert(inputHeight != filterHeight);
    assert(inputWidth != filterWidth);
    assert(inputDepth != filterDepth);*/
    float value;
        value = 0;
        for(int depth=0;depth<inputDepth;depth++){
            for(int height=0;height<inputHeight;height++){
                for(int width=0;width<inputWidth;width++){
                    value += inputVolumn.getElement(depth,height,width) * filter_storage[filter].getElement(depth,height,width);
                    //std::cout<<inputVolumn.getElement(depth,height,width)<<" * "<< filter_storage[filter].getElement(depth,height,width)<<" = "<< value<<std::endl;
                }
            }
        }
    return value;
}

void FC_layer::setInputVolumn(const char * inputfile){
    std::ifstream in_input_file(inputfile, std::ios::in);
    Matrix3D temp(inputfile, inputDepth, inputHeight, inputWidth);
    inputVolumn = temp;
}

/*
void FC_layer::setInputVolumn(Matrix3D &arg){
    assert( arg.getHeight() !=0 && arg.getWidth() !=0 && arg.getDepth() !=0);
    inputVolumn = arg;
}
*/

Matrix3D FC_layer::WeightedDelta() const {
    float sum;
    Matrix3D temp (filterDepth,filterHeight,filterWidth);
    for (int i=0; i<filterDepth ;i++){
        for (int j=0; j<filterHeight ;j++){
            for (int k=0; k<filterWidth ;k++){
                sum = 0;
                for (int t=0; t< numFilters; t++){
                    sum += delta.getElement(t,0,0) * filter_storage[t].getElement(i,j,k);
                }
                temp.setElement(i,j,k,sum);
            }
        }
    }
    return temp;
}

void FC_layer::updateFilterBias(){
    for (int i=0; i<numFilters; i++){
        filter_storage[i] = filter_storage[i] - dJdW[i].multConst(learningRate);
        bias_storage[i] = bias_storage[i] - dJdB[i]*learningRate;
    }
}

void FC_layer::printFilter(){
    for (int i=0;i<numFilters; i++){
        filter_storage[i].printMatrix();
    }
}