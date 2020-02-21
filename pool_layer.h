//#include "layer.h"
#include <cmath>
#include <stdlib.h>
#include "layer.h"
#include <utility>

struct Position{
    int row;
    int col;
    void print(){
        std::cout<< "row:col "<<row<<":"<<col;
        };
};

class Pool_layer : public layer{// : public Layer{ 
    public:
    Pool_layer(){}
    Pool_layer(int depth, int height, int width,int stride,int spatialExtent);
    Pool_layer(int depth, int height, int width, std::istream& ins);
    Pool_layer(const char * inputfile,//Matrix3D input
        int inputHeight,int inputWidth,
        int inputDepth,int stride,
        int spatialExtent);
    std::pair<float,Position> Pooling(int filter,int indexH,int indexW);
    void setInputVolumn(const char * inputfile);//,int height,int width,int depth);
    void ForwardPropergate();
    void BackPropergate(const Matrix3D& nextLayerDelta);

    int getStride(){return stride;};
    int getSpatialExtent(){return spatial_extent;}

    private:
    int spatial_extent;
    int stride;
    std::vector< std::vector< std::vector< Position > > > positions;
}; 

Pool_layer::Pool_layer(int depth, int height, int width,int stride, int spatialExtent){

    this->inputHeight = height;
    this->inputWidth = width;
    this->inputDepth = depth;

    this->outputHeight = 1+(inputHeight-spatialExtent)/stride;
    this->outputWidth = 1+(inputWidth-spatialExtent)/stride;
    this->outputDepth = depth;

    this->spatial_extent = spatialExtent;
    this->stride = stride;
    inputVolumn = Matrix3D (inputDepth,inputHeight,inputWidth);
    outputVolumn = Matrix3D(outputDepth,outputHeight,outputWidth);
    // struct position

    Position t;
    for(int i = 0; i < outputDepth; i++){
    	std::vector < std::vector < Position > > h;
    	positions.push_back( h );
    	for(int j = 0; j < outputHeight; j++){
      		std::vector <Position> w;
      		positions[i].push_back( w );
      		for(int k = 0; k < outputWidth; k++){
        		positions[i][j].push_back(t);
      		}
    	}
	}
    //position = outputVolumn;
    std::cout<<"***POOL*** outputHeight: "<<outputHeight<<"\t"<<"outputWidth: "<<outputWidth<<"\t"<<"outputDepth: "<<outputDepth<<std::endl;
    std::cout<< "**** POOL_LAYER IS CONSTRUCTED ***"<<std::endl;
}

Pool_layer::Pool_layer(int depth, int height, int width, std::istream& ins){
    this->inputHeight = height;
    this->inputWidth = width;
    this->inputDepth = depth;

    ins >> this->spatial_extent;
    ins >> this->stride;
    std::cout <<"spatial extent: "<<spatial_extent<<" stride: "<<stride<<std::endl;

    this->outputHeight = 1+(inputHeight-spatial_extent)/stride;
    this->outputWidth = 1+(inputWidth-spatial_extent)/stride;
    this->outputDepth = depth;

    inputVolumn = Matrix3D (inputDepth,inputHeight,inputWidth);
    outputVolumn = Matrix3D(outputDepth,outputHeight,outputWidth);
    // struct position

    Position t;
    for(int i = 0; i < outputDepth; i++){
    	std::vector < std::vector < Position > > h;
    	positions.push_back( h );
    	for(int j = 0; j < outputHeight; j++){
      		std::vector <Position> w;
      		positions[i].push_back( w );
      		for(int k = 0; k < outputWidth; k++){
        		positions[i][j].push_back(t);
      		}
    	}
	}
    //position = outputVolumn;
    std::cout<<"***POOL*** outputHeight: "<<outputHeight<<"\t"<<"outputWidth: "<<outputWidth<<"\t"<<"outputDepth: "<<outputDepth<<std::endl;
    std::cout<< "**** POOL_LAYER IS CONSTRUCTED ***"<<std::endl;
}

Pool_layer::Pool_layer(const char * inputfile,
        int input_Height,int input_Width,
        int input_Depth,int stride,
        int spatialExtent){
    this->inputHeight=input_Height;
    this->inputWidth=input_Width;
    this->inputDepth=input_Depth;
    this->stride=stride;
    this->spatial_extent=spatialExtent;
    setInputVolumn(inputfile);//,inputHeight,inputWidth,inputDepth);
}

