%2D plot of data from magenetometer test
%Roy Stillwell
% 4.21.13 
clc
clear all
close all

%fid = fopen('HMC5883SensorTest_deliminated3.30.13.txt');
% fid = fopen('4.5.13CTLM_top_enterance.txt');
fid = fopen('Test3Magnetometer4.4.13.txt');
%fid = fopen('5.10.13.NorthCTLMEXIT.txt');
%fid = fopen('5.10.13.SouthCTLMEXIT.txt');
% data = textscan(fid,'%*s%f%f%f','Delimiter',',<>')
% fclose(fid);
lag = 10;
dim = 1;

data = textscan(fid,'%f%f%f','Delimiter',' ')
fclose(fid);
datasize = size(data{1,3});

f = 1:datasize; % Builds an incremented array 1 to 'datasize' long
f = f'; %transpose the array to be x by 1 array

data{4} = f;
data{5} = tsmovavg(data{1}, 's', lag, dim);
% Create a 2d scatter plot
% c= [1,2,3];
% c2 = [3,4,5];
% 
% x = -pi:pi/10:pi;
% y = tan(sin(x)) - sin(tan(x));
% plot(x,y,'--rs','LineWidth',2,...
%                 'MarkerEdgeColor','k',...
%                 'MarkerFaceColor','g',...
%                 'MarkerSize',10)
% x,y,time plot
% scatter(cell2mat(data(1)),cell2mat(data(4))); %x scatter points
% hold on
% scatter(cell2mat(data(2)),cell2mat(data(4))); %y scatter points
%   hold on
%   scatter(cell2mat(data(3)),cell2mat(data(4))); %z scatter points
%   hold on

plot(cell2mat(data(4)),cell2mat(data(1)),'-rs'); % x plot
hold on
plot(cell2mat(data(4)),cell2mat(data(5)),'-b'); % x plot

% plot(cell2mat(data(4)),cell2mat(data(2)),'-bs'); % y plot
% hold on
% plot(cell2mat(data(4)),cell2mat(data(3)),'-gs'); % z plot
% hold off

% plot3(cell2mat(data(1)),cell2mat(data(4)),cell2mat(data(2)));
% x,z,time plot

% surf(cell2mat(data(1)),cell2mat(data(2)),cell2mat(data(3)));

box on;
