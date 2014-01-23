#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include "slv.h"


/* Tri-Diagonal Solver */
void solve_matrix(int n, double* a, double* b, double* c, double* v, double* x)
{ 
	for(int i = 1; i<n; i++){
		double m = a[i]/b[i-1];
		b[i] = b[i]-m*c[i-1];
		v[i] = v[i]-m*v[i-1];
	}
	x[n-1] = v[n-1]/b[n-1];
	for(int i = n-2;i>=0;i--){
		x[i] = (v[i]-c[i]*x[i+1])/b[i];
	}
}

int main(int argc, char** argv)
{
	/* Initialize some variables */
	int nx = 40;
	int ny = 40;	
	int nt = 1000;
	double Re = 100.0;
	double dt = 0.001;
	
	/* Parse Input Arguments */
	for(int i = 0; i<argc-1; i++){
		if(strcmp(argv[i],"-nx")==0){
			nx = atoi(argv[i+1]);
			i++;
		} else if(strcmp(argv[i],"-ny")==0){
			ny = atoi(argv[i+1]);
			i++;
		} else if (strcmp(argv[i],"-nt")==0){
			nt = atoi(argv[i+1]);
			i++;
		} else if (strcmp(argv[i],"-Re")==0){
			Re = atof(argv[i+1]);
			i++;
		} else if (strcmp(argv[i],"-dt")==0){
			dt = atof(argv[i+1]);
			i++;
		}	
	}

	/* Calcs for Input */
	double dx = 1.0/((double)(nx-1));
	double dy = 1.0/((double)(ny-1));

	/* Echo to User */
	printf("***\t2D Incompressible Navier Stokes Lid Driven Cavity Solver\t***\n");
	printf("\n\tGrid size in X: %i\n",nx);
	printf("\tGrid size in Y: %i\n",ny);
	printf("\tReynolds Number: %f\n",Re);
	printf("\tTime Step: %f\n",dt);
	printf("\tTime Iterations: %i\n",nt);
	printf("\n\tEstimated Memory Usage: (words)\n");

	/*Allocate Memory for Spacial Variables */
#ifdef DEBUG
	printf("\t Begin Memory Allocation \n");
#endif
	double** omega = malloc(nx*sizeof(double*));

	double** phi = malloc((nx+1)*sizeof(double*));
	double** phinext = malloc((nx+1)*sizeof(double*));	
	double** strm = malloc((nx+3)*sizeof(double*));
	double** strmnext = malloc((nx+3)*sizeof(double*));
	
	double** u = malloc(nx*sizeof(double*));
	double** us = malloc(nx*sizeof(double*));
	double** dus = malloc(nx*sizeof(double*));
	double** duss = malloc(nx*sizeof(double*));

	double** v = malloc((nx+1)*sizeof(double*));
	double** vs = malloc((nx+1)*sizeof(double*));
	double** dvs = malloc((nx+1)*sizeof(double*));
	double** dvss = malloc((nx+1)*sizeof(double*));
		
	double** usv = malloc(nx*sizeof(double*));
	double** vsv = malloc(nx*sizeof(double*));

	double** hu = malloc(nx*sizeof(double*));
	double** huold = malloc(nx*sizeof(double*));

	double** hv = malloc((nx+1)*sizeof(double*));
	double** hvold = malloc((nx+1)*sizeof(double*));
	
	for(int i = 0; i<nx+2; i++){
		if(i != (nx) && i != (nx+1)){
			u[i] = malloc((ny+1)*sizeof(double));	
			us[i] = malloc((ny+1)*sizeof(double));		
			dus[i] = malloc((ny+1)*sizeof(double));		
			duss[i] = malloc((ny+1)*sizeof(double));
					
			hu[i] = malloc((ny+1)*sizeof(double));
			huold[i] = malloc((ny+1)*sizeof(double));

			usv[i] = malloc(ny*sizeof(double));
			vsv[i] = malloc(ny*sizeof(double));
			omega[i] = malloc(nx*sizeof(double));
		}
		if(i!=(nx+1)){
			phi[i] = malloc((ny+1)*sizeof(double));
			phinext[i] = malloc((ny+1)*sizeof(double));
	
			v[i] = malloc(ny*sizeof(double));		
			vs[i] = malloc(ny*sizeof(double));		
			dvs[i] = malloc(ny*sizeof(double));		
			dvss[i] = malloc(ny*sizeof(double));
	
			hv[i] = malloc(ny*sizeof(double));
			hvold[i] = malloc(ny*sizeof(double));
		}
		
		strm[i] = malloc((ny+2)*sizeof(double));
		strmnext[i] = malloc((ny+2)*sizeof(double));

		
		for(int j=0;j<ny+2;j++){
			if (i != nx && i != (nx+1)){
				u[i][j] = 0.0;
				us[i][j] = 0.0;
				dus[i][j] = 0.0;
				duss[i][j] = 0.0;
				hu[i][j] = 0.0;
				huold[i][j] = 0.0;
				if(j != ny && j != (ny+1)){
					usv[i][j]=0.0;
					vsv[i][j]=0.0;
					omega[i][j] = 0.0;
				}
			}
			if (j !=ny && j != (ny+1) && i != (nx+1)){
				v[i][j] = 0.0;
				vs[i][j] = 0.0;
				dvs[i][j] = 0.0;
				dvss[i][j] = 0.0;
				hv[i][j] = 0.0;
				hvold[i][j] = 0.0;
			}
			if (i != (nx+1)){
				if(j !=(ny+1)){
					phi[i][j] = 0.0;
					phinext[i][j] = 0.0;
				}
			}
			strm[i][j] = 0.0;
			strmnext[i][j] = 0.0;
		}
	}
#ifdef DEBUG
	printf("\t\t MEMORY ALLOCATION DONE \n\n");
#endif
	double min = 0.00001;
	/* Time Iteration Loop */
	for(int t = 0;t<nt;t++){
		if(t > nt*9.0/10.0){
			min = 0.00001;
		}
		if(t%1000==0){
			printf("\t %i / %i \n",t,nt);
		}
#ifdef DEBUG
		printf("\t Simulation Loop. Time: %f / %f \n",(double)t*dt,dt*nt);
#endif

		/* Set BCS */
#ifdef DEBUG
		printf("\t Setting BCS \n");
#endif
		set_bcs(u, v, dx, dy, nx, ny, Re);

		/* Solv Viscous Bergers Equation */
#ifdef DEBUG
		printf("\t Solving Viscous Bergers \n");
#endif
		slv_vbe(u,us,dus,duss,v,vs,dvs,dvss,hu,huold,hv,hvold,dx,dy,nx,ny,Re,dt);

		/* Calculate phi */	
#ifdef DEBUG
		printf("\t Solving Poisson Equation \n");
#endif
		slv_pssn(phi,phinext,us,vs,dx,dy,nx,ny,dt,min);

		/* Apply Projection */
#ifdef DEBUG
		printf("\t Applying Projection \n");
#endif
		apply_projection(phi,u,us,v,vs,dx,dy,nx,ny,dt);

		/* Repeat */
		if (t==nt-1){
			get_uv(u,v,usv,vsv,nx,ny);
			printf("\t Getting Vorticity\n");
			get_vorticity(u,v,omega,dx,dy,nx,ny);	
			printf("\t Getting Stream Function\n");
			get_stream(strm,strmnext,omega,dx,dy,nx,ny);	
			printf("\t Writing Data to Files\n");
			write_matrix_2d(usv, nx, ny, "u.dat");
			write_matrix_2d(vsv, nx, ny, "v.dat");
			write_matrix_2d(u,nx,ny+1,"uraw.dat");
			write_matrix_2d(v,nx+1,ny,"vraw.dat");
			write_matrix_2d(us,nx,ny+1,"us.dat");
			write_matrix_2d(vs,nx+1,ny,"vs.dat");
			write_matrix_2d(omega, nx, ny, "omega.dat");
			write_matrix_2d(phi,nx+1,ny+1,"phi.dat");
			write_matrix_2d(strm,nx+2,ny+2,"stream.dat");
		}
	} 
	
	printf("***\tDONE!\t***\n");
	return EXIT_SUCCESS;
}
	
