#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <numpy/arrayobject.h>
#include <math.h>
#include <stdlib.h> 
//
//      Author:         Donald Guiles
//      Github:         https://github.com/Donny-GUI
//      Date:           2024-09-23
//      Version:        1.0
//      Description:    C extension to generate Bézier curve points
//      License:        GNU3 
//      References: 
//              https://en.wikipedia.org/wiki/B%C3%A9zier_curve
//              https://en.wikipedia.org/wiki/Pascal%27s_triangle
//
// 


/**
 * @brief Calculates a specific row of Pascal's Triangle
 *
 * @param[in] n: The row number (0-indexed)
 * @param[out] row: An array to store the row elements
 */
static void pascal_row(int n, int *row) {
    // The first and last elements of the row are always 1
    row[0] = row[n] = 1;
    for (int k = 1; k < n; k++) {
        // Calculate each element using the binomial coefficient formula
        // n! / (k! * (n-k)!)
        row[k] = row[k - 1] * (n - k) / k;
    }
}

/**
 * @brief Generates control points for the Bézier curve given two points and a deviation
 *
 * @param[in] args: tuple of arguments; two points (init and fin) and a deviation value
 * @return A 2D NumPy array of shape (4, 2) containing the control points
 */
static PyObject* generate_control_points(PyObject *self, PyObject *args) {
    double init_x, init_y, fin_x, fin_y, deviation;

    // Parse the input arguments: two points (init and fin) and a deviation value
    if (!PyArg_ParseTuple(args, "(dd)(dd)d", &init_x, &init_y, &fin_x, &fin_y, &deviation)) {
        return NULL; // Return NULL if parsing fails
    }

    // Calculate the distance between the initial and final positions
    double distance = sqrt(pow(fin_x - init_x, 2) + pow(fin_y - init_y, 2));
    double max_deviation = deviation * distance; // Calculate max deviation

    // Generate random control points based on the deviation
    double control_1_x = init_x + ((double)rand() / RAND_MAX) * 2 * max_deviation - max_deviation;
    double control_1_y = init_y + ((double)rand() / RAND_MAX) * 2 * max_deviation - max_deviation;
    double control_2_x = fin_x + ((double)rand() / RAND_MAX) * 2 * max_deviation - max_deviation;
    double control_2_y = fin_y + ((double)rand() / RAND_MAX) * 2 * max_deviation - max_deviation;

    // Create a 2D NumPy array to hold the control points
    npy_intp dims[2] = {4, 2}; // 4 control points, each with 2 coordinates (x, y)
    PyObject *control_points = PyArray_SimpleNew(2, dims, NPY_DOUBLE);
    double *data = (double *)PyArray_DATA(control_points); // Access the underlying data

    // Assign control points to the NumPy array
    data[0] = init_x; data[1] = init_y; // Control point 1
    data[2] = control_1_x; data[3] = control_1_y; // Control point 2
    data[4] = control_2_x; data[5] = control_2_y; // Control point 3
    data[6] = fin_x; data[7] = fin_y; // Control point 4

    return control_points; // Return the array of control points
}

/**
 * @brief Computes the Bézier curve points given control points
 *
 * @param[in] args: tuple of arguments; the first element is a 2D NumPy array of control points
 * @return A 2D NumPy array of shape (101, 2) containing the Bézier curve points
 */
static PyObject* bezier(PyObject *self, PyObject *args) {
    PyObject *control_points_obj; // Input: control points array
    int num_control_points; // Number of control points
    double *control_points; // Pointer to control points data

    // Parse the input arguments to get the control points
    if (!PyArg_ParseTuple(args, "O", &control_points_obj)) {
        return NULL; // Return NULL if parsing fails
    }

    // Get the number of control points and access their data
    num_control_points = (int)PyArray_DIM(control_points_obj, 0);
    control_points = (double *)PyArray_DATA(control_points_obj);

    // Create output array for Bézier curve points
    npy_intp dims[2] = {101, 2}; // 101 points, each with 2 coordinates (x, y)
    PyObject *points_array = PyArray_SimpleNew(2, dims, NPY_DOUBLE);
    double *points = (double *)PyArray_DATA(points_array); // Access the output data

    // Create an array to hold coefficients from Pascal's Triangle
    int *row = (int *)malloc(num_control_points * sizeof(int));
    pascal_row(num_control_points - 1, row); // Fill the array with coefficients

    // Loop to calculate each point on the Bézier curve
    for (int t_index = 0; t_index <= 100; t_index++) {
        double t = t_index / 100.0; // Normalize t to the range [0, 1]
        double x = 0.0; // Initialize x and y coordinates for the curve point
        double y = 0.0;
        
        // Calculate the Bézier curve point using the Bernstein polynomial
        for (int i = 0; i < num_control_points; i++) {
            double coeff = row[i] * pow(t, i) * pow(1 - t, num_control_points - 1 - i);
            x += coeff * control_points[2 * i]; // X-coordinate contribution
            y += coeff * control_points[2 * i + 1]; // Y-coordinate contribution
        }

        points[2 * t_index] = x; // Store the calculated point
        points[2 * t_index + 1] = y;
    }

    free(row); // Free allocated memory for coefficients
    return points_array; // Return the array of calculated points
}


// Method definitions for the module
static PyMethodDef BezierMethods[] = {
    {"generate_control_points", generate_control_points, METH_VARARGS, "Generate control points for Bézier curve"},
    {"bezier", bezier, METH_VARARGS, "Calculate Bézier curve points"},
    {NULL, NULL, 0, NULL} // Sentinel to mark the end of the method definitions
};

// Module definition structure
static struct PyModuleDef beziermodule = {
    PyModuleDef_HEAD_INIT,
    "bezier",   // Name of the module
    NULL, // No module documentation
    -1,       // Size of per-interpreter state of the module
    BezierMethods // Method definitions
};

// Initialization function for the module
/**
 * @brief Initializes the Bezier curve C extension module
 *
 * @return The created module
 */
PyMODINIT_FUNC PyInit_bezier(void) {
    import_array(); // Necessary for NumPy
    return PyModule_Create(&beziermodule); // Create the module and return it
}
