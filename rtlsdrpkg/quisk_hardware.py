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
        self.rf_gain_labels = ('Auto','50','40','30','20','10', '6', '3', '0')
        self.tune = self.vfo = getattr(conf, "freq", None)

    def ChangeFrequency(self, tune, vfo, source='', band='', event=None):
        if vfo != self.vfo:
            rtl_sdr.set_center_frequency(vfo)
            self.vfo = vfo
        if tune != self.tune:
            self.tune = tune
        return self.tune, self.vfo

    def ReturnFrequency(self):
        return self.tune, self.vfo

    def open(self):
        rtl_sdr.set_decimation(self.decimation)
        rtl_sdr.open_samples()
        rtl_sdr.set_sample_rate(960000)
        rtl_sdr.set_sampling_mode(self.sampling_mode)
        rtl_sdr.set_auto_gain()
        if self.vfo:
            rtl_sdr.set_center_frequency(self.vfo)

    def close(self):
        rtl_sdr.close_samples()

    def OnButtonRfGain(self, event):
        btn = event.GetEventObject()
        n = btn.index
        if n:
            rtl_sdr.set_tuner_gain(10*int(self.rf_gain_labels[n]))
        else:
            rtl_sdr.set_auto_gain()




