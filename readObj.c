/*Second attempt.  This time just read the contents of the .obj file into some structures and later can deal with how to output them. */
#include "objStructs.h"
#include "checkedMem.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LEN 64
#define MAX_GROUP_NAME 32

//global state, the vertice group which lines from the file are contributing to
GROUP *group;

int groupVertexCount;
int totalVertexCount;

int groupTexVertexCount;
int totalTexVertexCount;

int groupNormalCount;
int totalNormalCount;

void processGroupLine(OBJ *obj, char *line)
{	
    if(obj==NULL) fprintf(stderr, "NULL OBJ IN processGroupLine()\n");
    //stop the old group being edited
    //if(group != NULL) group = NULL;
    
    //update the global count variables
    totalVertexCount += groupVertexCount;
    groupVertexCount = 0;
    totalTexVertexCount += groupTexVertexCount;
    groupTexVertexCount = 0;
    totalNormalCount += groupNormalCount;
    groupNormalCount = 0;

    //if out of space, reallocate
    if(++obj->numGroups > obj->groupSize) growGroups(obj);
    
    //make a new Group in the array, and update the global variable
    group = obj->groups[obj->numGroups - 1] = createGROUP();
    //read in data
    group->groupName = checked_malloc(sizeof(char) * (MAX_GROUP_NAME + 1));
    sscanf(line, "o %32s", group->groupName);

}

void processVertexLine(char *line)
{
    if(group == NULL) return;
    if(++group->numVertices > group->vertSize) growVertices(group);
    float *v = group->vertices[group->numVertices - 1];
    int read = sscanf(line, "v %f %f %f", v, v+1, v+2);
    if(read != 3) fprintf(stderr, "sscanf() read %d values in processVertexLine()\n", read);
    //update the global count of all vertices in this group
    groupVertexCount++;
}

void processTexVertexLine(char *line)
{
    if(group == NULL) return;
    if(++group->numTexVertices > group->texVertSize) growTexVertices(group);
    float *vt = group->texVertices[group->numTexVertices - 1];
    int read = sscanf(line, "vt %f %f", vt, vt+1);
    if(read != 2) fprintf(stderr, "sscanf() read %d values in processTexVertexLine()\n", read);
    //update the global count of all texture vertices in this group
    groupTexVertexCount++;
}

void processNormalLine(char *line)
{
    if(group == NULL) return;
    if(++group->numNormals > group->normSize) growNormals(group);
    float *vn = group->normals[group->numNormals - 1];
    int read = sscanf(line, "vn %f %f %f", vn, vn+1, vn+2);
    if(read != 3) fprintf(stderr, "sscanf() read %d values in processNormalLine()\n", read);
    //update the global count of normals in this group
    groupNormalCount++;
}

void processFaceLine(char *line) 
{
    if(group == NULL) return;
    if(++group->numFaces > group->faceSize) growFaces(group);
    //allocate an OBJFACE structure
    OBJFACE *f = group->faces[group->numFaces-1] = createOBJFACE();
    //line = "f a/b/c d/e/f g/h/i ... ..."
    char *triplets[MAX_VERTS_PER_FACE + 1];
    
    char *current = line;
    while(*current != '\0') 
    {
	if(*current == ' ')
	{
	    *current = '\0';
	    //increment count and store char * to triplet of indices which should follow the space
	    triplets[f->numVertices++] = current+1;
	    if(f->numVertices > MAX_VERTS_PER_FACE)
	    {
		fprintf(stderr, "More than %d vertices in this face.\n", MAX_VERTS_PER_FACE);
		break;
	    }
	}
	current++;
    }

    //process them
    for(int i=0; i < f->numVertices; i++)
    {
	//read the index triplet
	int *it = f->indices[i];
	int read = sscanf(triplets[i], "%d%*c%d%*c%d", it, it+1, it+2);
	if(read != 3) fprintf(stderr, "sscanf() read %d values in processFaceLine()\n", read);
	
	//reduce the indices to be local to each group
	it[0] -= totalVertexCount;
	it[1] -= totalTexVertexCount;
	it[2] -= totalNormalCount;
    }
}

/* Add any relevant data from this line into the OBJ structure. */
void processLine(OBJ *obj, char *line)
{
    //determine what type of line it is
    switch(line[0])
    {
	case 'o':
	    processGroupLine(obj, line);	
	    break;
	case 'v':
	    //nested switch to examine second char
	    switch(line[1])
	    {
		case ' ':
		    processVertexLine(line);
		    break;
		case 't':
		    processTexVertexLine(line);
		    break;
		case 'n':
		    processNormalLine(line);
		    break;
		default:
		    fprintf(stderr, "Unhandled line: %s\n", line);
		    break;
	    }
	    break;
	case 'f':
	    processFaceLine(line);
	    break;
	default:
	    fprintf(stderr, "Unhandled line: %s\n", line);
	    break;
    }
}

/*Open a .obj file and read the relevant information into the structures defined in "objStructs.h".  Return 1 for success, 0 for failure.*/
OBJ *readObj(char *filename)
{
    if(filename == NULL)
    {
	fprintf(stderr, "readObj() passed NULL filename.\n");
	return NULL;
    }

    //open the file and ensure success
    FILE *ifp = fopen(filename, "r");
    if(ifp == NULL)
    {
	fprintf(stderr, "Could not open %s in readObj().\n", filename);
	return NULL;
    }

    //create an OBJ structure to populate
    OBJ *obj = createOBJ();

    //create a buffer to read into, and read the file
    char line[MAX_LINE_LEN + 1];
    while(fgets(line, sizeof(line), ifp) != NULL)
    {
	//process the line
	processLine(obj, line);

    }

    //check the state of the file stream
    if(feof(ifp)) 
    {
	printf("Read to completion of the .obj file\n");
    }
    else if(ferror(ifp) != 0)
    {
	fprintf(stderr, "Finished reading .obj file due to an error.\n");
	//free memory
	//NOT COMPLETED
	return NULL;
    }

    //close the file and report success
    fclose(ifp);
    return obj;
}