void Pool_layer::ForwardPropergate(){
    assert((inputHeight - spatial_extent)%stride == 0);
    assert((inputWidth - spatial_extent)%stride == 0);

    //perform pooling
    std::pair<float,Position> pool; 
    Matrix3D temp(inputDepth , 1+(inputHeight-spatial_extent)/stride , 1+(inputWidth-spatial_extent)/stride);
    for (int numLayer = 0; numLayer < inputDepth; numLayer++){
        for (int i=0; i < 1+(inputHeight-spatial_extent)/stride; i++){
            for (int j=0; j < 1+(inputWidth-spatial_extent)/stride; j++){
                pool = Pooling(numLayer,i,j);
                temp.setElement(numLayer,i,j,pool.first);//Pooling(numLayer,i,j).first
                positions[numLayer][i][j] = pool.second; // Pooling(numLayer,i,j).second
                //std::cout<<Convolution(i,j,filter_index)<<' ';
                //std::cout<<temp.getElement(numLayer,i,j)<<' ';//numLayer<<i<<j<<' '<<
            }
        }
    }

    outputVolumn = temp;
    std::cout<<"***** POOL OUTPUT START: ******"<<std::endl;
    outputVolumn.printMatrix();
    std::cout<<"***** POOL OUTPUT END: ******"<<std::endl;
    std::cout<<"***** POOL POSITION OUTPUT START: ******"<<std::endl;
    for(int i=0;i<positions.size();i++){
        for(int j=0;j<positions[0].size();j++){
            for(int k=0;k<positions[0][0].size();k++){
                positions[i][j][k].print();
                std::cout<<" ";
            }
            std::cout<<std::endl;
        }
        std::cout<<std::endl;
    }
    std::cout<<"***** POOL POSITION OUTPUT END: ******"<<std::endl;
}
//MAX POOLING
std::pair<float,Position> Pool_layer::Pooling(int layer,int indexH,int indexW){
    std::pair<float,Position> store;
    Position pos;
    float max= -INFINITY;
    for(int j=indexH*stride; j<indexH*stride+spatial_extent;j++){
        for(int k=indexW*stride;k<indexW*stride+spatial_extent;k++){
            //std::cout<<j<<' '<<k<<std::endl;
            if (inputVolumn.getElement(layer,j,k)>max){
                max=inputVolumn.getElement(layer,j,k);
                pos.row= j ; 
                pos.col= k ;
            }
        }
    }
    store.first =max;
    store.second = pos;
    return store;
}

void Pool_layer::setInputVolumn(const char * inputfile){
    Matrix3D temp(inputfile,inputDepth,inputHeight,inputWidth);
    inputVolumn = temp;
    inputVolumn.printMatrix();
}

void Pool_layer::BackPropergate(const Matrix3D& nextLayerDelta){
    /*
    std::cout << "Height: "<<nextLayerDelta.getHeight() <<" "<< outputHeight << std::endl;
    std::cout << "Width: "<<nextLayerDelta.getWidth() <<" "<< outputWidth << std::endl;
    std::cout << "Depth: "<<nextLayerDelta.getDepth() <<" "<< outputDepth << std::endl;
    */
    assert(nextLayerDelta.getHeight() == outputHeight && nextLayerDelta.getWidth() == outputWidth && nextLayerDelta.getDepth() == outputDepth);

    Matrix3D temp (inputDepth, inputHeight,inputWidth);
    int tempRow;
    int tempCol;
    for(int i=0;i<positions.size();i++){
        for(int j=0;j<positions[0].size();j++){
            for(int k=0;k<positions[0][0].size();k++){
                tempRow =positions[i][j][k].row;
                tempCol = positions[i][j][k].col;
                temp.setElement(i,tempRow,tempCol,temp.getElement(i,tempRow,tempCol) + nextLayerDelta.getElement(i,j,k));
            }
        }
    }
    std::cout<<"***** POOL NextLayerDelta Start: ******"<<std::endl;
    nextLayerDelta.printMatrix();
    std::cout<<"***** POOL NextLayerDelta End: ******"<<std::endl;
    std::cout<<"***** POOL INPUT Start: ******"<<std::endl;
    inputVolumn.printMatrix();
    std::cout<<"***** POOL INPUT End: ******"<<std::endl;
    delta = temp;
    std::cout<<"***** POOL Delta Start: ******"<<std::endl;
    delta.printMatrix();
    std::cout<<"***** POOL Delta End: ******"<<std::endl;
}