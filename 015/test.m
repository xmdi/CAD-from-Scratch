% run in octave/matlab

tri1=[[0,0,0];[3,0,0];[3,3,0];[0,0,0]];
tri2=[[0,2,-1];[0,1,1];[8,1,0.5];[0,2,-1]];
tri3=[[2,1,0];[1,-2,0];[2,-2,0];[2,1,0]];
tri4=[[1,2,0];[4,2,0];[1,-1,0];[1,2,0]];

tri1_tri2_intersect=[[1.454545,1.454545,0];[3,1.406250,0]];
tri1_tri3_intersect=[[1.666667,0,0];[2,1,0];[2,0,0];[1.666667,0,0]];
tri1_tri4_intersect=[[3,1,0];[3,2,0];[2,2,0];[1,1,0];[1,0,0];[2,0,0];[3,1,0]];

figure 
hold on
axis equal

% octave only?
set(0,'defaultlinelinewidth',2);

plot3(tri1(:,1),tri1(:,2),tri1(:,3));
plot3(tri2(:,1),tri2(:,2),tri2(:,3));
plot3(tri1_tri2_intersect(:,1),tri1_tri2_intersect(:,2),tri1_tri2_intersect(:,3));

pause
clf; hold on

plot3(tri1(:,1),tri1(:,2),tri1(:,3));
plot3(tri3(:,1),tri3(:,2),tri3(:,3));
plot3(tri1_tri3_intersect(:,1),tri1_tri3_intersect(:,2),tri1_tri3_intersect(:,3));

pause
clf; hold on

plot3(tri1(:,1),tri1(:,2),tri1(:,3));
plot3(tri4(:,1),tri4(:,2),tri4(:,3));
plot3(tri1_tri4_intersect(:,1),tri1_tri4_intersect(:,2),tri1_tri4_intersect(:,3));

pause

close all
