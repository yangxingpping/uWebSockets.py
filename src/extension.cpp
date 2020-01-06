#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <App.h>
#include <iostream>

/////////////////////////////////////////////////////////

// HttpResponse object
typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
	uWS::HttpResponse<false> *res;
} HttpResponseObject;

// HttpResponse::end
static PyObject *HttpResponse_end(HttpResponseObject *self, PyObject *args) {

	PyObject *one;
	PyArg_ParseTuple(args, "O", &one);

	// string to utf8
	Py_ssize_t size;
	const char *str = PyUnicode_AsUTF8AndSize(one, &size);
	std::string_view message(str, size);

	self->res->end(message);
	return Py_None;
}

// HttpResponse method list
static PyMethodDef HttpResponse_methods[] = {
    {"end", (PyCFunction) HttpResponse_end, METH_VARARGS,
     "no doc"
    },
    {NULL}  /* Sentinel */
};

// HttpResponse type
static PyTypeObject HttpResponseType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "whatever",
    .tp_basicsize = sizeof(HttpResponseObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = "no doc",
    .tp_methods = HttpResponse_methods,
    .tp_new = PyType_GenericNew,
};



/////////////////////////////////////////////////////////



// data for App object
typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
	uWS::App *app;
	PyObject *cb;
} CustomObject;

// typical calling convention
static PyObject *App_listen(CustomObject *self, PyObject *args) {

	// expect one function
	PyObject *one;
	PyArg_ParseTuple(args, "O", &one);

	// store cb for later
	self->cb = one;
	Py_INCREF(self->cb);


	self->app->get("/*", [self](auto *res, auto *req) {


			// response
			Py_INCREF(&HttpResponseType);
			HttpResponseObject *obj = PyObject_New(HttpResponseObject, &HttpResponseType);
			PyObject *resObj = PyObject_Init((PyObject *) obj, &HttpResponseType);
			obj->res = res;

			// request
			Py_INCREF(&HttpResponseType);
			HttpResponseObject *obj2 = PyObject_New(HttpResponseObject, &HttpResponseType);
			PyObject *reqObj = PyObject_Init((PyObject *) obj2, &HttpResponseType);
			obj2->res = res;

			// call python
			PyObject *call = PyObject_CallFunction(self->cb, "OO", resObj, reqObj);
			if (!call) {
				PyErr_Print();
			}

			// decrement
			Py_DECREF(resObj);
			Py_DECREF(reqObj);

	}).listen(3000, [](auto *token) {
	    if (token) {
		std::cout << "Listening on port " << 3000 << std::endl;
	    }
	}).run();

	return Py_None;
}

// list of what methods are in App object
static PyMethodDef Custom_methods[] = {
    {"listen", (PyCFunction) App_listen, METH_VARARGS,
     "no doc"
    },
    {NULL}  /* Sentinel */
};


// App constructor
static PyObject *App_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	//printf("constructing app\n");


    CustomObject *self;
    self = (CustomObject *) type->tp_alloc(type, 0);
    if (self != NULL) {

	self->app = new uWS::App();

        /*self->first = PyUnicode_FromString("");
        if (self->first == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        self->last = PyUnicode_FromString("");
        if (self->last == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        self->number = 0;*/
    }
    return (PyObject *) self;
}

// custom type def
static PyTypeObject CustomType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "asdasdasd",
    .tp_basicsize = sizeof(CustomObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = "no doc",
    .tp_methods = Custom_methods,
    .tp_new = App_new,
};

// module def
static PyModuleDef custommodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "uwebsocketspy",
    .m_doc = "uWebSockets.py",
    .m_size = -1,
};

// constructs the module object with all its objects and methods (exports)
PyMODINIT_FUNC PyInit_uwebsocketspy(void)
{
    PyObject *m;
    if (PyType_Ready(&CustomType) < 0)
        return NULL;

    if (PyType_Ready(&HttpResponseType) < 0)
        return NULL;

    m = PyModule_Create(&custommodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&CustomType);
    if (PyModule_AddObject(m, "App", (PyObject *) &CustomType) < 0) {
        Py_DECREF(&CustomType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}
