# -*- compile-command: "cd ..; python setup.py build" -*-

# sample_rate = 24000, 48000, 96000, 192000
# sample_mode = 0: disabled, 1: I, 2: Q

import rtl_sdr
from quisk_hardware_model import Hardware as BaseHardware

class Hardware(BaseHardware):

    def __init__(self, app, conf):
        BaseHardware.__init__(self, app, conf)
        self.sampling_mode = getattr(conf, "sampling_mode", 0)
        if conf.sample_rate == 192000:
            self.decimation = 5
        elif conf.sample_rate == 96000:
            self.decimation = 10
        elif conf.sample_rate == 48000:
            self.decimation = 20
        else:
            raise Exception
        self.rf_gain_labels = ('Auto','0','3','6','9','10','20','25','30','40','50') 
        self.vfo = None

    def ChangeFrequency(self, tune, vfo, source='', band='', event=None):
        if vfo != self.vfo:
            rtl_sdr.set_center_frequency(vfo)
            self.vfo = vfo
        return tune, self.vfo

    def ReturnFrequency(self):
        return None, self.vfo

    def open(self):
        rtl_sdr.set_decimation(self.decimation)
        rtl_sdr.open_samples()
        rtl_sdr.set_sample_rate(960000)
        rtl_sdr.set_sampling_mode(self.sampling_mode)
        rtl_sdr.set_auto_gain()

    def close(self):
        rtl_sdr.close_samples()

    def OnButtonRfGain(self, event):
        btn = event.GetEventObject()
        n = btn.index
        if n:
            rtl_sdr.set_tuner_gain(10*int(self.rf_gain_labels[n]))
        else:
            rtl_sdr.set_auto_gain()












"""
# from rtlsdr import RtlSdr

class RtlSdr(object):
    def set_sample_rate(self, rate):
        print "setting sample rate to", rate
        pass
    def set_center_freq(self, freq):
        pass
    def set_direct_sampling(self, direct):
        pass
    def set_gain(self, gain):
        pass
    def close(self):
        pass

    def VarDecimGetChoices(self):         # return text labels for the control
        l = []              # a list of sample rates
        for dec in range(self.max_decimation):
            l.append(str(int(float(self.sample_rate) / (dec + 1))))
        print "choices", l
        return l

    def VarDecimGetLabel(self):           # return a text label for the control
        print "get label"
        return "Sample rate ksps"

    def VarDecimGetIndex(self):           # return the current index
        print "get index", self.index
        return self.index

    def VarDecimSet(self, index=None):            # set decimation, return sample 
        if index is None:   # initial call to set decimation before the call to open()
            print "reset decimation", index
            index = self.decimation - 1
        self.index = index
        dec = index + 1
        rtl_sdr.set_decimation(dec)
        print "setting to decimation", dec
        return int(float(self.sample_rate) / dec + .5)
"""

