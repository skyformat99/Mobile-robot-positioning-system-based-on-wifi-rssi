function [TrainingTime,TrainingAccuracy,vvar] = elm_train(TrainingData_File, Elm_Type, NumberofHiddenNeurons, wifiActivationFunction)

% Usage: elm_train(TrainingData_File, Elm_Type, NumberofHiddenNeurons, ActivationFunction)
% OR:    [TrainingTime, TrainingAccuracy] = elm_train(TrainingData_File, Elm_Type, NumberofHiddenNeurons, ActivationFunction)
%
% Input:
% TrainingData_File     - Filename of training data set
% Elm_Type              - 0 for regression; 1 for (both binary and multi-classes) classification
% NumberofHiddenNeurons - Number of hidden neurons assigned to the ELM
% ActivationFunction    - Type of activation function:
%                           'sig' for Sigmoidal function
%                           'sin' for Sine function
%                           'hardlim' for Hardlim function
%
% Output: 
% TrainingTime          - Time (seconds) spent on training ELM
% TrainingAccuracy      - Training accuracy: 
%                           RMSE for regression or correct classification rate for classification
%
% MULTI-CLASSE CLASSIFICATION: NUMBER OF OUTPUT NEURONS WILL BE AUTOMATICALLY SET EQUAL TO NUMBER OF CLASSES
% FOR EXAMPLE, if there are 7 classes in all, there will have 7 output
% neurons; neuron 5 has the highest output means input belongs to 5-th class
%
% Sample1 regression: [TrainingTime, TrainingAccuracy, TestingAccuracy] = elm_train('sinc_train', 0, 20, 'sig')
% Sample2 classification: elm_train('diabetes_train', 1, 20, 'sig')
%
    %%%%    Authors:    MR QIN-YU ZHU AND DR GUANG-BIN HUANG
    %%%%    NANYANG TECHNOLOGICAL UNIVERSITY, SINGAPORE
    %%%%    EMAIL:      EGBHUANG@NTU.EDU.SG; GBHUANG@IEEE.ORG
    %%%%    WEBSITE:    http://www.ntu.edu.sg/eee/icis/cv/egbhuang.htm
    %%%%    DATE:       APRIL 2004

%%%%%%%%%%% Macro definition
REGRESSION=0;
CLASSIFIER=1;

%%%%%%%%%%% Load training dataset
train_data=load(TrainingData_File);
T=train_data(:,1:2)';
P=train_data(:,3:size(train_data,2))';
clear train_data;                                   %   Release raw training data array

NumberofTrainingData=size(P,2);
NumberofInputNeurons=size(P,1);

% if Elm_Type~=REGRESSION
%     %%%%%%%%%%%% Preprocessing the data of classification
%     sorted_target=sort(T,2);
%     label=zeros(1,1);                               %   Find and save in 'label' class label from training and testing data sets
%     label(1,1)=sorted_target(1,1);
%     j=1;
%     for i = 2:NumberofTrainingData
%         if sorted_target(1,i) ~= label(1,j)
%             j=j+1;
%             label(1,j) = sorted_target(1,i);
%         end
%     end
%     number_class=j;
%     NumberofOutputNeurons=number_class;
%     
%     %%%%%%%%%% Processing the targets of training
%     temp_T=zeros(NumberofOutputNeurons, NumberofTrainingData);
%     for i = 1:NumberofTrainingData
%         for j = 1:number_class
%             if label(1,j) == T(1,i)
%                 break; 
%             end
%         end
%         temp_T(j,i)=1;
%     end
%     T=temp_T*2-1;
% end                                                 %   end if of Elm_Type

%%%%%%%%%%% Calculate weights & biases
tic

%%%%%%%%%%% Random generate input weights InputWeight (w_i) and biases BiasofHiddenNeurons (b_i) of hidden neurons
wifiInputWeight=rand(NumberofHiddenNeurons,NumberofInputNeurons)*2-1;   %[-1,1]
wifiBiasofHiddenNeurons=rand(NumberofHiddenNeurons,1);
tempH=wifiInputWeight*P;
clear P;                                            %   Release input of training data 
ind=ones(1,NumberofTrainingData);
BiasMatrix=wifiBiasofHiddenNeurons(:,ind);              %   Extend the bias matrix wifiBiasofHiddenNeurons to match the demention of H
tempH=tempH+BiasMatrix;

%%%%%%%%%%% Calculate hidden neuron output matrix H
switch lower(wifiActivationFunction)  %Lee:What is the effect of this function?
    case {'sig','sigmoid'}
        %%%%%%%% Sigmoid 
        H = 1 ./ (1 + exp(-tempH));
    case {'sin','sine'}
        %%%%%%%% Sine
        H = sin(tempH);    
    case {'hardlim'}
        %%%%%%%% Hard Limit
        H = hardlim(tempH);            
        %%%%%%%% More activation functions can be added here                
end
clear tempH;                                        %   Release the temparary array for calculation of hidden neuron output matrix H

%%%%%%%%%%% Calculate output weights OutputWeight (beta_i)
wifiOutputWeight=pinv(H') * T';
TrainingTime=toc      %   Calculate time (seconds) spent for training ELM

%%%%%%%%%%% Calculate the training accuracy
Y=(H' * wifiOutputWeight)';                             %   Y: the actual output of the training data
if Elm_Type == REGRESSION
    TrainingAccuracy=mean(sqrt(sum((abs(T') - abs(Y')).^2,2)))                   %   Calculate training accuracy (RMSE) for regression case
    %sqrt(sum((abs(T') - abs(Y')).^2,2))    实际物理坐标矩阵减去数据坐标矩阵求得 误差矩阵。
    %sum函数按照第二维的顺序求和，得到误差向量，最后求取所有误差向量的均值。
    vvar=std(sqrt(sum((abs(T') - abs(Y')).^2,2)))
    output=Y;    
end
clear H;

% if Elm_Type == CLASSIFIER
% %%%%%%%%%% Calculate training & testing classification accuracy
%     MissClassificationRate_Training=0;
% 
%     for i = 1 : size(T, 2)
%         [x, label_index_expected]=max(T(:,i));
%         [x, label_index_actual]=max(Y(:,i));
%         output(i)=label(label_index_actual);
%         if label_index_actual~=label_index_expected
%             MissClassificationRate_Training=MissClassificationRate_Training+1;
%         end
%     end
%     TrainingAccuracy=1-MissClassificationRate_Training/NumberofTrainingData
% end

if Elm_Type~=REGRESSION
    save('wifielm_model', 'NumberofInputNeurons', 'NumberofOutputNeurons', 'wifiInputWeight', 'wifiBiasofHiddenNeurons', 'wifiOutputWeight', 'wifiActivationFunction', 'label', 'Elm_Type');
else
    save('wifielm_model', 'wifiInputWeight', 'wifiBiasofHiddenNeurons', 'wifiOutputWeight', 'wifiActivationFunction', 'Elm_Type');    
end