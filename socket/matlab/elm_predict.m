function [ x, y] = elm_predict(test_data)

% Usage: elm_predict(TestingData_File)
% OR:    [TestingTime, TestingAccuracy] = elm_predict(TestingData_File)
%
% Input:
% TestingData_File      - Filename of testing data set
%
% Output: 
% TestingTime           - Time (seconds) spent on predicting ALL testing data
% TestingAccuracy       - Testing accuracy: 
%                           RMSE for regression or correct classification rate for classification
%
% MULTI-CLASSE CLASSIFICATION: NUMBER OF OUTPUT NEURONS WILL BE AUTOMATICALLY SET EQUAL TO NUMBER OF CLASSES
% FOR EXAMPLE, if there are 7 classes in all, there will have 7 output
% neurons; neuron 5 has the highest output means input belongs to 5-th class
%
% Sample1 regression: [TestingTime, TestingAccuracy] = elm_predict('sinc_test')
% Sample2 classification: elm_predict('diabetes_test')
%
    %%%%    Authors:    MR QIN-YU ZHU AND DR GUANG-BIN HUANG
    %%%%    NANYANG TECHNOLOGICAL UNIVERSITY, SINGAPORE
    %%%%    EMAIL:      EGBHUANG@NTU.EDU.SG; GBHUANG@IEEE.ORG
    %%%%    WEBSITE:    http://www.ntu.edu.sg/eee/icis/cv/egbhuang.htm
    %%%%    DATE:       APRIL 2004

%%%%%%%%%%% Macro definition
REGRESSION=0;
CLASSIFIER=1;

%%%%%%%%%%% Load testing dataset
%test_data=load(TestingData_File);
TV.T=test_data(:,1:2)';
TV.P=test_data(:,3:size(test_data,2))';
clear test_data;                                    %   Release raw testing data array

NumberofTestingData=size(TV.P,2);

load wifielm_model.mat;

if Elm_Type~=REGRESSION

    %%%%%%%%%% Processing the targets of testing
    temp_TV_T=zeros(NumberofOutputNeurons, NumberofTestingData);
    for i = 1:NumberofTestingData
        for j = 1:size(label,2)
            if label(1,j) == TV.T(1,i)
                break; 
            end
        end
        temp_TV_T(j,i)=1;
    end
    TV.T=temp_TV_T*2-1;

end                                                 %   end if of Elm_Type

%%%%%%%%%%% Calculate the output of testing input
tic
tempH_test= wifiInputWeight*TV.P;
clear TV.P;             %   Release input of testing data             
ind=ones(1,NumberofTestingData);
BiasMatrix= wifiBiasofHiddenNeurons(:,ind);              %   Extend the bias matrix  wifiBiasofHiddenNeurons to match the demention of H
tempH_test=tempH_test + BiasMatrix;
switch lower( wifiActivationFunction)
    case {'sig','sigmoid'}
        %%%%%%%% Sigmoid 
        H_test = 1 ./ (1 + exp(-tempH_test));
    case {'sin','sine'}
        %%%%%%%% Sine
        H_test = sin(tempH_test);        
    case {'hardlim'}
        %%%%%%%% Hard Limit
        H_test = hardlim(tempH_test);        
        %%%%%%%% More activation functions can be added here        
end
TY=(H_test' * wifiOutputWeight)';                       %   TY: the actual output of the testing data
TestingTime=toc;           %   Calculate CPU time (seconds) spent by ELM predicting the whole testing data

if Elm_Type == REGRESSION
    TestingAccuracy=mean(sqrt(sum((abs(TV.T') - abs(TY')).^2,2)))  ;                    %   Calculate testing accuracy (RMSE) for regression case
    outputtyt=size(TV.T,1);
    outputtytl=size(TV.T,2);
    outputty=size(TY,1);
    outputtyl=size(TY,2);
    output=sum(TY')/10  ;
   
    x=TY(1);
    y=TY(2);
    
end

save('elm_output','output');