/* Set Boundary Conditions */
void set_bcs(double** restrict u, double** restrict v, double dx, double dy, int  nx, int ny, double Re)
{
	#pragma omp parallel for
	for(int i=0;i<nx+1;i++){
		for(int j=0;j<ny+1;j++){
			if (j==0){
				if(i!=nx){
					u[i][j]=-u[i][j+1];	
				}
				v[i][j]=0.0;
			}
			if (j==ny-1){
				v[i][j]=0.0;
			}
			if(i==0){
				if(j!=ny){
					v[i][j]=-v[1][j];
				}
				u[i][j]=0.0;
			}
			if(i==nx){
				if(j!=ny){
					v[i][j]=-v[i-1][j];
				}
			}
			if(i==nx-1){
				u[i][j]=0.0;
			}	
			if (j==ny){
				if(i!=nx){
					u[i][j]=2-u[i][j-1];				
				}	
			}
		}
	}
}

/* Get u and V */
void get_uv(double** restrict u, double** restrict v, double** restrict usv,double** restrict vsv,int nx, int ny)
{
	#pragma omp parallel for
	for(int i = 0; i<nx; i++){
		for(int j = 0;j<ny; j++){
			usv[i][j] = (u[i][j]+u[i][j+1])/2.0;
			vsv[i][j] = (v[i][j]+v[i+1][j])/2.0;
		}
	}	
}

