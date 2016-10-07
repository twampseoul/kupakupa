function [ pk ] = gammatotext( p )
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here

k= 0.1;

text=zeros(p,2);

for i = 1:p
    hasil = randg(k,100000,2);
    pk=mean(abs(hasil(:,1)-hasil(:,2)));
    text(i,1)=k;
    text(i,2)=pk;
    k = k+0.1;
end
text
% fileID = fopen('pk.txt','w');
% fprintf(fileID,'%3.2f %4.15f\n',text);
% fclose(fileID);


type pk.txt
end
