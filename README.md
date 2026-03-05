# High-speed-oscilloscope-based-on-STM32
This is a scope project based on stm32 + ch340 + SerialPlot serial communication, featuring a 10 microsecond sampling period and using a binary transmission rate of 2,000,000 baud.


# >>Serial port host computer
SerialPlot：https://github.com/hyOzd/serialplot
I chose this serial port software because it enables direct use of high-speed binary transmission and waveform plotting.

# >>The project includes Keil engineering files, hex files for burning binary files, and multisim14 circuit simulation files.
Please note that the external detection voltage should not exceed 0 - 3.3V. Therefore, a simple biasing voltage divider circuit is provided for your reference.