/* Solve for vorticity */
void get_vorticity(double** u, double** v,double** omega, double dx, double dy, int nx, int ny)
{
	#pragma omp parallel for
	for(int i = 0;i<nx;i++){
		for(int j=0;j<ny;j++){
			omega[i][j] = (v[i+1][j]-v[i][j])/dx-(u[i][j+1]-u[i][j])/dy;
		}
	}
}

/* Solve for Stream Function */
void get_stream(double** restrict strm, double** restrict strmnext, double** restrict omega, double dx, double dy, int nx, int ny)
{	
	double dy2 = pow(dy,2.0);
	double dx2 = pow(dx,2.0);
	double resid = 100.0;
	for(int cnt=0;cnt<100000;cnt++){
		#pragma omp parallel for
		for(int i=1;i<nx;i++){
			for(int j=1;j<ny;j++){
				strmnext[i][j] = 1.0/(2.0/dx2+2.0/dy2)*((strm[i+1][j]+strm[i-1][j])/(dx2)+(strm[i][j+1]+strm[i][j-1])/(dy2))+1.0/(2.0/dx2+2.0/dy2)*omega[i-1][j-1];
			}
		}
		#pragma omp parallel for
		for(int i=1;i<nx;i++){
			for(int j=1;j<ny;j++){
				strm[i][j] = 1.0/(2.0/dx2+2.0/dy2)*((strmnext[i+1][j]+strmnext[i-1][j])/(dx2)+(strmnext[i][j+1]+strmnext[i][j-1])/(dy2))+1.0/(2.0/dx2+2.0/dy2)*omega[i-1][j-1];
			}
		}
		resid = 0.0;
		for(int i=1;i<nx;i++){
			for(int j=1;j<ny;j++){
				resid  = resid+fabs(strm[i][j]-strmnext[i][j]);
			}
		}
	}	
	printf("residual = %f \n",resid);
}


