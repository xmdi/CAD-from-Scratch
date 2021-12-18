% run in octave/matlab

points0=[[-4.000000,-1.000000];
		[-6.000000,-4.000000];
		[3.000000,1.000000];
		[1.000000,-2.000000];
		[5.000000,-4.000000];
		[0.000000,7.000000];
		[5.000000,5.000000];
		[-5.000000,5.000000];
		[-2.000000,3.000000]];

vertices0=[[8,7,0,8];
		[3,0,1,3];
		[8,3,2,8];
		[6,2,4,6];
		[6,5,2,6];
		[4,3,1,4];
		[4,2,3,4];
		[8,5,7,8];
		[7,1,0,7];
		[8,0,3,8];
		[8,2,5,8]];

vertices1=[[8,7,0,8];
		[3,0,1,3];
		[8,3,2,8];
		[6,2,4,6];
		[6,5,2,6];
		[4,3,1,4];
		[4,2,3,4];
		[8,5,7,8];
		[7,1,0,7];
		[8,0,3,8];
		[8,2,5,8]];

vertices2=[[8,7,0,8];
		[3,0,1,3];
		[8,0,2,8];
		[6,2,4,6];
		[6,5,2,6];
		[4,3,1,4];
		[4,2,3,4];
		[8,5,7,8];
		[7,1,0,7];
		[2,0,3,2];
		[8,2,5,8]];

vertices3=[[8,7,0,8];
		[3,0,1,3];
		[5,3,2,5];
		[6,2,4,6];
		[6,5,2,6];
		[4,3,1,4];
		[4,2,3,4];
		[8,5,7,8];
		[7,1,0,7];
		[8,0,3,8];
		[8,3,5,8]];

vertices4=[[8,7,0,8];
		[3,2,1,3];
		[6,1,2,6];
		[6,2,4,6];
		[6,5,8,6];
		[4,3,1,4];
		[4,2,3,4];
		[8,5,7,8];
		[7,1,0,7];
		[6,0,1,6];
		[8,0,6,8]];

vertices5=[[3,7,0,3];
		[3,0,1,3];
		[8,7,4,8];
		[6,2,4,6];
		[6,5,2,6];
		[4,3,1,4];
		[4,2,8,4];
		[8,5,7,8];
		[7,1,0,7];
		[4,7,3,4];
		[8,2,5,8]];

vertices6=[[8,7,0,8];
		[3,0,1,3];
		[8,0,2,8];
		[6,2,4,6];
		[6,5,8,6];
		[4,3,1,4];
		[4,2,3,4];
		[8,5,7,8];
		[7,1,0,7];
		[2,0,3,2];
		[8,2,6,8]];

vertices7=[[8,7,0,8];
		[3,0,1,3];
		[6,0,2,6];
		[6,2,4,6];
		[6,5,7,6];
		[4,3,1,4];
		[4,2,3,4];
		[8,0,6,8];
		[7,1,0,7];
		[2,0,3,2];
		[7,8,6,7]];

vertices8=[[8,7,0,8];
		[3,2,1,3];
		[8,0,2,8];
		[6,2,4,6];
		[6,5,2,6];
		[4,3,1,4];
		[4,2,3,4];
		[8,5,7,8];
		[7,1,0,7];
		[2,0,1,2];
		[8,2,5,8]];


figure(1);hold on;axis equal;set(gca,'linewidth',2,'fontsize',20);
for i=1:size(vertices0,1)
	plot(points0(vertices0(i,:)+1,1),points0(vertices0(i,:)+1,2),'o','linewidth',3);
end
title('Point Cloud');
pause
for i=1:size(vertices0,1)
	plot(points0(vertices0(i,:)+1,1),points0(vertices0(i,:)+1,2),'linewidth',3);
end
title('Before Constraints');


pause
figure(2);hold on;axis equal;set(gca,'linewidth',2,'fontsize',20);
for i=1:size(vertices0,1)
	plot(points0(vertices1(i,:)+1,1),points0(vertices1(i,:)+1,2),'o','linewidth',3);
