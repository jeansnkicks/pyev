/*******************************************************************************
* utilities
*******************************************************************************/

/* set the Signal */
int
set_Signal(Signal *self, int signum)
{
    struct ev_loop *loop = ((Watcher *)self)->loop->loop;

    if (signum <= 0 || signum >= EV_NSIG) {
        PyErr_SetString(Error, "illegal signal number");
        return -1;
    }
    if (signals[signum - 1].loop && signals[signum - 1].loop != loop) {
        PyErr_SetString(Error, "the same signal must not be attached to two "
            "different loops");
        return -1;
    }
    signals[signum - 1].loop = loop;
    ev_signal_set(&self->signal, signum);
    return 0;
}


/*******************************************************************************
* SignalType
*******************************************************************************/

/* SignalType.tp_doc */
PyDoc_STRVAR(Signal_tp_doc,
"Signal(signum, loop, callback[, data=None])");


/* SignalType.tp_new */
static PyObject *
Signal_tp_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
    Signal *self = (Signal *)WatcherType.tp_new(type, args, kwargs);
    if (!self) {
        return NULL;
    }
    new_Watcher((Watcher *)self, (ev_watcher *)&self->signal, EV_SIGNAL);
    return (PyObject *)self;
}


/* SignalType.tp_init */
static int
Signal_tp_init(Signal *self, PyObject *args, PyObject *kwargs)
{
    int signum;
    Loop *loop;
    PyObject *callback, *data = NULL;

    static char *kwlist[] = {"signum",
                             "loop", "callback", "data", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "iO!O|O:__init__", kwlist,
            &signum, &LoopType, &loop, &callback, &data)) {
        return -1;
    }
    if (init_Watcher((Watcher *)self, loop, 0, callback, NULL, data)) {
        return -1;
    }
    if (set_Signal(self, signum)) {
        return -1;
    }
    return 0;
}


/* Signal.set(signum) */
PyDoc_STRVAR(Signal_set_doc,
"");

static PyObject *
Signal_set(Signal *self, PyObject *args)
{
    int signum;

    if (!PyArg_ParseTuple(args, "i:set", &signum)) {
        return NULL;
    }
    if (!inactive_Watcher((Watcher *)self)) {
        return NULL;
    }
    if (set_Signal(self, signum)) {
        return NULL;
    }
    Py_RETURN_NONE;
}


/* SignalType.tp_methods */
static PyMethodDef Signal_tp_methods[] = {
    {"set", (PyCFunction)Signal_set, METH_VARARGS, Signal_set_doc},
    {NULL}  /* Sentinel */
};


/* SignalType.tp_members */
static PyMemberDef Signal_tp_members[] = {
    {"signum", T_INT, offsetof(Signal, signal.signum), READONLY, ""},
    {NULL}  /* Sentinel */
};


/* SignalType */
static PyTypeObject SignalType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "pyev.Signal",                            /*tp_name*/
    sizeof(Signal),                           /*tp_basicsize*/
    0,                                        /*tp_itemsize*/
    0,                                        /*tp_dealloc*/
    0,                                        /*tp_print*/
    0,                                        /*tp_getattr*/
    0,                                        /*tp_setattr*/
    0,                                        /*tp_compare*/
    0,                                        /*tp_repr*/
    0,                                        /*tp_as_number*/
    0,                                        /*tp_as_sequence*/
    0,                                        /*tp_as_mapping*/
    0,                                        /*tp_hash */
    0,                                        /*tp_call*/
    0,                                        /*tp_str*/
    0,                                        /*tp_getattro*/
    0,                                        /*tp_setattro*/
    0,                                        /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    Signal_tp_doc,                            /*tp_doc*/
    0,                                        /*tp_traverse*/
    0,                                        /*tp_clear*/
    0,                                        /*tp_richcompare*/
    0,                                        /*tp_weaklistoffset*/
    0,                                        /*tp_iter*/
    0,                                        /*tp_iternext*/
    Signal_tp_methods,                        /*tp_methods*/
    Signal_tp_members,                        /*tp_members*/
    0,                                        /*tp_getsets*/
    0,                                        /*tp_base*/
    0,                                        /*tp_dict*/
    0,                                        /*tp_descr_get*/
    0,                                        /*tp_descr_set*/
    0,                                        /*tp_dictoffset*/
    (initproc)Signal_tp_init,                 /*tp_init*/
    0,                                        /*tp_alloc*/
    Signal_tp_new,                            /*tp_new*/
};
