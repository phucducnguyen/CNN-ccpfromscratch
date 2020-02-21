#include "cnn.h"
#include "string.h"
#include "takeInput.h"
#include <chrono>
#include <ctime>
#include <iomanip>

class TrainerCNN{
    public:
    TrainerCNN(){};
    TrainerCNN(CNN& A);//, std::string const dataTraining, std::string const dataTesting);
    TrainerCNN(CNN& A,std::string parameterFile, std::string logFile, std::string average_costFile);
    void Training(std::vector <Matrix3D> trainingData, std::vector <Matrix3D> trainingLabels, int epoch, float learningRate);
    void MnistTraining2(int epoch, int numberOfImageSamples,float learningRate);
    void MnistTraining10(int epoch, int numberOfImageSamples,float learningRate);
    void InputFileTrain(std::string inputfile, int epoch, int numberOfImageSamples,float learningRate);
    
    void normalized();
    float Testing(std::vector <Matrix3D> testingData, std::vector <Matrix3D> testingLabels);
    float MnistTesting2();
    float MnistTesting10();
    void ImageIDTesting(std::string inputfile, std::string logFile, int ImageID);
    int getMaxOutPosition(Matrix3D output);

    private:
    CNN model; //(int conv,int pool,int fc,int inputDepth,int inputHeight,int inputWidth, int numOfOutput)
    //read_Mnist_Label
    //read_Mnist
    int numberOfImage;
    int imageSize; // = 28*28
    std::vector <std::vector <double>> dataTrain;
    std::vector <double> dataTest;
    //Matrix3D trainingInputData;
    //Matrix3D expectedOutput;
    std::string log_file_name = "CNN_training_log.txt";
    std::string parameter_file_name = "CNN_parameter.txt";
    std::string average_cost_file = "CNN_average_cost.txt";
};

TrainerCNN::TrainerCNN(CNN& A){//, std::string const dataTraining, std::string const dataTesting){
    this->model = A;
}
TrainerCNN::TrainerCNN(CNN& A,std::string parameterFile, std::string logFile, std::string average_costFile){//, std::string const dataTraining, std::string const dataTesting){
    this->model = A;
    this ->log_file_name = logFile;
    this ->parameter_file_name = parameterFile;
    this ->average_cost_file = average_costFile;
}
int TrainerCNN::getMaxOutPosition(Matrix3D output){
    assert (output.getDepth() >0 && output.getHeight()==1 && output.getWidth()==1);
    int maxPosition =0;
    float max = output.getElement(0,0,0);
    for (int i=0; i<output.getDepth(); i++){
        if (max < output.getElement(i,0,0)){
            max = output.getElement(i,0,0);
            maxPosition = i;
        }
    }
    return maxPosition;
}


