clear;
clc;

FileData = load('1K/600-1200.mat');
csvwrite('1K/600-1200.csv', FileData.b);

FileData = load('2K/1200-2400.mat');
csvwrite('2K/1200-2400.csv', FileData.b);

FileData = load('4K/2400-5000.mat');
csvwrite('4K/2400-5000.csv', FileData.b);

FileData = load('8K/5000-10000.mat');
csvwrite('8K/5000-10000.csv', FileData.b);

FileData = load('16K/10000-20000.mat');
csvwrite('16K/10000-20000.csv', FileData.b);

FileData = load('32/20-40.mat');
csvwrite('32/20-40.csv', FileData.b);

FileData = load('64/40-80.mat');
csvwrite('64/40-80.csv', FileData.b);

FileData = load('125/80-160.mat');
csvwrite('125/80-160.csv', FileData.b);

FileData = load('250/160-300.mat');
csvwrite('250/160-300.csv', FileData.b);

FileData = load('500/300-600.mat');
csvwrite('500/300-600', FileData.b);