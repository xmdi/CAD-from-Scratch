#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "geom.h"
#include "trig.h"
#include "draw.h"
#include "out.h"

void writeSTL_binary(struct BODY* body, char* *filename){
	FILE *f=fopen(*filename,"w");
	char header[80]={0};
	char attribute_byte_count[2]={0};
	int numTriangles = countFaces(body);
	fwrite(header,1,80,f);
	fwrite(&numTriangles,4,1,f);
	struct FACE* iter=body->face;
	while (iter!=NULL){	
		fwrite(iter->normal,4,3,f);
		for (int j=0; j<3; j++){
			float node[3]={iter->node_array[j]->x,iter->node_array[j]->y,iter->node_array[j]->z};
			fwrite(node,4,3,f);
		}
		fwrite(attribute_byte_count,1,2,f);
		iter=iter->next;
	}
	fclose(f);
	return;
}

void writeSTL_ASCII(struct BODY* body, char* *filename){
	FILE *f=fopen(*filename,"w");
	struct FACE* iter=body->face;
	fprintf(f,"solid \n");
	while (iter!=NULL){	
		fprintf(f,"facet normal %f %f %f\n",iter->normal[0],iter->normal[1],iter->normal[2]);
		fprintf(f,"\touter loop\n");
		for (int j=0; j<3; j++){
			fprintf(f,"\t\tvertex %f %f %f\n",iter->node_array[j]->x,iter->node_array[j]->y,iter->node_array[j]->z);
		}
		fprintf(f,"\tendloop\nendfacet\n");
		iter=iter->next;
	}
	fprintf(f,"endsolid \n");
	fclose(f);
	return;
}

void trimLeadingWhitespace(char* buffer){
	int count=0, k=0;
	while (isspace(buffer[count]))
		count++;
	for (int j=count; buffer[j]!='\0'; j++, k++)
		buffer[k]=buffer[j];
	buffer[k]='\0';
	return;
}

struct BODY* readSTL(char* *filename){
	struct BODY* body = (struct BODY*)malloc(sizeof(struct BODY));
	body->node=NULL;
	body->face=NULL;

	struct FACE* last_face = (struct FACE*)malloc(sizeof(struct FACE));

	FILE *f=fopen(*filename,"rb");
	char ft_buf[5];
	fread(&ft_buf,5,1,f);
	if (!memcmp(ft_buf,"solid",5)){
		//ASCII
		while(1){
			char line[999]={0};
			fgets(line,999,f);
			trimLeadingWhitespace(line);
			while (strncmp(line,"facet normal",12)){
				if (feof(f)) {
					fclose(f);
					printf("\rLoaded %s model: %d faces, %d nodes\n",*filename,countFaces(body),countNodes(body));
					return body;
				}
				fgets(line,999,f);
				trimLeadingWhitespace(line);
			} 
			struct FACE* face = (struct FACE*)malloc(sizeof(struct FACE*)+sizeof(float)*3+sizeof(int)+3*sizeof(struct NODE*));
			face->numNodes=0;
			sscanf(line,"%*s %*s %f %f %f\n",&face->normal[0],&face->normal[1],&face->normal[2]);
			while (strncmp(line,"outer loop",10)){
				fgets(line,999,f);
				trimLeadingWhitespace(line);
			}
			for (int j=0;j<3;j++){
				while (strncmp(line,"vertex",6)){
					fgets(line,999,f);
					trimLeadingWhitespace(line);
				}
				float coords[3];
				sscanf(line,"%*s %f %f %f\n",&coords[0],&coords[1],&coords[2]);
				struct NODE* iter=body->node;
				struct NODE* prev=iter;
				bool match_found=0;
				while(iter!=NULL){
					if ((iter->x==coords[0]) && (iter->y==coords[1]) && (iter->z==coords[2])){
						match_found=1;
						break;
					}
					prev=iter;
					iter=iter->next;
				}
				if (match_found){
					face->node_array[j]=iter;
					face->numNodes++;
				}
				else{
					struct NODE* node = (struct NODE*)malloc(sizeof(struct NODE));
					makeNode(node,coords[0],coords[1],coords[2],NULL);
					if (body->node==NULL){
						body->node=node;
					}
					else{
						prev->next=node;
					}
					face->node_array[j]=node;
					face->numNodes++;
				}
				fgets(line,999,f);
				trimLeadingWhitespace(line);
			}
			while (strncmp(line,"endloop",7)){
				fgets(line,999,f);
				trimLeadingWhitespace(line);
			} 
			while (strncmp(line,"endfacet",8)){
				fgets(line,999,f);
				trimLeadingWhitespace(line);
			} 
	
			if (body->face==NULL){
				body->face=face;
				last_face=face;
			}
			else{
				last_face->next=face;
				last_face=face;
			}
		}
	}
	else{
		//BINARY
		fseek(f,75,SEEK_CUR);
		int numTriangles;
		fread(&numTriangles,4,1,f);
		printf("Loading model:\n");
		for (int i=0;i<numTriangles;i++){
			if (!(i%(numTriangles/100))){
				printf("\rLoading %s model: %d/%d faces",*filename,i,numTriangles);
				fflush(stdout);
			}
			struct FACE* face = (struct FACE*)malloc(sizeof(struct FACE*)+sizeof(int)+3*sizeof(float)+3*sizeof(struct NODE*));
			face->numNodes=0;
			fread(face->normal,4,3,f);
			for (int j=0;j<3;j++){
				float coords[3];
				fread(coords,4,3,f);
			
				struct NODE* iter=body->node;
				struct NODE* prev=iter;
				bool match_found=0;
				while(iter!=NULL){
					if ((iter->x==coords[0]) && (iter->y==coords[1]) && (iter->z==coords[2])){
						match_found=1;
						break;
					}
					prev=iter;
					iter=iter->next;
				}
				if (match_found){
					face->node_array[j]=iter;
					face->numNodes++;
				}
				else{
					struct NODE* node = (struct NODE*)malloc(sizeof(struct NODE));
					makeNode(node,coords[0],coords[1],coords[2],NULL);
					if (body->node==NULL){
						body->node=node;
					}
					else{
						prev->next=node;
					}
					face->node_array[j]=node;
					face->numNodes++;
				}
			}
			if (body->face==NULL){
				body->face=face;
				last_face=face;
			}
			else{
				last_face->next=face;
				last_face=face;
			}
			fseek(f,2,SEEK_CUR);
		}
	}
	fclose(f);
	printf("\rLoaded %s model: %d faces, %d nodes\n",*filename,countFaces(body),countNodes(body));
	return body;
}
