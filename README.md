# PciReadForLinux
Access PCI configuration space on Linux of X86 platform

usage:

./pci -v                            
Show tool version

./pci -h                            
Show help

./pci                               
Show all Pci devices in your system

./pci bus dev fun                  
Show the PCI configuration space for the specified device

./pci bus dev fun reg               
Show the PCI configuration space by register

Note: Only for x86 platform!
Please run with root permission!
