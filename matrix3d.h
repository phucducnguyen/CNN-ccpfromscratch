#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#ifndef MATRIX_3D_
#define MATRIX_3D_

//typedef double mat_value_type;
class Matrix3D
{
	public:
		typedef float mat_value_type;
		typedef std::size_t mat_size_type;
		Matrix3D();
		Matrix3D(mat_size_type Depth, mat_size_type Height,mat_size_type Width); // constructor
		Matrix3D(std::string, mat_size_type Depth, mat_size_type Height,mat_size_type Width); // constructor that reads from file
		Matrix3D(const Matrix3D &arg); // copy constructor
		
		void setElement (mat_size_type depth, mat_size_type heigth,mat_size_type width, mat_value_type val);
		//void setElement (std::string);
		void setMatrix (std::vector <std::vector <std::vector <mat_value_type> > > arg){this->matrix = arg;};
		void setStruct(mat_size_type depth,mat_size_type height,mat_size_type width){nHeight = height;nWidth = width;nDepth = depth;}

		mat_value_type getElement(mat_size_type depth, mat_size_type height,mat_size_type width) const;
		mat_size_type getHeight()const{return nHeight;};
		mat_size_type getWidth()const{return nWidth;};
		mat_size_type getDepth()const{return nDepth;};

		Matrix3D operator- (const Matrix3D & arg);
		Matrix3D elementMult (const Matrix3D & arg)const;
		Matrix3D transpose()const;
		Matrix3D multConst(float)const;
		float vectorNorm();

		//Matrix3D operator= (const Matrix3D& arg); // assignment operator
		void printMatrix(std::string file);
		void printMatrix(void)const;
		void printfMatrix() const;
		//~Matrix3D(); // Destructor

	private:
		mat_size_type nHeight;
		mat_size_type nWidth;
        mat_size_type nDepth;
		std::vector <std::vector <std::vector <mat_value_type> > > matrix; //pointer to pointer to multidim array
};
std::ostream& operator<< (std::ostream& outs,const Matrix3D& source){
	for (Matrix3D::mat_size_type i = 0; i < source.getDepth(); i++)
	{
		for (Matrix3D::mat_size_type j = 0; j < source.getHeight(); j++)
		{
            for (Matrix3D::mat_size_type k =0;k<source.getWidth();k++)
            {
                outs << source.getElement(i,j,k) << " ";
            }
			outs << std::endl;
		}
		outs << std::endl;
	}
	return outs;
}

Matrix3D::Matrix3D()
{
	nHeight = 0;
	nWidth = 0;
	nDepth = 0;
	matrix.empty();
}
Matrix3D::Matrix3D(mat_size_type Depth, mat_size_type Height,mat_size_type Width)
{
	// constructor
	nHeight = Height;
	nWidth = Width;
	nDepth = Depth;
	// allocate memory for nHeight
	for(int i = 0; i < nDepth; i++){
    	std::vector < std::vector < mat_value_type > > h;
    	matrix.push_back( h );
    	for(int j = 0; j < nHeight; j++){
      		std::vector <mat_value_type> w;
      		matrix[i].push_back( w );
      		for(int k = 0; k < nWidth; k++){
        		matrix[i][j].push_back(0);
      		}
    	}
	}
	//printMatrix();
}

Matrix3D::Matrix3D (std::string input_file_name,mat_size_type Depth, mat_size_type Height,mat_size_type Width)
{
	// TODO : constructor that reads array from input file
	std::ifstream input_file (input_file_name, std::ios::in);
	nHeight = Height;
	nWidth = Width;
	nDepth = Depth;
	float temp;
	for(int i = 0; i < nDepth; i++){
    	std::vector < std::vector < mat_value_type > > h;
    	matrix.push_back( h );
    	for(int j = 0; j < nHeight; j++){
      		std::vector <mat_value_type> w;
      		matrix[i].push_back( w );
      		for(int k = 0; k < nWidth; k++){
				input_file >> temp;
        		matrix[i][j].push_back(temp);// populate each element from input file
      		}
    	}
	}      
}

Matrix3D::Matrix3D(const Matrix3D &arg)
{
	nHeight = arg.nHeight;
	nWidth = arg.nWidth;
	nDepth = arg.nDepth;
	// allocate memory for nHeight
	this->matrix = arg.matrix;
}


void Matrix3D::setElement (mat_size_type depth, mat_size_type height,mat_size_type width, mat_value_type val)
{
	try
	{
		if (height >= nHeight || width > nWidth || depth > nDepth)
		{
			throw 11;
		}
	}
	catch(int errorNum)
	{
		std::cout << "FATAL ERROR #" << errorNum << " - setElement failed. Check inputs." << std::endl;
		std::cout << "SUGGESTION: the size of this matrix is: " << this->nHeight << " by " << this->nWidth << std::endl;
	}
	assert(height < this->nHeight && width < this->nWidth && depth< this->nDepth);
	matrix[depth][height][width] = val;

}

