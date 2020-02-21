// readMNIST.cc
// read MNIST data into double vector, OpenCV Mat, or Armadillo mat
// free to use this code for any purpose
// author : Eric Yuan 
// my blog: http://eric-yuan.me/
// part of this code is stolen from http://compvisionlab.wordpress.com/

//#include "opencv2/core/core.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/highgui/highgui.hpp"
//#include <armadillo>
#include <vector>
#include <math.h>
#include <iostream>
#include <fstream>

//using namespace cv;
using namespace std;
//using namespace arma;

int ReverseInt (int i)
{
    unsigned char ch1, ch2, ch3, ch4;
    ch1 = i & 255;
    ch2 = (i >> 8) & 255;
    ch3 = (i >> 16) & 255;
    ch4 = (i >> 24) & 255;
    return((int) ch1 << 24) + ((int)ch2 << 16) + ((int)ch3 << 8) + ch4;
}

void read_Mnist(string filename, vector<vector<double> > &vec)
{
    ifstream file (filename, ios::binary);
    if (file.is_open())
    {
        int magic_number = 0;
        int number_of_images = 0;
        int n_rows = 0;
        int n_cols = 0;
        file.read((char*) &magic_number, sizeof(magic_number));
        magic_number = ReverseInt(magic_number);
        file.read((char*) &number_of_images,sizeof(number_of_images));
        number_of_images = ReverseInt(number_of_images);
        file.read((char*) &n_rows, sizeof(n_rows));
        n_rows = ReverseInt(n_rows);
        file.read((char*) &n_cols, sizeof(n_cols));
        n_cols = ReverseInt(n_cols);
        for(int i = 0; i < number_of_images; ++i)
        {
            vector<double> tp;
            for(int r = 0; r < n_rows; ++r)
            {
                for(int c = 0; c < n_cols; ++c)
                {
                    unsigned char temp = 0;
                    file.read((char*) &temp, sizeof(temp));
                    tp.push_back((double)temp);
                }
            }
            vec.push_back(tp);
        }
    }
}

void read_Mnist_Label(string filename, vector<double> &vec)
{
    ifstream file (filename, ios::binary);
    if (file.is_open())
    {
        int magic_number = 0;
        int number_of_images = 0;
        int n_rows = 0;
        int n_cols = 0;
        file.read((char*) &magic_number, sizeof(magic_number));
        magic_number = ReverseInt(magic_number);
        file.read((char*) &number_of_images,sizeof(number_of_images));
        number_of_images = ReverseInt(number_of_images);
        
        for(int i = 0; i < number_of_images; ++i)
        {
            //cout<<i<<endl;
            
            unsigned char temp = 0;
            file.read((char*) &temp, sizeof(temp));
            vec.push_back((double)temp);
            
        }
    }
}

/*
int main()
{
    string filename = "train-images.idx3-ubyte";
    int number_of_images = 60000;
    int image_size = 28 * 28;

    //read MNIST iamge into double vector
    vector<vector<double>> vec;
    read_Mnist(filename, vec);
    cout<<"vec.size(): "<<vec.size()<<endl;
    cout<<"vec[0].size(): "<<vec[0].size()<<endl;
    int t= 4;
    int count =0;
    for (int i=0;i<28;i++){
        for (int j=0;j<28;j++){
            printf("%03.0f ",vec[t][count]);//cout<<vec[0][count]<<" ";
            count++;
        }
        cout<<endl;
    }
    
    string filename_label = "train-labels.idx1-ubyte";
    int number_of_images_label = 0;

    //vector<double> dec;
    //read MNIST label into double vector
    vector<double> dec;//number_of_images_label
    read_Mnist_Label(filename_label, dec);
    cout<<"dec.size(): "<<dec.size()<<endl;
    cout<<"dec["<<t<<"]: "<<dec[t]<<endl;
    return 0;
}
*/