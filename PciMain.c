#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/io.h>
#include<string.h>

#define PCI_MAX_BUS 255
#define PCI_MAX_DEV 31
#define PCI_MAX_FUN 7
#define PCI_BASE_ADDR 0x80000000L
#define CONFIG_ADDR 0xcf8
#define CONFIG_DATA 0xcfc
#define PCI_Addr(bus,dev,fun,reg) \
(((reg) & 0xfff) | (((fun) & 0x07) << 12) | (((dev) & 0x1f) << 15) | (((bus) & 0xff) << 20))

typedef unsigned long DWORD;
typedef unsigned int WORD;
typedef unsigned long DWORD;

void delay(unsigned int xms)
{
unsigned int i,j;
for(i=xms;i>0;i--)
for(j=110;j>0;j--);
}


void Int_To_Hex( int integer )
{
    int c;
    int     i, j, digit;
    
    for ( i = 7, j = 0; i >= 0; i--, j++ )
    {
        digit = (integer >> (i * 4)) & 0xf;
        if (digit < 10)
        {
            c = digit + 0x30;
        }
        else
        {
            c = digit + 0x37;
        }
    }
}


int tolower(int c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return c + 'a' - 'A';
    }
    else
    {
        return c;
    }
}

int htoi(char s[])
{
    int i;
    int n = 0;
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X'))
    {
            i = 2;
        }
    else
    {
            i = 0;
        }
    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i)
    {
            if (tolower(s[i]) > '9')
            {
                        n = 16 * n + (10 + tolower(s[i]) - 'a');
                    }
            else
            {
                        n = 16 * n + (tolower(s[i]) - '0');
                    }
        }
    return n;
}


void usage(){

	printf("usage:\n./pci \n");
	printf("./pci bus dev fun \n");
	printf("./pci bus dev fun reg\n");
	printf("Note:\n Only for x86 platform!\n");
	printf("Please run with root permission!\n\n");

}

void version(){

	printf("Version: 1.0.0\n");
	printf("Author : 530954029@qq.com\n\n");
	
}

char* getOptionRom(DWORD addr, char *OptionRom){

	DWORD 	 Enablebit;
	char 	 *have= "Have";
	char     *none = "NONE";

	outl(addr+0x30,CONFIG_ADDR);
	Enablebit = inl(CONFIG_DATA);

	Enablebit &=0x00000001;	
	
	if (Enablebit){
		OptionRom = have;
	}else{
		OptionRom = none;
	}
	
	return OptionRom;
}


char* getDeviceType(DWORD addr, char *devicetype){

	DWORD 	headtype;
 	char	*pcidevice="PCI device";
	char    *bridge="PCI-to-PCI bridge";
	char    *card ="Card Bus bridge";
	char    *unknow="Unknow";
	
	outl(addr+0x0C,CONFIG_ADDR);
	headtype = inl(CONFIG_DATA);
	headtype &=0x00ff0000;
	
	switch((headtype >> 16)&7){
		case 0x0: 
			devicetype = pcidevice;
			break;
		case 0x1:
			devicetype = bridge;
			break;

		case 0x2:
			devicetype = card;
			break;	
		
		default:
			devicetype = unknow;
			break;			
	}
	
	
	return devicetype;
}