void TrainerCNN::Training(std::vector <Matrix3D> trainingData, std::vector <Matrix3D> trainingLabels, int epoch,float learningRate = 0.002){
    assert (trainingData.size() == trainingLabels.size() && epoch >0);
    std::ofstream logFile(log_file_name, std::ios::out);
    std::ofstream averageCostFile(average_cost_file, std::ios::out);
    
    float cost_sum;
    float cost_average;
    model.setLearningRate(learningRate);
    auto start = std::chrono::system_clock::now();
    std::time_t start_time = std::chrono::system_clock::to_time_t(start);
    logFile << "Training start time: " <<std::ctime(&start_time)<<std::endl;
    logFile << "Number Of Training Image Samples: "<<trainingData.size() << " Epoch: " << epoch << " Learning Rate: " << learningRate<< std::endl;
    logFile << "INPUT -> [[CONV -> RELU]*N -> POOL]*M -> [FC -> RELU]*K -> FC"<<std::endl;
    logFile << "N= "<<model.getConv() <<" M= "<< model.getPool() <<" K= "<< model.getFC() <<std::endl;
    logFile << "Image Depth: "<<trainingData[0].getDepth() <<" Image Height: "<<trainingData[0].getHeight()<<" Image Width: "<<trainingData[0].getWidth()<<std::endl;
    logFile << "Number Of Outputs: "<<trainingLabels[0].getDepth()<<std::endl;
    logFile.close();

    for (int ep=0; ep<epoch; ep++){
        cost_sum = 0;
        std::cout<<"Epoch: "<<ep<<std::endl;
        model.logCNN(log_file_name,ep);
        for (int i=0; i<trainingData.size(); i++){
            //std::cout<<"trainingLabels depth: "<<trainingLabels[i].getDepth()<<" trainingLabels height: "<<trainingLabels[i].getHeight()<<" trainingLabels width: "<<trainingLabels[i].getWidth()<<std::endl;
            model.SetExpectedOutput(trainingLabels[i]);
            model.Forward(trainingData[i]);
            cost_sum += model.Cost();
            model.Back();
        }
        cost_average = cost_sum / trainingData.size();
        logFile = std::ofstream(log_file_name, std::ios::out | std::ios::app);
        logFile << "Average Cost: "<<cost_average<<std::endl;
        logFile.close();
        averageCostFile <<cost_average<<std::endl;
    }
    model.logCNN(log_file_name,epoch);
    logFile = std::ofstream(log_file_name, std::ios::out | std::ios::app);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    logFile << "Training finish time: " << std::ctime(&end_time) <<std::endl;
    logFile << "Training duration: "<< elapsed_seconds.count() << "s"<< std::endl;
    logFile.close();
    averageCostFile.close();
}

void TrainerCNN::MnistTraining2(int epoch, int numberOfImageSamples,float learningRate = 0.002){
    std::string dataTrainingImages = "train-images.idx3-ubyte";
    std::string dataTrainingLabels = "train-labels.idx1-ubyte";

    read_Mnist(dataTrainingImages, dataTrain); //vector< vector<double> >
    read_Mnist_Label(dataTrainingLabels, dataTest); // vector <double>

    Matrix3D expectedOutput (2,1,1);
    std::vector <Matrix3D> trainingLabels;
    Matrix3D trainingInputData(1,28,28);
    std::vector <Matrix3D> trainingInputMatrices;

    //for(int image=0; image<numberOfImageSamples; image++){
    for (int image = 0; image < 60000 && trainingInputMatrices.size() < numberOfImageSamples; image ++){
        if (dataTest[image] == 1 || dataTest[image] ==0){
            //printf("Current Label = %f\n", dataTest[image]);
            expectedOutput.setElement(dataTest[image],0,0,1);
            trainingLabels.push_back(expectedOutput);
            expectedOutput.setElement(dataTest[image],0,0,0);
            int count = 0;
            for (int i=0; i<28;i++){
                for (int j=0; j<28; j++){
                    trainingInputData.setElement(0,i,j,(dataTrain[image][count])/255);
                    count++;
                }
            }
            trainingInputMatrices.push_back(trainingInputData);
            printf("Current Label = %f\t\tExpectedOutputMatrix = \n", dataTest[image]);
            trainingLabels[trainingLabels.size()-1].printMatrix() ;
            trainingInputMatrices[trainingInputMatrices.size()-1].printfMatrix();

        }
    }
    std::cout << " trainingInputMatrices size: "<<trainingInputMatrices.size()<<std::endl;

    Training(trainingInputMatrices, trainingLabels, epoch, learningRate);
    
}

