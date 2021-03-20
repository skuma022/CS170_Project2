#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream> 
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <ctime>

using namespace std;

struct dataVector{
    int dataClassifier; //classifies as 1 or 2
    vector<double> data;    //organizes data into vector
};
struct dataSet{ 
  vector<dataVector> lines; 
};

dataSet getData(string filename){ //get data from data dataSet file
    dataSet mainSet; 
    dataVector v; 
    ifstream file; 
    string temp; 
    double num;
    file.open(filename.c_str());
    if(file.is_open()){
        while(getline(file, temp)){
            v.data.clear();
            stringstream ss;
            ss << temp;
            ss >> num;
            v.dataClassifier = num;
            while(ss >> num){ 
              v.data.push_back(num); 
            }
            mainSet.lines.push_back(v);   
        }
    }
    else{
        cout << "Failed to open file\n";
        exit(EXIT_FAILURE);
    }
    file.close();
    return mainSet;
}
int countFeature(dataSet s){
    int i;
    for(i = 0; i < s.lines.at(0).data.size(); i++) {}
    return i;
}
int countInstance(dataSet s){
    int i;
    for(i = 0; i < s.lines.size(); i++) {}
    return i;
}
double getDistance(dataVector V1, dataVector V2, vector<int>featureNumbers){ //using distance formula
    double temp = 0, s = 0;
    for(int i = 0; i < featureNumbers.size(); i++){
        temp = pow(V1.data.at(featureNumbers.at(i)) - V2.data.at(featureNumbers.at(i)), 2);
        s += temp;
    }
    return sqrt(s);
}
double kFold(dataSet s, vector<int> features, int & bestNumWrong){
    double numberCorrect = 0.0, numberIncorrect = 0.0, closestFeature = 99999.0; 
    dataVector closest;
    for(int i = 0; i < s.lines.size(); i++){
        dataVector v = s.lines.at(i);   //data index left out
        for(int j = 0; j < i; j++){
            if(getDistance(v, s.lines.at(j), features) < closestFeature){
                closestFeature = getDistance(v, s.lines.at(j), features);
                closest = s.lines.at(j);
            }
        }
        for(int j = i + 1; j < s.lines.size(); j++){
            if(getDistance(v, s.lines.at(j), features) < closestFeature){
                closestFeature = getDistance(v, s.lines.at(j), features);
                closest = s.lines.at(j);
            }
        }
        if(v.dataClassifier == closest.dataClassifier){ 
          numberCorrect++; 
        }
        else numberIncorrect++;
        if(numberIncorrect > bestNumWrong) 
          return 0;
        closestFeature = 99999;
    }
    bestNumWrong = countInstance(s) - numberCorrect;
    return numberCorrect/countInstance(s) * 100;    //calculate perecentage
}
double kFold(dataSet s, vector<int> features){  //from slides
    double numberCorrect = 0, closestFeature = 99999; dataVector closest;
    for(int i = 0; i < s.lines.size(); i++){
        dataVector v = s.lines.at(i);   //data index left out
        for(int j = 0; j < i; j++){
            if(getDistance(v, s.lines.at(j), features) < closestFeature){
                closestFeature = getDistance(v, s.lines.at(j), features);
                closest = s.lines.at(j);
            }
        }
        for(int j = i + 1; j < s.lines.size(); j++){
            if(getDistance(v, s.lines.at(j), features) < closestFeature){
                closestFeature = getDistance(v, s.lines.at(j), features);
                closest = s.lines.at(j);
            }
        }
        if(v.dataClassifier == closest.dataClassifier){ numberCorrect++; }
        closestFeature = 99999;
    }
    return numberCorrect/countInstance(s) * 100;    //calculate percentage
}
string printSubset(vector<int> v){ //adds strings to organize subset
    string misc = "{";
    for(int j = v.size() - 1; j >= 0; j--){
        misc += to_string(v.at(j) + 1);
        if(j != 0) misc += ",";
    }
    return misc + "}";
}