int main(int argc,char *argv[])
{
	WORD bus,dev,fun,classcode;
	DWORD addr,data,tempaddr;
	int 	ret;
	int 	index=0;
	int 	rev;
	char 	*DeviceType;
	char    *OptionRom;
	
	if (argc != 1){
	
		int tempbus,tempdev,tempfun,tempreg;
		if (argc == 2){
			if(strcmp(argv[1] ,"-h") == 0 || strcmp(argv[1] ,"-?") == 0 ||strcmp(argv[1] ,"-help") == 0 ){
				usage();
				return 0;
			}
			
			if(strcmp(argv[1] ,"-v") == 0 || strcmp(argv[1] ,"-V") == 0 ||strcmp(argv[1] ,"-version") == 0 ){
				version();
				return 0;
			}
		}
		
		if(argc == 5){
			ret = iopl(3);
			if(ret < 0)
			{ 
				perror("Run with Root\n");
				return -1;
			}
			
			tempbus = htoi(argv[1]);
			tempdev = htoi(argv[2]);
			tempfun = htoi(argv[3]);
			tempreg = htoi(argv[4]);
			addr = PCI_BASE_ADDR|(tempbus << 16)|(tempdev << 11)|(tempfun << 8)+tempreg;
			outl(addr,CONFIG_ADDR);
			data = inl(CONFIG_DATA);
			printf("PCI Device:Bus %02X Dev %02X Fun %02X register data is %08lx\n",tempbus,tempdev,tempfun,data);
			return 0;
		}
		
		if(argc == 4){
			int i,j=0;
			int count = 1;
			ret = iopl(3);
			if(ret < 0)
			{ 
				perror("Run with Root\n");
				return -1;
			}
			
			tempbus = htoi(argv[1]);
			tempdev = htoi(argv[2]);
			tempfun = htoi(argv[3]);
			addr = PCI_BASE_ADDR|(tempbus << 16)|(tempdev << 11)|(tempfun << 8);
			printf("\nPCI Device:Bus %02X Dev %02X Fun %02X dump as follow:\n\n",tempbus,tempdev,tempfun);
			printf("\t0x00~0x03\t\t0x04~0x08\t\t0x09~0x0B\t\t0x0C~0x0F \t\n0x00 ");
			
			for (i = 0;i<=63;i++){
				delay(100);
				outl((addr+(i*4)),CONFIG_ADDR);
				data = inl(CONFIG_DATA);
				if(j == 4){
					j = 0;
					count++;
					if (count == 0x10){
						printf("\n");
						printf("\n");
						return 0;
					}
					printf("\n0x%02X",count);
				}
				if(data == 0xffffffff){
					printf("\n\n");
					return 0;
				}
				printf("\t%08lX\t",data);
				j++;
			}
			
			printf("\n\n");
			return 0;
		
		}
		usage();
	}
	
	ret = iopl(3);
	if(ret < 0)
	{ 
		printf("Run with Root\n");
		return -1;
	}
	
	printf("\n");
	printf("bus#\tdev#\tfun#\tdidvid#\t\tclasscode#\trevsion#\tdevicetype#\t\toptionrom#\t");
	printf("\n");
	
	for(bus = 0; bus <= PCI_MAX_BUS; bus++)
	for(dev = 0; dev <= PCI_MAX_DEV; dev++)
	for(fun = 0; fun <= PCI_MAX_FUN; fun++)
	{
		addr = PCI_BASE_ADDR|(bus << 16)|(dev << 11)|(fun << 8);
		outl(addr,CONFIG_ADDR);
		data = inl(CONFIG_DATA);
		if((data != 0xffffffff)&&(data != 0))
		{
			index++;		
			tempaddr = addr+0x08;
			outl(tempaddr,CONFIG_ADDR);
			classcode = inl(CONFIG_DATA);
			rev = (int) (classcode & 0x000000ff);
			classcode &= 0xffffff00;
			if (classcode == 0){
				classcode == 0x00;
			}else{
				classcode = classcode >> 8;
			}
			DeviceType=getDeviceType(addr,DeviceType);
			OptionRom=getOptionRom(addr,OptionRom);
			if(strcmp(DeviceType,"PCI-to-PCI bridge")==0){
				printf("%02X\t%02X\t%02X\t%08X\t %06X\t\t   %02X\t\t%s\t   %s\t",bus,dev,fun,( WORD )data,classcode,rev,DeviceType,OptionRom);
			}else{
				printf("%02X\t%02X\t%02X\t%08X\t %06X\t\t   %02X\t\t%s\t\t   %s\t",bus,dev,fun,( WORD )data,classcode,rev,DeviceType,OptionRom);
			}
			printf("\n");
			
		} 
	}
	printf("Pci devices count is = %d \n",index);
	
	ret = iopl(0);
	if(ret < 0){
		printf("iopl set error\n");
		return -1;
	}
	printf("\n");
	return 0;

}

