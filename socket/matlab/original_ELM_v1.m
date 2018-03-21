tic;
clear all;
load('file');
%load('oldtrain');
%load('testall');
 n_hid=5000;
 i=0;
 for j=1:10:500
     i=i+1;
     t(i)=i;
     % elm_train('oldtrain',0,n_hid,'hardlim'); % [r(j,1) r(j,2)] ` TrainingTime,TrainingAccuracy
      [~,ACC(i),VARR(i)] = elm_train('file',0,j,'hardlim'); % [r(j,1) r(j,2)] ` TrainingTime,TrainingAccuracy
      %elm_predict('testall');    % [r(j,3) r(j,4)] ~ TestingTime, TestingAccuracy
 end 
 plot(t,ACC,'-b',t,VARR,'-r');
toc