void TrainerCNN::MnistTraining10(int epoch, int numberOfImageSamples,float learningRate = 0.002){
    std::string dataTrainingImages = "train-images.idx3-ubyte";
    std::string dataTrainingLabels = "train-labels.idx1-ubyte";

    read_Mnist(dataTrainingImages, dataTrain); //vector< vector<double> >
    read_Mnist_Label(dataTrainingLabels, dataTest); // vector <double>

    Matrix3D expectedOutput (10,1,1);
    std::vector <Matrix3D> trainingLabels;
    for(int i=0; i<numberOfImageSamples; i++){
        expectedOutput.setElement(dataTest[i],0,0,1);
        trainingLabels.push_back(expectedOutput);
        expectedOutput.setElement(dataTest[i],0,0,0);
    }

    Matrix3D trainingInputData(1,28,28);
    std::vector <Matrix3D> trainingInputMatrices;
    for (int image=0; image<numberOfImageSamples; image++){
        int count = 0;
        for (int i=0; i<28;i++){
            for (int j=0; j<28; j++){
                trainingInputData.setElement(0,i,j,(dataTrain[image][count])/255);
                count++;
            }
        }
        trainingInputMatrices.push_back(trainingInputData);
    }

    Training(trainingInputMatrices, trainingLabels, epoch, learningRate);
    
}

void TrainerCNN::InputFileTrain(std::string inputfile, int epoch, int numberOfImageSamples,float learningRate = 0.002){
    std::ifstream inFile(inputfile, std::ios::in);
    std::string temp;
    float number_of_images;
    int temp_int;

    Matrix3D expectedOutput (2,1,1);
    std::vector <Matrix3D> trainingLabels;
    Matrix3D trainingInputData(1,28,28);
    std::vector <Matrix3D> trainingInputMatrices;

    inFile >> temp;
    if (temp.compare("Number_Of_Images:") != 0){
        //std::cout << "ERROR: expected Number_Of_Images: but read " << temp<<std::endl;
        throw std::string("ERROR: expected Number_Of_Images: but read")+temp;
    }
    inFile >> number_of_images;
    if (number_of_images < numberOfImageSamples){
        inFile.close();
        throw std::string("ERROR: Number of Samples in input_struct_train.txt cannot be larger than Number of Images in Input_Train_Images.txt");
    }
    for (int i=0; i<numberOfImageSamples; i++){
        inFile >> temp;
        if (temp.compare("Image_ID:") != 0){
            //std::cout << "ERROR: expected Image_ID: but read " << temp<<std::endl;
            throw std::string("ERROR: expected Image_ID: but read ")+temp;
        }
        inFile >> temp;
        if (std::stoi(temp) != i){
            throw std::string("ERROR: Image_ID Out Of Order. EXPECTED: Image_ID: ")+std::to_string(i)+ std::string(" but read Image_ID: ")+temp;
        }
        inFile >> temp;
        if (temp.compare("Current_Label:") != 0){
            //std::cout << "ERROR: expected Current_Label: but read " << temp<<std::endl;
            throw std::string("ERROR: expected Current_Label: but read ")+temp;
        }
        inFile >> temp_int;
        if (temp_int == 1 || temp_int ==0){
            //printf("Current Label = %f\n", temp_int);
            expectedOutput.setElement(temp_int,0,0,1);
            //expectedOutput.printMatrix();
            trainingLabels.push_back(expectedOutput);
            expectedOutput.setElement(temp_int,0,0,0);
        }
        else{
            throw std::string("ERROR: Current_Label must be 0 or 1 but read ")+ std::to_string(temp_int);
        }
        for (int height=0; height<28; height++){
            for (int width=0; width<28; width++){
                inFile >> temp_int;
                trainingInputData.setElement(0,height,width, (float)temp_int/255.0 );
            }
        }
        trainingInputMatrices.push_back(trainingInputData);
        //trainingInputData.printfMatrix();
    }
    Training(trainingInputMatrices, trainingLabels, epoch, learningRate);
}
    
