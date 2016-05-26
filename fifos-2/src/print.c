#include "print.h"

void WriteCharacter(char *c)
{
	char forecolour=0x0F;
	char backcolour=0x00;
	int index = 0;
	char *video_buffer;
	char attrib;
	//resets the cursor if position outside 80*25
	
	while(c[index])
	{
		if(pos_x>=80)
		{
			pos_x=0;
			pos_y=pos_y+1;
		}
		if(pos_y>25)
			break;
		//now print it
		attrib = (backcolour << 4) | (forecolour & 0x0F);
		video_buffer = (char *)0xB8000 + (pos_y * 80 + pos_x) ;
		*video_buffer = attrib;
		*video_buffer++ = c[index];
		index++;
		pos_x = pos_x + 2;
	}
	
     return;
}

void print_num(long mem)
{
	int j=0,n[10],m;
	char *ch="";
	
	pos_x = 0;
	//pos_y ++;	
while(mem>0)
		{
			n[j] = mem%10;
			mem = mem/10;
			j++;
		}
		for(m=j-1;m>=0;m--)
		{
			*ch = n[m] + '0';
			WriteCharacter(ch);
			//pos_x= pos_x -2;
		}
return;

}