end
for i=1:size(vertices0,1)
	plot(points0(vertices1(i,:)+1,1),points0(vertices1(i,:)+1,2),'linewidth',3);
end
title({'Test Case 1','(constraint edge already exists)'});


pause
close(2);figure(2);hold on;axis equal;set(gca,'linewidth',2,'fontsize',20);
for i=1:size(vertices0,1)
	plot(points0(vertices2(i,:)+1,1),points0(vertices2(i,:)+1,2),'o','linewidth',3);
end
for i=1:size(vertices0,1)
	plot(points0(vertices2(i,:)+1,1),points0(vertices2(i,:)+1,2),'linewidth',3);
end
title({'Test Case 2','(constraint edge intersects 1 edge)'});


pause
close(2);figure(2);hold on;axis equal;set(gca,'linewidth',2,'fontsize',20);
for i=1:size(vertices0,1)
	plot(points0(vertices3(i,:)+1,1),points0(vertices3(i,:)+1,2),'o','linewidth',3);
end
for i=1:size(vertices0,1)
	plot(points0(vertices3(i,:)+1,1),points0(vertices3(i,:)+1,2),'linewidth',3);
end
title({'Test Case 3','(constraint edge intersects 1 edge)'});


pause
close(2);figure(2);hold on;axis equal;set(gca,'linewidth',2,'fontsize',20);
for i=1:size(vertices0,1)
	plot(points0(vertices4(i,:)+1,1),points0(vertices4(i,:)+1,2),'o','linewidth',3);
end
for i=1:size(vertices0,1)
	plot(points0(vertices4(i,:)+1,1),points0(vertices4(i,:)+1,2),'linewidth',3);
end
title({'Test Case 4','(constraint edge intersects multiple edges)'});


pause
close(2);figure(2);hold on;axis equal;set(gca,'linewidth',2,'fontsize',20);
for i=1:size(vertices0,1)
	plot(points0(vertices5(i,:)+1,1),points0(vertices5(i,:)+1,2),'o','linewidth',3);
end
for i=1:size(vertices0,1)
	plot(points0(vertices5(i,:)+1,1),points0(vertices5(i,:)+1,2),'linewidth',3);
end
title({'Test Case 5','(constraint edge intersects multiple edges)'});


pause
close(2);figure(2);hold on;axis equal;set(gca,'linewidth',2,'fontsize',20);
for i=1:size(vertices0,1)
	plot(points0(vertices6(i,:)+1,1),points0(vertices6(i,:)+1,2),'o','linewidth',3);
end
for i=1:size(vertices0,1)
	plot(points0(vertices6(i,:)+1,1),points0(vertices6(i,:)+1,2),'linewidth',3);
end
title({'Test Case 6','(2 constraint edges intersect single edges)'});


pause
close(2);figure(2);hold on;axis equal;set(gca,'linewidth',2,'fontsize',20);
for i=1:size(vertices0,1)
	plot(points0(vertices7(i,:)+1,1),points0(vertices7(i,:)+1,2),'o','linewidth',3);
end
for i=1:size(vertices0,1)
	plot(points0(vertices7(i,:)+1,1),points0(vertices7(i,:)+1,2),'linewidth',3);
end
title({'Test Case 7','(2 constraint edges intersecting many edges)'});


pause
close(2);figure(2);hold on;axis equal;set(gca,'linewidth',2,'fontsize',20);
for i=1:size(vertices0,1)
	plot(points0(vertices8(i,:)+1,1),points0(vertices8(i,:)+1,2),'o','linewidth',3);
end
for i=1:size(vertices0,1)
	plot(points0(vertices8(i,:)+1,1),points0(vertices8(i,:)+1,2),'linewidth',3);
end
title({'Test Case 8','(combination of existing and intersecting)'});


pause
close all
