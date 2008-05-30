#include <Python.h>
#include <wport.h>

/* struct used to hold a reference to a python callable object, and a python obj containing user data */
typedef struct {
    PyObject* callback;
    PyObject* arg;
    } callbackStruct;
PyObject* pyWFile_register(void *, void * , PyObject* pyfunc, PyObject* arg);
extern warc_bool_t     WFile_register       (void *, void *,
          int (*)
          (void *, const char *,
           const unsigned int),
          void *);
    
