% run in octave/matlab

points1=[[-4.000000,-1.000000];
		[-6.000000,-4.000000];
		[3.000000,1.000000];
		[1.000000,-2.000000];
		[5.000000,-4.000000];
		[0.000000,7.000000];
		[5.000000,5.000000];
		[-5.000000,5.000000];
		[-2.000000,3.000000]];

vertices1=[[8,7,0,8];
		[3,0,1,3];
		[6,2,4,6];
		[5,2,6,5];
		[4,3,1,4];
		[3,2,8,3];
		[4,2,3,4];
		[5,7,8,5];
		[7,1,0,7];
		[8,0,3,8];
		[8,2,5,8]];

points2=[[-1.000000,-1.000000];
		[-2.000000,-3.000000];
		[4.000000,-2.000000];
		[3.000000,4.000000];
		[1.000000,1.000000];
		[-2.000000,3.000000];
		[-2.000000,2.000000]];

vertices2=[[6,0,4,6];
		[2,0,1,2];
		[5,4,3,5];
		[4,0,2,4];
		[4,2,3,4];
		[6,4,5,6];
		[6,1,0,6]];

% octave only?
set(0,'defaultlinelinewidth',2);
figure 
hold on
axis equal
set(0,'defaultlinelinewidth',2);
for i=1:size(vertices1,1)
	plot(points1(vertices1(i,:)+1,1),points1(vertices1(i,:)+1,2),'o');
end
pause
for i=1:size(vertices1,1)
	plot(points1(vertices1(i,:)+1,1),points1(vertices1(i,:)+1,2));
end
pause
clf
hold on
axis equal
for i=1:size(vertices2,1)
	plot(points2(vertices2(i,:)+1,1),points2(vertices2(i,:)+1,2),'o');
end
pause
for i=1:size(vertices2,1)
	plot(points2(vertices2(i,:)+1,1),points2(vertices2(i,:)+1,2));
end
pause
close all
