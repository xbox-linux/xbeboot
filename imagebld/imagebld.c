#include <errno.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

// #include <linux/hdreg.h>
#include <string.h>

#include <stdarg.h>
#include <stdlib.h>
#include "sha1.h"
#include "xbe-header.h"
#include "../config.h"


#define debug





struct Checksumstruct {
	unsigned char Checksum[20];	
	unsigned int Size_ramcopy;
	unsigned int compressed_image_start;
	unsigned int compressed_image_size;
	unsigned int Biossize_type;
}Checksumstruct;

void shax(unsigned char *result, unsigned char *data, unsigned int len)
{
	struct SHA1Context context;
	SHA1Reset(&context);
	SHA1Input(&context, (unsigned char *)&len, 4);
	SHA1Input(&context, data, len);
	SHA1Result(&context,result);	
}



int xberepair (	unsigned char * xbeimage,
		unsigned char * vmlinuzname,
		unsigned char * initrdname,
		unsigned char * configname
		)
{
	FILE *f;
	
        int a;
	unsigned char sha_Message_Digest[SHA1HashSize];

        unsigned char *xbe;
        unsigned int xbesize = 0;

    	unsigned char vmlinuz[2048*1024];
    	unsigned int vmlinux_size = 0;
        unsigned int vmlinux_start=0;
        
        unsigned char *initrd;         
        unsigned int initrd_size = 0;
	unsigned int initrd_start = 0;


        unsigned char *config;         
        unsigned int config_size = 0;
	unsigned int config_start = 0;

	
        unsigned int xbeloader_size=0;
	
	unsigned int temp;
		 
	XBE_HEADER *header;
 	XBE_SECTION *sechdr;

       	printf("ImageBLD Hasher by XBL Project (c) hamtitampti\n");
       	printf("XBEBOOT Modus\n\n");

#ifdef LOADXBE	
	f = fopen(vmlinuzname, "rb");
	if (f!=NULL) 
    	{    
 		fseek(f, 0, SEEK_END); 
         	vmlinux_size	 = ftell(f);        
         	fseek(f, 0, SEEK_SET);
    		memset(vmlinuz,0xff,sizeof(vmlinuz));
    		fread(vmlinuz, 1, vmlinux_size, f);
    		fclose(f);
    		printf("VMLinuz Existing, Linking it in\n");
    	} else  {
    		printf("VMLinuz not Existing     ----> ERROR \n");
    		return 1;
    		}
#endif
#ifdef LOADHDD_CFGFALLBACK
	f = fopen(configname, "rb");
	if (f!=NULL) 
    	{    
 		fseek(f, 0, SEEK_END); 
         	config_size	 = ftell(f);        
         	fseek(f, 0, SEEK_SET);
    		config = malloc(config_size);
    		fread(config, 1, config_size, f);
    		fclose(f);
		printf("Linuxboot.cfg Existing, Linking it in\n");
    	} else  {
    		printf("Linuxboot.cfg not Existing ---> ERROR \n");
    		return 1;
    		}
#endif
#ifdef LOADXBE
	f = fopen(initrdname, "rb");
	if (f!=NULL) 
    	{    
 		fseek(f, 0, SEEK_END); 
         	initrd_size	 = ftell(f);        
         	fseek(f, 0, SEEK_SET);
    		initrd = malloc(initrd_size);
    		fread(initrd, 1, initrd_size, f);
   		printf("Initrd Existing, Linking it in\n");
    	} else  {
    		printf("Initrd not Existing   ---> ERROR \n");
    		return 1;
    		}
#endif    	
	f = fopen(xbeimage, "rb");
    	if (f!=NULL) 
    	{   
  		fseek(f, 0, SEEK_END); 
         	xbesize	 = ftell(f); 
         	fseek(f, 0, SEEK_SET);
           	  
           	
//           	xbe = malloc(xbesize+vmlinux_size+1024*1024);
           	xbe = malloc(xbesize+vmlinux_size+3*1024*1024+initrd_size);
           	    		
    		memset(xbe,0x00,sizeof(xbesize+vmlinux_size+1024*1024));
    		fread(xbe, 1, xbesize, f);
    		fclose(f);
	      
	      	//printf("xxx:     : 0x%08X\n", (unsigned int)xbesize);
	        
	       	// We make some Allignment
	       	xbesize = (xbesize & 0xfffffff0) + 32;
	        	      
	        //vmlinux_size = (vmlinux_size & 0xfffff800 ) + 0x400;
	        
#ifdef LOADXBE
	        vmlinux_start = xbesize;
	        memcpy(&xbe[0x1080],&vmlinux_start,4);
	        
	        
	        memcpy(&xbe[vmlinux_start],vmlinuz,vmlinux_size);
		memcpy(&xbe[0x1084],&vmlinux_size,4);
		
		// We tell the XBEBOOT loader, that the Paramter he should pass to the Kernel = 1MB for the Size
		temp= 1024*1024;
		memcpy(&xbe[0x1088],&temp,4);		
		
		xbesize = xbesize + vmlinux_size;
		// Ok, we allign again
		xbesize = (xbesize & 0xfffffff0) + 32;

#endif
		
#ifdef LOADXBE		
		initrd_start = xbesize;
		memcpy(&xbe[0x108C],&initrd_start,4);
		memcpy(&xbe[0x1090],&initrd_size,4);
		
		memcpy(&xbe[initrd_start],initrd,initrd_size);
		
		xbesize = xbesize + initrd_size;	
                xbesize = (xbesize & 0xfffffff0) + 32;
#endif                

#ifdef LOADHDD_CFGFALLBACK
               	config_start = xbesize;
		memcpy(&xbe[0x1094],&config_start,4);
		memcpy(&xbe[0x1098],&config_size,4);               	
		
		memcpy(&xbe[config_start],config,config_size);               	

		xbesize = xbesize + config_size;	
                xbesize = (xbesize & 0xfffffff0) + 32;
#endif
                			        
		#ifdef debug
	 	printf("Linking Section\n");
	 	#ifdef LOADXBE	
	 	printf("Start of Linux Kernel    : 0x%08X\n", vmlinux_start);
	 	printf("Size of Linux Kernel     : 0x%08X\n", vmlinux_size);
		printf("Start of InitRD          : 0x%08X\n", initrd_start);
	 	printf("Size of Initrd           : 0x%08X\n", initrd_size);
		#endif		
		#ifdef LOADHDD_CFGFALLBACK
		printf("Start of Config          : 0x%08X\n", config_start);
	 	printf("Size of config           : 0x%08X\n", config_size);
	 	#endif
		printf("----------------\n");
		#endif	      
	      
	      
	      	  
	        header = (XBE_HEADER*) xbe;
	        
		// We calculate a new Size of the overall XBE, we allign too		
		
		xbeloader_size = xbesize - 0x1000;
		
		xbesize = (xbesize & 0xfffffff0) + 32;
		
	        header->ImageSize = xbesize; 
		
		//printf("%08x",sechdr->FileSize);                    
		
	        
	  	#ifdef debug
	 	printf("Size of all headers:     : 0x%08X\n", (unsigned int)header->HeaderSize);
         	printf("Size of entire image     : 0x%08X\n", (unsigned int)header->ImageSize);
		#endif

		// This selects the First Section, we only have one
		sechdr = (XBE_SECTION *)(((char *)xbe) + (int)header->Sections - (int)header->BaseAddress);
		

		sechdr->FileSize = xbeloader_size;
		sechdr->VirtualSize = xbeloader_size;
			        
        	shax(&sha_Message_Digest[0], ((unsigned char *)xbe)+(int)sechdr->FileAddress ,sechdr->FileSize);
	  	memcpy(&sechdr->ShaHash[0],&sha_Message_Digest[0],20);
	  	
	  	#ifdef debug
		
		printf("S0: Virtual address      : 0x%08X\n", (unsigned int)sechdr->VirtualAddress);
         	printf("S0: Virtual size         : 0x%08X\n", (unsigned int)sechdr->VirtualSize);
         	printf("S0: File address         : 0x%08X\n", (unsigned int)sechdr->FileAddress);
         	printf("S0: File size            : 0x%08X\n", (unsigned int)sechdr->FileSize);

		printf("Section 0 Hash XBE       : ");
		for(a=0; a<SHA1HashSize; a++) {
			printf("%02x",sha_Message_Digest[a]);
		}
	      	printf("\n");
	      	#endif




	      	
		// Write back the Image to Disk
		f = fopen(xbeimage, "wb");
    		if (f!=NULL) 
    		{   
		 fwrite(xbe, 1, xbesize, f);
        	 fclose(f);			
		}	  	
	        
	        printf("\nXbeboot.xbe Created    : %s\n",xbeimage);
	      	
		free(initrd);         
        	free(config);   

	} else return 1;


        
	return 0;	
}


int main (int argc, const char * argv[])
{
	int error;
	
	
	error = xberepair((unsigned char*)argv[1],(unsigned char*)argv[2],(unsigned char*)argv[3],(unsigned char*)argv[4]);
	
	return error;	
}
