 /*-----------------------------------------------------------------------------------------
 * Copyright (C) 2013  For the list of authors, see file AUTHORS.
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
 *   Read viscoacoustic model properties (vp,density,Qp) from files  
 *
 *  Copyright (c)  T. Bohlen
 *  ----------------------------------------------------------------------*/


/* This file contains function readmod, which has the purpose
   to read data from model-files for viscoelastic simulation */

#include "fd.h"

void readmod_viscac(float  **  rho, float **  pi, float ** taup, float * eta){
	
	extern float DT, *FL, TAU;
	extern int L;
	extern int NX, NY, NXG, NYG,  POS[3], MYID, INVMAT1;
	extern char  MFILE[STRING_SIZE];	
	extern FILE *FP;
	
	
	/* local variables */
	float rhov, piv, vp, qp, *pts;
	int i, j, ii, jj, l, sw_Qp=1;
	FILE *fp_vp, *fp_rho, *fp_qp;
	char filename[STRING_SIZE];
	
	
	/* vector for maxwellbodies */
	pts=vector(1,L);
	for (l=1;l<=L;l++) {
		pts[l]=1.0/(2.0*PI*FL[l]);
		eta[l]=DT/pts[l];
	}
	
	fprintf(FP,"\n...reading model information from modell-files...\n");
	
	/* read density and seismic velocities */
	/* ----------------------------------- */
	if(INVMAT1==1){
		fprintf(FP,"\t Vp:\n\t %s.vp\n\n",MFILE);
		sprintf(filename,"%s.vp",MFILE);
		fp_vp=fopen(filename,"r");
		if (fp_vp==NULL) err(" Could not open model file for Vp ! ");
		
		fprintf(FP,"\t Density:\n\t %s.rho\n\n",MFILE);
		sprintf(filename,"%s.rho",MFILE);
		fp_rho=fopen(filename,"r");
		if (fp_rho==NULL) err(" Could not open model file for densities ! ");
		
		fprintf(FP,"\t Qp:\n\t %s.qp\n\n",MFILE);
		sprintf(filename,"%s.qp",MFILE);
		fp_qp=fopen(filename,"r");
		if (fp_qp==NULL){
			if (MYID==0){
				printf(" Could not open model file for Qp-values ! \n");
				printf(" Uses input file value for tau! \n");
			}
			sw_Qp=0;
		}
	}
	
	
	/* loop over global grid */
	for (i=1;i<=NXG;i++){
	for (j=1;j<=NYG;j++){
		fread(&vp, sizeof(float), 1, fp_vp);
		fread(&rhov, sizeof(float), 1, fp_rho);
		if (sw_Qp){
			fread(&qp, sizeof(float), 1, fp_qp);
		}
		
		
		/* only the PE which belongs to the current global gridpoint 
		is saving model parameters in his local arrays */
		if ((POS[1]==((i-1)/NX)) && (POS[2]==((j-1)/NY))){
			ii=i-POS[1]*NX;
			jj=j-POS[2]*NY;
		
			rho[jj][ii]=rhov;
			pi[jj][ii]=vp;
			
			if (sw_Qp){
				taup[jj][ii]=2.0/qp;}
			else taup[jj][ii]=TAU;
		}
	}
	}
	
	
	fclose(fp_vp);
	fclose(fp_rho);
	if (sw_Qp)  fclose(fp_qp);
	
	
	/* each PE writes his model to disk */
	sprintf(filename,"%s.fdveps.pi",MFILE);
	writemod(filename,pi,3);
	MPI_Barrier(MPI_COMM_WORLD);

	if (MYID==0) mergemod(filename,3);
	
	sprintf(filename,"%s.fdveps.rho",MFILE);
	writemod(filename,rho,3);
	MPI_Barrier(MPI_COMM_WORLD);
	                        
	if (MYID==0) mergemod(filename,3);

	free_vector(pts,1,L);
}