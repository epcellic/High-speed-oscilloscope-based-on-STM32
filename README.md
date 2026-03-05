# High-speed-oscilloscope-based-on-STM32
This is a scope project based on stm32 + ch340 + SerialPlot serial communication, featuring a 10 microsecond sampling period and using a binary transmission rate of 2,000,000 baud.

# >>Serial port host computer
SerialPlot：https://github.com/hyOzd/serialplot  
I chose this serial port software because it enables direct use of high-speed binary transmission and waveform plotting.

# >>The project includes Keil engineering files, hex files for burning binary files, and multisim14 circuit simulation files.
Please note that the external detection voltage should not exceed 0 - 3.3V. Therefore, a simple biasing voltage divider circuit is provided for your reference.

# Summary: What do you need?
## Hardware
·STM32F103 series  
·Ch340 TTL to Serial Port Conversion
## Software
·keil (optional)  
·multisim  
·flymcu (replaceable)  
·SerialPlot

# How to use it?
According to my configuration options
<img width="1117" height="234" alt="image" src="https://github.com/user-attachments/assets/571a677c-b4e5-4272-a941-22b07334efeb" />  
<img width="947" height="313" alt="image" src="https://github.com/user-attachments/assets/ecad7777-1127-4ae2-8906-70b3b4a9ea94" />  
(You need to select your own serial port here)
<img width="2555" height="400" alt="image" src="https://github.com/user-attachments/assets/adaa884e-0d94-4540-98b0-069108618bdb" />  
You can refer to the coordinate parameters of this chart. If you feel uncomfortable, you can make the adjustments yourself.
