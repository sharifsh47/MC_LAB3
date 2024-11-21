Configure the ADC0 to sample the output of the internal on-chip temperature
sensor instead of an analog channel with Sequencer 0. The ADC0 is to be
started by software triggering (see ADC0_PSSI_R register) each time a new
value is to be measured.

The temperature value is to be calculated from the digital ADC output and to
be printed on the console of the CCS with printf(). The temperature value is to
be updated and output every 1s. The intervall shall be controlled by a General
Purpose Timer Module (GPTM, reuse code from the previous lab session) in
compare mode.