float TrainerCNN::Testing(std::vector <Matrix3D> testingData, std::vector <Matrix3D> testingLabels){
    assert (testingData.size() == testingLabels.size());
    int countCorrect=0;
    Matrix3D temp;
    //std::cout<<"trainingLabels depth: "<<trainingLabels[i].getDepth()<<" trainingLabels height: "<<trainingLabels[i].getHeight()<<" trainingLabels width: "<<trainingLabels[i].getWidth()<<std::endl;
    for (int i=0; i<testingData.size(); i++){
        model.Forward(testingData[i]);
        temp = model.getOuputVolumn();
        //testingLabels.printMatrix();
        if (testingLabels[i].getElement(getMaxOutPosition(temp),0,0)==1){
            countCorrect++;
        }
        /*
        if (getMaxOutPosition(temp) == getMaxOutPosition(testingLabels[i]){
            countCorrect++;
        }
        */
    }
    std::cout << "Number of Testing Images: "<<testingData.size()<<std::endl;
    std::cout << "Number of correct prediction: "<<countCorrect<<std::endl;
    std::cout << "Percentage % Correct: "<<((float) countCorrect/ (float)testingData.size())*100 <<"%"<<std::endl;
    return ((float) countCorrect/ (float)testingData.size())*100;
}

float TrainerCNN::MnistTesting2(){
    std::string dataTestingImages = "t10k-images.idx3-ubyte";
    std::string dataTestingLabels = "t10k-labels.idx1-ubyte";

    read_Mnist(dataTestingImages, dataTrain); //vector< vector<double> >
    read_Mnist_Label(dataTestingLabels, dataTest); // vector <double>

    //Matrix3D expectedOutput (10,1,1); 10 ouputs 0 -> 9
    Matrix3D expectedOutput (2,1,1); // 2 outputs 0 and 1
    std::vector <Matrix3D> testingLabels;
    
    Matrix3D testingInputData(1,28,28);
    std::vector <Matrix3D> testingInputMatrices;
    //for (int image=0; image<10000; image++){
    for (int image = 0; image < 10000 && testingInputMatrices.size() < 1000; image ++){
        if (dataTest[image] == 1 || dataTest[image] ==0){
            expectedOutput.setElement(dataTest[image],0,0,1);
            testingLabels.push_back(expectedOutput);
            expectedOutput.setElement(dataTest[image],0,0,0);
            int count = 0;
            for (int i=0; i<28;i++){
                for (int j=0; j<28; j++){
                    testingInputData.setElement(0,i,j, (dataTrain[image][count])/255);
                    count++;
                }
            }
            testingInputMatrices.push_back(testingInputData);
            std::cout << "testingInputMatrices size: "<<testingInputMatrices.size()<<std::endl;
            printf("Current Label = %f\t\tExpectedOutputMatrix = \n", dataTest[image]);
            testingLabels[testingLabels.size()-1].printMatrix() ;
            testingInputMatrices[testingInputMatrices.size()-1].printfMatrix();
        }
    }
    return Testing(testingInputMatrices,testingLabels);
}

float TrainerCNN::MnistTesting10(){
    std::string dataTestingImages = "t10k-images.idx3-ubyte";
    std::string dataTestingLabels = "t10k-labels.idx1-ubyte";

    read_Mnist(dataTestingImages, dataTrain); //vector< vector<double> >
    read_Mnist_Label(dataTestingLabels, dataTest); // vector <double>

    Matrix3D expectedOutput (10,1,1);
    std::vector <Matrix3D> testingLabels;
    for(int i=0; i<10000; i++){
        expectedOutput.setElement(dataTest[i],0,0,1);
        testingLabels.push_back(expectedOutput);
        expectedOutput.setElement(dataTest[i],0,0,0);
    }

    Matrix3D testingInputData(1,28,28);
    std::vector <Matrix3D> testingInputMatrices;
    for (int image=0; image<10000; image++){
        int count = 0;
        for (int i=0; i<28;i++){
            for (int j=0; j<28; j++){
                testingInputData.setElement(0,i,j,(dataTrain[image][count])/255);
                count++;
            }
        }
        testingInputMatrices.push_back(testingInputData);
    }
    return Testing(testingInputMatrices,testingLabels);
}

