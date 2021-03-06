/*
 * slv.h
 *
 *  Created on: Jan 21, 2014
 *      Author: justin
 */

#ifndef SLV_H_
#define SLV_H_

typedef enum {WALL,PERIODIC,SLIPWALL} jdh_BC;

	struct slv_settings {
		int nx;
		int ny;
		int nt;
		double Re;
		double dt;
		double g;
		jdh_BC XBC;
		jdh_BC YBC;
		double XUV;
		double XLV;
		double YLV;
		double YRV;
		double XConst;
		double YConst;
		double rhog;
		double rhol;
		double mug;
		double mul;
		double Reg;
		double Rel;
		double sigma;
	};

void solve_matrix(int n, double* a, double* b, double* c, double* v, double* x);

void copy_2D(double** target, double** source, int nx, int ny);

/* Set Boundary Conditions */
void set_bcs(double** restrict u, double** restrict v, double dx, double dy, int  nx, int ny, int nghost, struct slv_settings st);

/* Set Neumann Boundary Conditions */
void set_all_bcs_neumann(double** restrict x, double dx, double dy, int  nx, int ny, int nghostx, int nghosty);

/* Periodic BCS */
void set_all_BCS(double** restrict x, double dx, double dy, int  nx, int ny, int nghost,struct slv_settings st);

/* Get u and V */
void get_uv(double** restrict u, double** restrict v, double** restrict usv,double** restrict vsv,int nx, int ny);

/* Solve for vorticity */
void get_vorticity(double** u, double** v,double** omega, double dx, double dy, int nx, int ny);

/* Solve for Stream Function */
void get_stream(double** restrict strm, double** restrict strmnext, double** restrict omega, double dx, double dy, int nx, int ny);

/* Viscous Burgers Equation Solver */
/* Uses AB2 + ADI */
void slv_vbe(double** restrict a, double** restrict u, double** restrict us, double** restrict dus, double** restrict duss, double** restrict v, double** restrict vs, double** restrict dvs, double** restrict dvss, double** restrict hu, double** restrict huold,double** restrict hv, double** restrict hvold, double dx, double dy, int nx, int ny, int nghost, double dt,struct slv_settings st);
/* ftcs solver */
void sslv_vbeftcs(double** restrict a, double** restrict G, double** restrict K, double** restrict u, double** restrict us, double** restrict dus, double** restrict duss, double** restrict v, double** restrict vs, double** restrict dvs, double** restrict dvss, double** restrict hu, double** restrict huold,double** restrict hv, double** restrict hvold, double dx, double dy, int nx, int ny, int nghost, double dt, struct slv_settings st);

/* Poisson Solver */
void slv_pssn(double** restrict phi,double** restrict phinext,double** restrict a, double** restrict us, double** restrict vs, double dx, double dy, int nx, int ny,int nghost, double dt,double min,struct slv_settings st);

/* Poisson Solver */
void slv_pssn_gmres(double** restrict phi,double* restrict x, double** restrict a, double** restrict us, double** restrict vs, double dx, double dy, int nx, int ny, int nghost, double dt,double min, struct slv_settings st);

/* gmres index helpers */
int get_pssn_ind(int i, int j, int nx, int ny);
int get_pssn_i(int ind, int nx, int ny);
int get_pssn_j(int ind, int nx, int ny);

/* find value in sparse triple pair array */
double find_in_A (double* A, int* iA, int* jA, int npt, int i, int j);

/* Apply Solution Projection into Solenoidal */
double apply_projection(double** restrict phi, double** restrict a, double** restrict u, double** restrict us, double** restrict v, double** restrict vs, double dx, double dy, int nx, int ny,int nghost, double dt,struct slv_settings st);


/* Write 2D Matrix to File */
void write_matrix_2d(double** mat, int nx, int ny, char* filename);

/* Write 2D Matrix to File */
void write_matrix_2d_int(int** mat, int nx, int ny, char* filename);

/*Init Settings*/
struct slv_settings init_settings();

/*Get weno5 weight*/
double slv_psi_weno(double a, double b, double c, double d);

void print_array(double* X, int n);

void print_array_int(int* X, int n);


#endif /* SLV_H_ */
