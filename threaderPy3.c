#include <Python.h>
#include<pthread.h>

static PyObject* threader_killThread(PyObject*, PyObject*);
static PyObject* threader_enableCancel(PyObject*, PyObject*);
static PyObject*
threader_killThread(PyObject* self, PyObject* arg) {
	int id = 0;
	if (!PyArg_ParseTuple(arg, "i", &id))
		return NULL;
	int succeeded = pthread_cancel(id);
	printf("Ended thread %d, with return code %d\n ", id, succeeded);
	return Py_BuildValue("i", succeeded);
}

static PyObject*
threader_enableCancel(PyObject* self, PyObject* arg) {
	int succeeded = 0;
	int a = 0;
	succeeded = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &a);
	printf("%d\n ", a);
	succeeded = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &a);
	printf("%d\n ", a);
	printf("Set cancel status %d\n ", succeeded);
	return Py_BuildValue("i", succeeded);
}

static PyMethodDef ThreaderMethods[] = {
	{"killThread",  threader_killThread, METH_VARARGS,
		"Cancel a thread."},
	{"enableCancel",  threader_enableCancel, METH_VARARGS,
		"Set cancel state to PTHREAD_CANCEL_ENABLE."},
	{NULL, NULL, 0, NULL}        
};

static struct PyModuleDef threadermodule = {
	PyModuleDef_HEAD_INIT,
	"threader",   /* name of module */
	NULL, /* module documentation, may be NULL */
	-1,       /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
	ThreaderMethods
};

PyMODINIT_FUNC
PyInit_threader(void)
{
	return PyModule_Create(&threadermodule);
}