This example demonstrates how to programm in LabView synchronous stream data acquisition from ADC and DIN and simultaneous preloaded cyclic DAC signal generation for L-502 and/or E-502 devices.

In example we first detect all detected devices on PCIe and USB buses and show them for user to select device. There is also option in example to use connected by Ethernet device by specify it IP-address.
After START button press we connect to selected device, configure it, preload DAC cyclic signals and start data acquisition and generation until STOP button press.


In this example we use .Net assembly lpcieNet.dll to get access to device functions from LabView  
(this assemply is part of "L502/E502 SDK" http://www.lcard.ru/download/lpcie_setup.exe),
therefore it is necessary to have Microsoft Net Framework 2.0 installed. (preinstalled in Windows started from Vista).
Also it is necessary to have installed x502api.dll, l502api.dll è e502api.dll libraries in system (also part of SDK)

This example can be used in all LabView versions started from 8.0. Use example from subdirectory 10.0 for LabView 10.0 
or higher and from subdirectory 8.0 for previous LabView version.