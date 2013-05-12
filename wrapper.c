/*
 * =====================================================================================
 *
 *       Filename:  wrapper.c
 *
 *    Description:  python module wrapper
 *
 *        Version:  1.0
 *        Created:  05/10/2013 10:12:29 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Reno
 *   Organization:  
 *
 * =====================================================================================
 */


#include "wrapper.h"

PyObject *ATOSError;
static PyObject *
symbolicate_wrapper(PyObject *self, PyObject *args)
{
    const char *arch, *executable;
    int result;

    int numofaddresses;
    PyObject* addresses_obj;

    if (!PyArg_ParseTuple(args, "ssO", &arch, &executable, &addresses_obj)
            || !PyTuple_Check(addresses_obj))
        return NULL;

    numofaddresses = PyTuple_Size(addresses_obj);
    char **addresses;
    addresses = malloc(sizeof(char *)*numofaddresses);
    int i = 0;
    PyObject *address_item;
    for (; i < numofaddresses; i++){
        address_item = PyTuple_GetItem(addresses_obj, i);
        if (PyString_Check(address_item)){
            addresses[i] = PyString_AsString(address_item);
        }else{
            PyErr_SetString(ATOSError, "tuple contains a non-string value.");
            return NULL;
        }
    }

    result = symbolicate(arch, executable, addresses, numofaddresses);

    free(addresses);
    if (result == -1){
        return NULL;
    }
    return Py_BuildValue("i", result);
}

static PyMethodDef ATOSMethods[] = {
    {"symbolicate",  symbolicate_wrapper, METH_VARARGS,
    "binary address to symbol."},
    {NULL, NULL, 0, NULL}        /*  Sentinel */
};

PyMODINIT_FUNC initatos(void)
{
    PyObject *m;

    m = Py_InitModule("atos", ATOSMethods);
    if (m == NULL)
        return;

    ATOSError = PyErr_NewException("atos.error", NULL, NULL);
    Py_INCREF(ATOSError);
    PyModule_AddObject(m, "error", ATOSError);
}

