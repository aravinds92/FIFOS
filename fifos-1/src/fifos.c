#include "multiboot.h"
#include "print.h" 

void gdt_done();

void mem(struct multiboot_info* mbt) 
{
	char *clear_screen = " ";
	int len = (int)mbt->mmap_length;
	unsigned long x = 0, mem=0;
	char *welcome = "Welcome to FIFOS: The System memory is: ";
	char *mb=" MB";

	memory_map_t* mmap = (memory_map_t*)mbt->mmap_addr;

		while(mmap <(mbt->mmap_addr + len))
		{
			if(mmap->type == 1)
				{
					x = mmap->length_low;
					mem = mem + x;
				}
							
			mmap = (memory_map_t*) ( (unsigned int)mmap + mmap->size + sizeof(unsigned int) );
		}
		mem = mem>>20;
		
		pos_x=0;
		pos_y=0;
		
		//clearing the screen before writing on it 
		while(1)
		{	
			WriteCharacter(clear_screen);
			if(pos_y>25)
				break;
		}
		
		//resetting the cursor value
		pos_x=0;
		pos_y=0;
	
		
		//printing the welcome message
			WriteCharacter(welcome);
		//printing the amount of free memory	
			print_num (mem);			
		//printing the unit of the free system memory
			WriteCharacter(mb);
		pos_y ++;
		pos_x = 0;
	return;
}

void gdt_done()
{
	char *gdt = "GDT initialised...";
	
	WriteCharacter(gdt);
	
	return;
}



