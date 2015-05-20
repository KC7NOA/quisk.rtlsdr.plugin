// -*- compile-command: "cd ..; python setup.py build" -*-

#include <Python.h>
#include <complex.h>
#include <stdlib.h>
#include "rtl-sdr.h"

#define ADJUSTMENT 1
#define IMPORT_QUISK_API
#include "quisk.h"

static int decimation;
static uint8_t *buffer;
static uint32_t out_block_size;
static rtlsdr_dev_t *dev;
static int dev_index = 0;

static int quisk_read_rtlsdr(complex double *samp)
{
    int i, j, n_read, r;
    
    r = rtlsdr_read_sync(dev, buffer, out_block_size, &n_read);
    if (r < 0) {
        fprintf(stderr, "WARNING: sync read failed. %d\n", r);
    }
        
    if ((uint32_t)n_read < out_block_size) {
        fprintf(stderr, "Short read, samples lost.\n");
    }
        
    for (i = j = 0; i < n_read; i += 2 * decimation, j++) {
        samp[j] = (buffer[i] - 127) + (buffer[i + 1] - 127) * I;
        samp[j] *= 16777216;
    }
    return j;
}

static PyObject * open_samples(PyObject * self, PyObject * args)
{
    out_block_size = 16384 * decimation / ADJUSTMENT;
    printf("out block size = %d\n", out_block_size);
    buffer = malloc(out_block_size * sizeof(uint8_t));
    rtlsdr_open(&dev, (uint32_t)dev_index);
    quisk_sample_source(NULL, NULL, &quisk_read_rtlsdr);
    rtlsdr_reset_buffer(dev);
    return Py_None;
}

static PyObject * close_samples(PyObject * self, PyObject * args)
{
    rtlsdr_close(dev);
    return Py_None;
}

static PyObject * set_decimation(PyObject * self, PyObject * args)
{
    int dec;
    if (!PyArg_ParseTuple (args, "i", &dec))
        return NULL;
    decimation = dec;
    printf("decimation = %d\n", decimation);
    return Py_None;
}

static PyObject * set_sample_rate(PyObject * self, PyObject * args)
{
    int rate;
    if (!PyArg_ParseTuple (args, "i", &rate))
        return NULL;
    printf("sampling rate = %d\n", rate);
    rtlsdr_set_sample_rate(dev, rate);
    return Py_None;
}

static PyObject * set_sampling_mode(PyObject * self, PyObject * args)
{
    int mode;
    if (!PyArg_ParseTuple (args, "i", &mode))
        return NULL;
    printf("direct sampling mode = %d\n", mode);
    rtlsdr_set_direct_sampling(dev, mode);
    return Py_None;
}

int nearest_gain(rtlsdr_dev_t *dev, int target_gain)
{
    int i, err1, err2, count, nearest;
    int* gains;
    rtlsdr_set_tuner_gain_mode(dev, 1);
    count = rtlsdr_get_tuner_gains(dev, NULL);
    gains = malloc(sizeof(int) * count);
    count = rtlsdr_get_tuner_gains(dev, gains);
    nearest = gains[0];
    for (i=0; i<count; i++) {
        err1 = abs(target_gain - nearest);
        err2 = abs(target_gain - gains[i]);
        if (err2 < err1) {
            nearest = gains[i];
        }
    }
    free(gains);
    return nearest;
}

static PyObject * set_tuner_gain(PyObject * self, PyObject * args)
{
    int gain;
    if (!PyArg_ParseTuple (args, "i", &gain))
        return NULL;
    rtlsdr_set_tuner_gain_mode(dev, 1);
    gain = nearest_gain(dev, gain);
    printf("gain (nearest) = %d\n", gain);
    rtlsdr_set_tuner_gain(dev, gain);
    return Py_None;
}

static PyObject * set_auto_gain(PyObject * self, PyObject * args)
{
    printf("setting gain = auto\n");
    rtlsdr_set_tuner_gain_mode(dev, 0);
    return Py_None;
}

static PyObject * set_center_frequency(PyObject * self, PyObject * args)
{
    int freq;
    if (!PyArg_ParseTuple (args, "i", &freq))
        return NULL;
    printf("center freq = %d\n", freq);
    rtlsdr_set_center_freq(dev, freq);
    return Py_None;
}

static PyMethodDef QuiskMethods[] = {
    {"open_samples", open_samples, METH_VARARGS, "Open the rtl-sdr."},
    {"close_samples", close_samples, METH_VARARGS, "Close the rtl-sdr."},
    {"set_decimation", set_decimation, METH_VARARGS, "Set decimation"},
    {"set_sample_rate", set_sample_rate, METH_VARARGS, "Set sample rate"},
    {"set_tuner_gain", set_tuner_gain, METH_VARARGS, "Set tuner gain"},
    {"set_auto_gain", set_auto_gain, METH_VARARGS, "Set tuner gain"},
    {"set_sampling_mode", set_sampling_mode, METH_VARARGS, "Enable direct sampling mode"},
    {"set_center_frequency", set_center_frequency, METH_VARARGS, "Set vfo frequency"},
    {NULL, NULL, 0, NULL}		/* Sentinel */
};

PyMODINIT_FUNC initrtl_sdr (void)
{
    if (Py_InitModule ("rtl_sdr", QuiskMethods) == NULL) {
        printf("Py_InitModule failed!\n");
        return;
    }
    if (import_quisk_api()) {
        printf("Failure to import pointers from _quisk\n");
        return;
    }
}