/* Viscous Burgers Equation Solver */
/* Uses AB2 + ADI */
void slv_vbe(double** restrict u, double** restrict us, double** restrict dus, double** restrict duss, double** restrict v, double** restrict vs, double** restrict dvs, double** restrict dvss, double** restrict hu, double** restrict huold,double** restrict hv, double** restrict hvold, double dx, double dy, int nx, int ny, double Re, double dt)
{
	double dx2 = pow(dx,2.0);
	double dy2 = pow(dy,2.0);
	double ce = dt/(2.0*Re);
	#pragma omp parallel
	{	
		double* bum = malloc((nx-2)*sizeof(double));
		double* lum = malloc((nx-2)*sizeof(double));
		double* mum = malloc((nx-2)*sizeof(double));
		double* uum = malloc((nx-2)*sizeof(double));
		double* xum = malloc((nx-2)*sizeof(double));


		double* bvm = malloc((nx-1)*sizeof(double));
		double* lvm = malloc((nx-1)*sizeof(double));
		double* mvm = malloc((nx-1)*sizeof(double));
		double* uvm = malloc((nx-1)*sizeof(double));
		double* xvm = malloc((nx-1)*sizeof(double));
		
		#pragma omp parallel for
		for(int i=1;i<nx;i++){
			for(int j=1;j<ny;j++){	
				/* Do this for all u */					
				if(i!=nx-1){
					huold[i][j] = hu[i][j];
					hu[i][j] = -((u[i+1][j]+u[i][j])*(u[i+1][j]+u[i][j])-(u[i-1][j]+u[i][j])*(u[i-1][j]+u[i][j]))/(4.0*dx)-((u[i][j+1]+u[i][j])*(v[i][j]+v[i+1][j])-(u[i][j-1]+u[i][j])*(v[i][j-1]+v[i+1][j-1]))/(4.0*dy);
				}		
				/* Do this for all v */
				if(j!=ny-1){
					hvold[i][j] = hv[i][j];
					hv[i][j] = -((v[i][j+1]+v[i][j])*(v[i][j+1]+v[i][j])-(v[i][j-1]+v[i][j])*(v[i][j-1]+v[i][j]))/(4.0*dy)-((v[i+1][j]+v[i][j])*(u[i][j]+u[i][j+1])-(v[i-1][j]+v[i][j])*(u[i-1][j]+u[i-1][j+1]))/(4.0*dx);
				}
			}
		}

		
		#pragma omp parallel for
		for(int j=1;j<ny;j++){
			for(int i = 1; i<nx;i++){
			
				/* limit range for u */
				if (i == 1){
					bum[i-1] = dt/2.0*(3.0*hu[i][j]-huold[i][j])+2.0*ce*((u[i+1][j]-2.0*u[i][j]+u[i-1][j])/dx2+(u[i][j+1]-2.0*u[i][j]+u[i][j-1])/dy2);
					lum[i-1] = -ce/dx2;
					mum[i-1] = (1+2*ce/dx2);
					
					if(j!=ny-1){
						bvm[i-1] = dt/2.0*(3.0*hv[i][j]-hvold[i][j])+2.0*ce*((v[i+1][j]-2.0*v[i][j]+v[i-1][j])/dx2+(v[i][j+1]-2.0*v[i][j]+v[i][j-1])/dy2);
						lvm[i-1] = -ce/dx2;
						mvm[i-1] = (1.0+2.0*ce/dx2);
					}
				} else if (i == nx-2){
					bum[i-1] = dt/1.0*(2.0*hu[i][j]-huold[i][j])+2.0*ce*((u[i+1][j]-2.0*u[i][j]+u[i-1][j])/dx2+(u[i][j+1]-2.0*u[i][j]+u[i][j-1])/dy2);
					mum[i-1] = (1+2*ce/dx2);
					uum[i-1] = -ce/dx2;
					
					if(j!=ny-1){
						bvm[i-1] = dt/2.0*(3.0*hv[i][j]-hvold[i][j])+2.0*ce*((v[i+1][j]-2.0*v[i][j]+v[i-1][j])/dx2+(v[i][j+1]-2.0*v[i][j]+v[i][j-1])/dy2);
						lvm[i-1] = -ce/dx2;
						mvm[i-1] = (1.0+2.0*ce/dx2);
						uvm[i-1] = -ce/dx2;
					}
				} else if (i == nx-1){
					if(j!=ny-1){
						bvm[i-1] = dt/2.0*(3.0*hv[i][j]-hvold[i][j])+2.0*ce*((v[i+1][j]-2.0*v[i][j]+v[i-1][j])/dx2+(v[i][j+1]-2.0*v[i][j]+v[i][j-1])/dy2);
						mvm[i-1] = (1.0+2.0*ce/dx2);
						uvm[i-1] = -ce/dx2;
					}
				} else {
					
					bum[i-1] = dt/2.0*(3.0*hu[i][j]-huold[i][j])+2.0*ce*((u[i+1][j]-2.0*u[i][j]+u[i-1][j])/dx2+(u[i][j+1]-2.0*u[i][j]+u[i][j-1])/dy2);
					lum[i-1] = -ce/dx2;
					mum[i-1] = (1.0+2.0*ce/dx2);
					uum[i-1] = -ce/dx2;
	
					if(j!=ny-1){
						bvm[i-1] = dt/2.0*(3.0*hv[i][j]-hvold[i][j])+2.0*ce*((v[i+1][j]-2.0*v[i][j]+v[i-1][j])/dx2+(v[i][j+1]-2.0*v[i][j]+v[i][j-1])/dy2);
						lvm[i-1] = -ce/dx2;
						mvm[i-1] = (1.0+2.0*ce/dx2);
						uvm[i-1] = -ce/dx2;
					}
				}
			}
			
			solve_matrix(nx-2,lum,mum,uum,bum,xum);
			if(j!=ny-1){
				solve_matrix(nx-1,lvm,mvm,uvm,bvm,xvm);
			}			

			for(int i = 1;i<nx;i++){
				if(i!=nx-1){
					duss[i][j] = xum[i-1];
				}
				if(j!=ny-1){
					dvss[i][j] = xvm[i-1];
				}
			}	
		}
	}
	#pragma omp parallel for
	for(int i = 1; i<nx; i++){
		for(int j=1; j<ny; j++){
			if(j!=ny-1){
				vs[i][j] = v[i][j]+dvss[i][j];
			}
			if(i!=nx-1){
				us[i][j] = u[i][j]+duss[i][j];
			}
		}
	}
	
	set_bcs(us, vs, dx, dy, nx, ny, Re);		
	
	#pragma omp parallel
	{
		double* bumy = malloc((nx-1)*sizeof(double));
                double* lumy = malloc((nx-1)*sizeof(double));
                double* mumy = malloc((nx-1)*sizeof(double));
                double* uumy = malloc((nx-1)*sizeof(double));
                double* xumy = malloc((nx-1)*sizeof(double));

                double* bvmy = malloc((nx-2)*sizeof(double));
                double* lvmy = malloc((nx-2)*sizeof(double));
                double* mvmy = malloc((nx-2)*sizeof(double));
                double* uvmy = malloc((nx-2)*sizeof(double));
                double* xvmy = malloc((nx-2)*sizeof(double));		

		for(int i = 1; i<nx; i++){
			for(int j = 1; j<ny; j++){
				/* limit range for v */
				if (j == 1){
					if(i!=nx-1){	
						bumy[j-1] = duss[i][j];	
						bumy[j-1] = dt/2.0*(3.0*hu[i][j]-huold[i][j])+2.0*ce*((us[i+1][j]-2.0*us[i][j]+us[i-1][j])/dx2+(us[i][j+1]-2.0*us[i][j]+us[i][j-1])/dy2);
						lumy[j-1] = -ce/dy2;
						mumy[j-1] = (1.0+2.0*ce/dy2);
					}
					bvmy[j-1] = dvss[i][j];		
					bvmy[j-1] = dt/2.0*(3.0*hv[i][j]-hvold[i][j])+2.0*ce*((vs[i+1][j]-2.0*vs[i][j]+vs[i-1][j])/dx2+(vs[i][j+1]-2.0*vs[i][j]+vs[i][j-1])/dy2);
					lvmy[j-1] = -ce/dy2;
					mvmy[j-1] = (1.0+2.0*ce/dy2);
				} else if (j == ny-2){
					if(i!=nx-1){
						bumy[j-1] = duss[i][j];	
						bumy[j-1] = dt/2.0*(3.0*hu[i][j]-huold[i][j])+2.0*ce*((us[i+1][j]-2.0*us[i][j]+us[i-1][j])/dx2+(us[i][j+1]-2.0*us[i][j]+us[i][j-1])/dy2);
						lumy[j-1] = -ce/dy2;
						mumy[j-1] = (1.0+2.0*ce/dy2);
						uumy[j-1] = -ce/dy2;
					}
					bvmy[j-1] = dvss[i][j];		
					bvmy[j-1] = dt/2.0*(3.0*hv[i][j]-hvold[i][j])+2.0*ce*((vs[i+1][j]-2.0*vs[i][j]+vs[i-1][j])/dx2+(vs[i][j+1]-2.0*vs[i][j]+vs[i][j-1])/dy2);
					mvmy[j-1] = (1.0+2.0*ce/dy2);
					uvmy[j-1] = -ce/dy2;
				} else if (j == ny-1){
					if(i!=nx-1){
						bumy[j-1] = duss[i][j];	
						bumy[j-1] = dt/2.0*(3.0*hu[i][j]-huold[i][j])+2.0*ce*((us[i+1][j]-2.0*us[i][j]+us[i-1][j])/dx2+(us[i][j+1]-2.0*us[i][j]+us[i][j-1])/dy2);
						mumy[j-1] = (1.0+2.0*ce/dy2);
						uumy[j-1] = -ce/dy2;
					}
				} else {
					if(i!=nx-1){	
						bumy[j-1] = duss[i][j];	
						bumy[j-1] = dt/2.0*(3.0*hu[i][j]-huold[i][j])+2.0*ce*((us[i+1][j]-2.0*us[i][j]+us[i-1][j])/dx2+(us[i][j+1]-2.0*us[i][j]+us[i][j-1])/dy2);
						lumy[j-1] = -ce/dy2;
						mumy[j-1] = (1.0+2.0*ce/dy2);
						uumy[j-1] = -ce/dy2;
					}
					bvmy[j-1] = dvss[i][j];	
					bvmy[j-1] = dt/2.0*(3.0*hv[i][j]-hvold[i][j])+2.0*ce*((vs[i+1][j]-2.0*vs[i][j]+vs[i-1][j])/dx2+(vs[i][j+1]-2.0*vs[i][j]+vs[i][j-1])/dy2);
					lvmy[j-1] = -ce/dy2;
					mvmy[j-1] = (1.0+2.0*ce/dy2);
					uvmy[j-1] = -ce/dy2;
				}
			}
			if(i!=nx-1){
				solve_matrix(ny-1,lumy,mumy,uumy,bumy,xumy);
			}
			solve_matrix(ny-2,lvmy,mvmy,uvmy,bvmy,xvmy);		
			for(int j = 1;j<ny;j++){
				if(j!=ny-1){
					dvs[i][j] = xvmy[j-1];
				}
				if(i!=nx-1){
					dus[i][j] = xumy[j-1];
				}
			}	
		}
	}
	#pragma omp parallel for
	for(int i = 1; i<nx; i++){
		for(int j=1; j<ny; j++){
			if(j!=ny-1){
				vs[i][j] = vs[i][j]+dvs[i][j];
			}
			if(i!=nx-1){
				us[i][j] = us[i][j]+dus[i][j];
			}
		}
	}
	/*
	#pragma omp parallel for
	for(int i = 0;i<nx+1;i++){
		vs[i][0] = v[i][0];	
		vs[i][ny-1] = v[i][ny-1];
		if(i!=nx){
			us[i][0] = u[i][0];
			us[i][ny] = u[i][ny];
		}
	}
	for(int j=0;j<ny+1;j++){
		us[0][j] = u[0][j];
		us[nx-1][j] = u[nx-1][j];
		if(j!=ny){
			vs[0][j] = v[0][j];
			vs[nx][j] = v[nx][j];
		}
	}
	*/
}

