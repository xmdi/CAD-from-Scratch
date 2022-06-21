figure
hold on
grid on

%Triangle intersection occurs between (1.000000,0.750000,1.000000) & (1.000000,0.750000,0.750000)
%for face1= (1.000000,1.000000,1.000000)->(1.000000,0.000000,1.000000)->(1.000000,0.000000,0.000000)
%for face2= (1.750000,0.750000,1.750000)->(0.750000,0.750000,0.750000)->(0.750000,0.750000,1.750000)


% first 2 faces intersecting
plot3([1 1 1 1],[1 0 0 1],[1 1 0 1],'-r');
plot3([1.75 .75 .75 1.75],[.75 .75 .75 .75],[1.75 .75 1.75 1.75],'-k');
plot3([1 1],[.75 .75],[1 .75],'-g');

% second 2 faces intersecting
%plot3([1 1 1 1],[1 0 0 1],[1 1 0 1],'-r');
%plot3([.75 1.75 1.75 .75],[.75 .75 .75 .75],[.75 1.75 .75 .75],'-k');
%plot3([1 1],[.75 .75],[1 .75],'-g');

pause
