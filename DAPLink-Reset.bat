@cd flasher
openocd -f interface/cmsis-dap.cfg -c "adapter_khz 1000" -f rtl8710.ocd -f cortex.ocd -c "init" -c "reset halt" -c "cortex_reboot" -c "shutdown"
rem 