/* ftcs solver */
void slv_vbeftcs(double** restrict u, double** restrict us, double** restrict dus, double** restrict duss, double** restrict v, double** restrict vs, double** restrict dvs, double** restrict dvss, double** restrict hu, double** restrict huold,double** restrict hv, double** restrict hvold, double dx, double dy, int nx, int ny, double Re, double dt)
{
	
	double dx2 = pow(dx,2.0);
	double dy2 = pow(dy,2.0);
	double ce = dt/(Re);
	#pragma omp parallel for
	for(int i=1;i<nx;i++){
		for(int j=1;j<ny;j++){
			/* Do this for all u */					
			if(i!=nx-1){
				huold[i][j] = hu[i][j];
				hu[i][j] = -((u[i+1][j]+u[i][j])*(u[i+1][j]+u[i][j])-(u[i-1][j]+u[i][j])*(u[i-1][j]+u[i][j]))/(4.0*dx)-((u[i][j+1]+u[i][j])*(v[i][j]+v[i+1][j])-(u[i][j-1]+u[i][j])*(v[i][j-1]+v[i+1][j-1]))/(4.0*dy);
			}		
			/* Do this for all v */
			if(j!=ny-1){
				hvold[i][j] = hv[i][j];
				hv[i][j] = -((v[i][j+1]+v[i][j])*(v[i][j+1]+v[i][j])-(v[i][j-1]+v[i][j])*(v[i][j-1]+v[i][j]))/(4.0*dy)-((v[i+1][j]+v[i][j])*(u[i][j]+u[i][j+1])-(v[i-1][j]+v[i][j])*(u[i-1][j]+u[i-1][j+1]))/(4.0*dx);
			}
		}
	}
	#pragma omp parallel for
	for(int i=1;i<nx;i++){
		for(int j = 1; j<nx;j++){
			if(i!=nx-1){	
				us[i][j] =u[i][j]+dt*(hu[i][j])+ce*((u[i+1][j]-2.0*u[i][j]+u[i-1][j])/dx2+(u[i][j+1]-2.0*u[i][j]+u[i][j-1])/dy2);
			}
			if(j!=ny-1){
				vs[i][j] = v[i][j]+dt*(hv[i][j])+ce*((v[i+1][j]-2.0*v[i][j]+v[i-1][j])/dx2+(v[i][j+1]-2.0*v[i][j]+v[i][j-1])/dy2);
			}
		}	
	}
	#pragma omp parallel for
	for(int i = 0;i<nx+1;i++){
		vs[i][0] = v[i][0];	
		vs[i][ny-1] = v[i][ny-1];
		if(i!=nx){
			us[i][0] = u[i][0];
			us[i][ny] = u[i][ny];
		}
	}
	#pragma omp parallel for
	for(int j=0;j<ny+1;j++){
		us[0][j] = u[0][j];
		us[nx-1][j] = u[nx-1][j];
		if(j!=ny){
			vs[0][j] = v[0][j];
			vs[nx][j] = v[nx][j];
		}
	}
}


