%3D plot of data
clc
clear all
close all

%fid = fopen('HMC5883SensorTest_deliminated3.30.13.txt');
fid = fopen('Test3Magnetometer4.4.13.txt');
%fid = fopen('5.10.13.NorthCTLMEXIT.txt');
%fid = fopen('5.10.13.SouthCTLMExit.txt');
% data = textscan(fid,'%*s%f%f%f','Delimiter',',<>')
% fclose(fid);

data = textscan(fid,'%f%f%f','Delimiter',' ')
fclose(fid);
datasize = size(data{1,3});

f = 1:datasize;
f = f';
% for j=1:datasize;
%     c{j,1} = j;
% end

data{4} = f;
% Create a 3d scatter plot
c= [1,2,3];
c2 = [3,4,5];
% x,y,time plot
 scatter3(cell2mat(data(1)),cell2mat(data(4)),cell2mat(data(2)));
hold on
 scatter3(cell2mat(data(1)),cell2mat(data(4)),cell2mat(data(3)));
  hold on
plot3(cell2mat(data(1)),cell2mat(data(4)),cell2mat(data(2)));
hold on
plot3(cell2mat(data(1)),cell2mat(data(4)),cell2mat(data(3)));
hold off

% plot3(cell2mat(data(1)),cell2mat(data(4)),cell2mat(data(2)));
% x,z,time plot

% surf(cell2mat(data(1)),cell2mat(data(2)),cell2mat(data(3)));

box on;

% Set the viewing angle and the axis limits

% tline = fgetl(fid);
% 
% CX = [];
% CY = [];
% CZ = [];
% 
% while ischar(tline)
%     % skip < and >
%    % tline = substr(tline, 1, length(tline)-2)
% 
%     % extract numbers
%     temp = textscan(tline,'%f%f%f', 'delimiter',',');
% 
%     CX(end+1,:) = [temp(1)];
%     CY(end+1,:) = [temp(2)];
%     CZ(end+1,:) = [temp(3)];
% 
%     tline = fgetl(fid);
% end
% 
% fclose(fid);