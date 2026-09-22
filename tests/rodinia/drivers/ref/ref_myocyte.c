// Reference Implementation for Rodinia Myocyte
// Directly reuses authentic Rodinia 3.1 openmp/myocyte solver, integrator, and ODE model

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define fp float
#define EQUATIONS 91
#define PARAMETERS 16

#define max(x,y) ( (x) < (y) ? (y) : (x) )
#define min(x,y) ( (x) < (y) ? (x) : (y) )

#include "ecc.c"
#include "cam.c"
#include "fin.c"
#include "master.c"
#include "embedded_fehlberg_7_8.c"
#include "solver.c"

#ifdef __cplusplus
extern "C" {
#endif

void ref_myocyte(int xmax,
                 float y[EQUATIONS],
                 const float params[PARAMETERS]) {
    fp **y_arr = (fp **)malloc((xmax + 1) * sizeof(fp *));
    for (int j = 0; j <= xmax; j++) {
        y_arr[j] = (fp *)malloc(EQUATIONS * sizeof(fp));
    }
    fp *x_arr = (fp *)malloc((xmax + 1) * sizeof(fp));

    for (int i = 0; i < EQUATIONS; i++) {
        y_arr[0][i] = y[i];
    }

    solver(y_arr, x_arr, xmax, (fp *)params, 1);

    for (int i = 0; i < EQUATIONS; i++) {
        y[i] = y_arr[xmax][i];
    }

    for (int j = 0; j <= xmax; j++) {
        free(y_arr[j]);
    }
    free(y_arr);
    free(x_arr);
}

#ifdef __cplusplus
}
#endif
