#include <Python.h>
#include<pthread.h>

static PyObject* threader_killThread(PyObject*, PyObject*);

static PyObject*
threader_killThread(PyObject* self, PyObject* arg) {
	unsigned long id = 0;
	if (!PyArg_ParseTuple(arg, "u", &id))
		return NULL;
	int succeeded = pthread_cancel(id);
	printf("Ended thread %u, with return code %d ", id, succeeded);
	return Py_BuildValue("i", succeeded);
}

static PyMethodDef ThreaderMethods[] = {
	{"killThread",  threader_killThread, METH_VARARGS,
		"Cancel a thread."},
	{NULL, NULL, 0, NULL}        
};

PyMODINIT_FUNC
initthreader(void)
{
	    (void) Py_InitModule("threader", ThreaderMethods);
}