void TrainerCNN::ImageIDTesting(std::string inputfile, std::string logFile, int ImageID){
    std::ifstream inFile(inputfile, std::ios::in);
    std::ofstream outFile(logFile, std::ios::out);
    std::string temp;
    float number_of_images;
    int temp_int;
    std::vector <int> labels;

    Matrix3D expectedOutput (2,1,1); // 2 outputs 0 and 1
    std::vector <Matrix3D> testingLabels;
    
    Matrix3D testingInputData(1,28,28);
    std::vector <Matrix3D> testingInputMatrices;

    inFile >> temp;
    if (temp.compare("Number_Of_Images:") != 0){
        //std::cout << "ERROR: expected Number_Of_Images: but read " << temp<<std::endl;
        throw std::string("ERROR: expected Number_Of_Images: but read")+temp;
    }
    inFile >> number_of_images;
    if (number_of_images <= ImageID || ImageID < 0){
        inFile.close();
        throw std::string("ERROR: Invalid Image ID");
    }
    for (int i=0; i<number_of_images; i++){
        inFile >> temp;
        if (temp.compare("Image_ID:") != 0){
            //std::cout << "ERROR: expected Image_ID: but read " << temp<<std::endl;
            throw std::string("ERROR: expected Image_ID: but read ")+temp;
        }
        inFile >> temp;
        if (std::stoi(temp) != i){
            throw std::string("ERROR: Image_ID Out Of Order. EXPECTED: Image_ID: ")+std::to_string(i)+ std::string(" but read Image_ID: ")+temp;
        }
        inFile >> temp;
        if (temp.compare("Current_Label:") != 0){
            //std::cout << "ERROR: expected Current_Label: but read " << temp<<std::endl;
            throw std::string("ERROR: expected Current_Label: but read ")+temp;
        }
        inFile >> temp_int;
        if (temp_int == 1 || temp_int ==0){
            //printf("Current Label = %f\n", temp_int);
            expectedOutput.setElement(temp_int,0,0,1);
            //expectedOutput.printMatrix();
            testingLabels.push_back(expectedOutput);
            expectedOutput.setElement(temp_int,0,0,0);
            ////////////////////////////////////////////////////////////////////
            labels.push_back(temp_int);
        }
        else{
            throw std::string("ERROR: Current_Label must be 0 or 1 but read ")+ std::to_string(temp_int);
        }
        for (int height=0; height<28; height++){
            for (int width=0; width<28; width++){
                inFile >> temp_int;
                testingInputData.setElement(0,height,width, (float)temp_int/255.0 );
            }
        }
        testingInputMatrices.push_back(testingInputData);
        //testingInputData.printfMatrix();
    }

    model.Forward(testingInputMatrices[ImageID]);
    //testingLabels.printMatrix();
    float actualOutput = getMaxOutPosition(model.getOuputVolumn());
    //if (testingLabels[ImageID].getElement(actualOutput,0,0)==1){
    outFile << "Image ID: "<<ImageID<<std::endl;
    outFile << "Expected Output: " << labels[ImageID] << std::endl;
    outFile << "Actual Output: " << actualOutput <<"\t";
    if (labels[ImageID] == actualOutput){
        outFile << "(Correct Prediction)"<<std::endl;
    }
    else {
        outFile << "(Incorrect Prediction)"<<std::endl;
    }
    outFile << "Image Input:\n";
    for (int i=0; i<28;i++){
        for (int j=0; j<28; j++){
            outFile.width(3);
            //outFile.fill('0');
            outFile<<testingInputMatrices[ImageID].getElement(0,i,j) * 255<<" ";
            //printf("Current Label = %f\t\tExpectedOutputMatrix = \n", TestingLabels[image]);
        }
        //std::cout<<std::endl;
        outFile << std::endl;
    }
    outFile.close();
    
    //return Testing(ImageID_image,ImageID_label);
    //return Testing(testingInputMatrices,testingLabels);
}