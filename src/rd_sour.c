/*-----------------------------------------------------------------------------------------
 * Copyright (C) 2016  For the list of authors, see file AUTHORS.
 *
 * This file is part of DENISE.
 * 
 * DENISE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2.0 of the License only.
 * 
 * DENISE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with DENISE. See file COPYING and/or <http://www.gnu.org/licenses/gpl-2.0.html>.
-----------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 *   Read extern source wavelet                                26 Mar 1997
 *
 *  written by T. Bohlen
 *  ----------------------------------------------------------------------*/

#include "fd.h"

float *rd_sour(int *nts,FILE* fp_source){

	/* local variables */
	float *psource;
	int i, c;

	if (fp_source==NULL) err(" Source file could no be opened !");
	/* fscanf(fp_source,"%i", nts); */
        *nts=0;
        while ((c=fgetc(fp_source)) != EOF)
         if (c=='\n') ++(*nts);
        rewind(fp_source);
			printf(" Number of samples (nts) in source file: %i\n",*nts);
		  
	psource=vector(1,*nts);
	for (i=1;i<=*nts;i++) fscanf(fp_source,"%e\n",&psource[i]);
	fclose(fp_source);
	return psource;
}