Matrix3D::mat_value_type Matrix3D::getElement(mat_size_type depth,mat_size_type height, mat_size_type width) const
{
	assert(height < this->nHeight);
	assert(width < this->nWidth);
	assert(depth < this->nDepth);
	mat_value_type element;

	if (height < nHeight && width < nWidth && depth <nDepth)
	{
		element = matrix[depth][height][width];
	}
	else
	{
		std::cout << "check inputs" << std::endl;
	}

	return element;
}
/*
void Matrix3D::setInput(std::string input_file_name)
{
	std::ifstream input_file (input_file_name, std::ios::in);

	matrix = new mat_value_type **[nHeight];
	for (mat_size_type i = 0; i < nHeight; i++)
	{
		// each pointer element in nheight, will point to a new column
		matrix[i] = new mat_value_type *[nWidth];
		for (mat_size_type j = 0; j < nWidth; j++)
		{
            matrix[i][j] = new mat_value_type [nDepth];
            for (mat_size_type k = 0; k<nDepth; k++)
            {
                input_file >> matrix[i][j][k]; // populate each element from input file
            }
		}
	}
}
*/
Matrix3D Matrix3D::operator- (const Matrix3D & arg){
	assert(arg.nDepth == this->nDepth && arg.nHeight == this->nHeight && arg.nWidth == this->nWidth);

	Matrix3D temp(arg.nDepth,arg.nHeight,arg.nWidth);
	for (int i=0; i<nDepth; i++){
		for (int j=0; j<nHeight; j++){
			for (int k=0; k<nWidth; k++){
				temp.setElement(i,j,k, this->matrix[i][j][k] - arg.matrix[i][j][k]); //getElement(i,j,k) );
			}
		}
	}
	return temp;
}

Matrix3D Matrix3D::elementMult (const Matrix3D & arg)const{
	assert(arg.nHeight == nHeight && arg.nWidth == nWidth && arg.nDepth == nDepth);

	Matrix3D element(nDepth,nHeight,nWidth);
	for (int i=0; i<nDepth; i++){
		for(int j=0; j<nHeight; j++){
			for(int k=0;k<nWidth; k++){
				element.setElement(i,j,k, this->matrix[i][j][k] * arg.matrix[i][j][k]);
			}
		}
	}
	return element;
}

Matrix3D Matrix3D::multConst(float constant)const{
	Matrix3D temp(nDepth,nHeight,nWidth);
	for (int i=0; i<nDepth; i++){
		for(int j=0; j<nHeight; j++){
			for(int k=0;k<nWidth; k++){
				temp.setElement(i,j,k,this->matrix[i][j][k] * constant);
			}
		}
	}
	//temp.printMatrix();
	return temp;
}

float Matrix3D::vectorNorm(){
	assert ( nHeight == 1 && nWidth == 1 && nDepth >0);
	float norm =0.0;
	for (int i=0; i< nDepth; i++){
		norm += matrix[i][0][0]*matrix[i][0][0];
	}
	norm = sqrt(norm);
	return norm;
}


/*
Matrix3D Matrix3D::operator = (const Matrix3D &arg)
{
	this->nHeight = arg.nHeight;
	this->nWidth = arg.nWidth;
	this->nDepth = arg.nDepth;
	this->matrix = arg.matrix;
	/*
	matrix = new mat_value_type **[this->nHeight];
	for (mat_size_type i = 0; i < this->nHeight; i++)
	{
		// each pointer element in nRow, will point to a new column
		matrix[i] = new mat_value_type *[this->nWidth];
		for (mat_size_type j = 0; j < this->nWidth; j++)
		{
			matrix[i][j] = new mat_value_type [this->nDepth];
			for (mat_size_type k = 0; k < this->nDepth; k++)
			{
				matrix[i][j][k] = arg.matrix[i][j][k]; // populate each element with 0
			}
		}
	}
	
	return *this;
}
*/
void Matrix3D::printMatrix(std::string file)
{
	std::ofstream outputFile(file, std::ios::out);

	for (Matrix3D::mat_size_type i = 0; i < nHeight; i++)
	{
		for (Matrix3D::mat_size_type j = 0; j < nWidth; j++)
		{
            for (mat_size_type k =0;k<nDepth;k++)
            {
                outputFile << matrix[i][j][k] << " ";
            }
			outputFile << std::endl;
		}
		outputFile << std::endl;
	}
	outputFile<<std::endl;
}

void Matrix3D::printMatrix() const
{
	//std::cout<< "MATRIX  nHeight: "<<nHeight<<" nWidth: "<<nWidth<<" nDepth: "<<nDepth<<std::endl;
	for (mat_size_type i = 0; i < nDepth; i++)
	{
		for (mat_size_type j = 0; j < nHeight; j++)
		{
            for (mat_size_type k =0;k<nWidth;k++)
            {
				//std::cout<< "MATRIX print MATRIX method i: "<<i<<" j: "<<j<<" k: "<<k<<std::endl;
                std::cout << matrix[i][j][k] << " ";
				//printf("%01.0f ",matrix[i][j][k]);
            }
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
	//std::cout<<std::endl;
}

void Matrix3D::printfMatrix() const
{
	//std::cout<< "MATRIX  nHeight: "<<nHeight<<" nWidth: "<<nWidth<<" nDepth: "<<nDepth<<std::endl;
	for (mat_size_type i = 0; i < nDepth; i++)
	{
		for (mat_size_type j = 0; j < nHeight; j++)
		{
            for (mat_size_type k =0;k<nWidth;k++)
            {
				//std::cout<< "MATRIX print MATRIX method i: "<<i<<" j: "<<j<<" k: "<<k<<std::endl;
                //std::cout << matrix[i][j][k] << " ";
				printf("%01.2f ",matrix[i][j][k]);
            }
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
	//std::cout<<std::endl;
}

/*
Matrix3D::~Matrix3D()
{
	// deallocate all memory used
    for (mat_size_type i=0;i<nHeight;i++){
        for (mat_size_type j=0;j<nWidth;j++){
            delete [] matrix[i][j];
        }
        delete matrix[i];
    }
    delete [] matrix;
}
*/


#endif