void forwardSelection(dataSet s, int k_fold){
    vector<int> vFeatureList; 
    double bestpreviousAccuracy = 0.01, previousAccuracy = 0.0, tempFeature = 0.0, prevAccuracy; 
    int bestNumWrong = 999;

    cout << "Beginning search.\n";
    while(previousAccuracy < bestpreviousAccuracy){
        previousAccuracy = bestpreviousAccuracy;
        for(int i = 0; i < s.lines.at(0).data.size(); i++){
            for(int j = 0; j < vFeatureList.size(); j++)
            if(vFeatureList.at(j) == i) goto skip;   //skips features that have already been used
            vFeatureList.push_back(i);
            if(k_fold == 1) prevAccuracy = kFold(s, vFeatureList);
            else               prevAccuracy = kFold(s, vFeatureList, bestNumWrong);
            cout << "\tUsing feature(s) " << printSubset(vFeatureList) << " accuracy is " << prevAccuracy << "%\n";   
            if(prevAccuracy > bestpreviousAccuracy){
                bestpreviousAccuracy = prevAccuracy;
                tempFeature = i;
            }         
            vFeatureList.pop_back();
            skip: ;
        }
        if(previousAccuracy == bestpreviousAccuracy) break;
        vFeatureList.push_back(tempFeature);
        cout << "Feature dataSet " << printSubset(vFeatureList) << " was best, accuracy is " << bestpreviousAccuracy << "%\n";
    }
    cout << "Finished search! The best feature subdataSet is " << printSubset(vFeatureList) << ", which has an accuracy of " << bestpreviousAccuracy << "%\n";
}
void backwardElimination(dataSet s){
    vector<int> vFeatureList, tFeatureList; double bestpreviousAccuracy = 0.01, previousAccuracy = 0, tempFeature = 0, prevAccuracy;
    cout << "Beginning search.\n";
    for(int i = 0; i < s.lines.at(0).data.size(); i++)
        vFeatureList.push_back(i);
    while(previousAccuracy < bestpreviousAccuracy){
        previousAccuracy = bestpreviousAccuracy;
        for(int i = 0; i < vFeatureList.size(); i++){
            tFeatureList = vFeatureList;
            tFeatureList.erase(tFeatureList.begin() + i);
            prevAccuracy = kFold(s, tFeatureList);
            cout << "\tUsing feature(s) " << printSubset(tFeatureList) << " accuracy is " << prevAccuracy << "%\n";   
            if(prevAccuracy > bestpreviousAccuracy){
                bestpreviousAccuracy = prevAccuracy;
                tempFeature = i;
            }
        }
        if(previousAccuracy == bestpreviousAccuracy) break;
        vFeatureList.erase(vFeatureList.begin() + tempFeature);
        cout << "Feature dataSet " << printSubset(vFeatureList) << " was best, accuracy is " << bestpreviousAccuracy << "%\n";
    }
    cout << "Finished search! The best feature subdataSet is " << printSubset(vFeatureList) << ", which has an accuracy of " << bestpreviousAccuracy << "%\n";
}
int main(){
    string filename; 
    int userInput = 0; 
    dataSet fileData; 
    vector<int> temp;
    cout << "Welcome to Surya Kumaraguru's Feature Selection Algorithm.\nType in the name of the file to test: ";
    cin >> filename;
    fileData = getData(filename);
    for(int i = 0; i < countFeature(fileData); i++){
       temp.push_back(i); 
       }
    cout << "This datadataSet has " << countFeature(fileData) << " features (not including the class attribute), with " << countInstance(fileData) << " instances.\n";
    cout << "Running nearest neighbor with all " << countFeature(fileData) << " features, using \"leaving-one-out\" evaluation, I get an previousAccuracy of " << kFold(fileData, temp) << "%\n";
    algorithmSelect: cout << "Type the number of the algorithm you want to run:\n1) Forward Selection\n2) Backward Elimination\n3) Forward Selection with k-old evaluation\n";
    cin >> userInput;
    if(userInput==1){ 
      forwardSelection(fileData, 1); 
      }
    else if(userInput==2){ 
      backwardElimination(fileData); 
      }
    else if(userInput==3){ 
      forwardSelection(fileData, 2); 
      }  
    else{
      goto algorithmSelect;
      }
}
