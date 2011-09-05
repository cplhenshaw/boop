#include <stdio.h>
#include <stdlib.h>

#include "modl.h"
#include "objStructs.h"
#include "read3do.h"
#include "write3do.h"
#include "readObj.h"
#include "update3do.h"

int main( int argc, char *argv[] )
{
	if( argc != 4 )
	{
		fprintf(stderr, "Expected 3 arguments, two input and one output filenames.\n");
		fprintf(stderr, "Usage example '3do manny.3do updated.obj manny.3do'\n");
		exit(EXIT_FAILURE);
	}

	//read in the .3do file
	MODL *m = read3do(argv[1]);
	if(m == NULL)
	{
		fprintf(stderr, "Failed to read in .3do file %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	//read in the .obj file which will update the .3do
	OBJ *o = readObj(argv[2]);
	if(o == NULL)
	{
		fprintf(stderr, "Failed to read in .obj file %s\n", argv[2]);
		exit(EXIT_FAILURE);
	}

	//JOIN EM
	update3do(m, o);

	//write it out
	write3do(m, argv[3]);



	//free memory
	freeMODL(m);

	//no free functions for the obj end of things yet
	//not really needed 

	exit(EXIT_SUCCESS);
}