/* Poisson Solver */
void slv_pssn(double** restrict phi,double** restrict phinext, double** restrict us, double** restrict vs, double dx, double dy, int nx, int ny,double dt,double min)
{
	double dx2 = pow(dx,2.0);
	double dy2 = pow(dy,2.0);
	double save = 0.00;
	double resid = 100.0;
	int cnt = 0;
	while(resid>min){
		cnt = cnt+1;
		#pragma omp parallel for
		for(int i = 0;i<nx+1;i++){
			phi[i][0] = phi[i][1];
			phi[i][ny] = phi[i][ny-1];
		}
		#pragma omp parallel for 
		for(int j = 0;j<ny+1;j++){
			phi[0][j] = phi[1][j];
			phi[nx][j] = phi[nx-1][j];
		}
		#pragma omp parallel for
		for(int i = 1; i<nx;i++){
			for(int j = 1; j<ny; j++){
				phinext[i][j] = 1.0/(2.0/dx2+2.0/dy2)*((phi[i+1][j]+phi[i-1][j])/dx2+(phi[i][j+1]+phi[i][j-1])/dy2)-1.0/(dt*(2.0/dx2+2.0/dy2))*((us[i][j]-us[i-1][j])/dx+(vs[i][j]-vs[i][j-1])/dy);
			}
		}
		#pragma omp parallel for
		for(int i = 0;i<nx+1;i++){
			phinext[i][0] = phinext[i][1];
			phinext[i][ny] = phinext[i][ny-1];
		}
		#pragma omp parallel for 
		for(int j = 0;j<ny+1;j++){
			phinext[0][j] = phinext[1][j];
			phinext[nx][j] = phinext[nx-1][j];
		}
		#pragma omp parallel for
                for(int i = 1; i<nx;i++){
                        for(int j = 1; j<ny; j++){
				phi[i][j] = 1.0/(2.0/dx2+2.0/dy2)*((phinext[i+1][j]+phinext[i-1][j])/dx2+(phinext[i][j+1]+phinext[i][j-1])/dy2)-1.0/(dt*(2.0/dx2+2.0/dy2))*((us[i][j]-us[i-1][j])/dx+(vs[i][j]-vs[i][j-1])/dy);
                        }
                }
		resid = 0.0;
		for(int i=0; i<nx; i++){
			for(int j=0;j<ny;j++){
				save = fabs(phi[i][j]-phinext[i][j]); //Infinity Norm
				if (save>resid){
					resid = save;
				} 	
			}
		}
	}
#ifdef DEBUG
	printf("\t\t Poisson Iterations: %i\n",cnt);
#endif
}

/* Poisson Solver */
void slv_pssn_gmres(double** restrict phi,double** restrict phinext, double** restrict us, double** restrict vs, double dx, double dy, int nx, int ny,double dt,double min)
{


}
void apply_projection(double** restrict phi, double** restrict u, double** restrict us, double** restrict v, double** restrict vs, double dx, double dy, int nx, int ny, double dt)
{
	#pragma omp parallel for
	for(int i=1;i<nx;i++){
		for(int j=1;j<ny;j++){
			if(i!=nx-1){
				u[i][j] = us[i][j]-dt*(phi[i+1][j]-phi[i][j])/dx;
			}
			if(j!=ny-1){
				v[i][j] = vs[i][j]-dt*(phi[i][j+1]-phi[i][j])/dy;
			}
		}
	}	
}

	
/* Write 2D Matrix to File */
void write_matrix_2d(double** mat, int nx, int ny, char* filename)
{
	FILE *file = fopen(filename,"w");
	size_t count;
	for (int i = 0; i<nx; i++){
		count = fwrite(mat[i],sizeof(double),ny,file);
	}
}
