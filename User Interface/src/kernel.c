#include "kernel.h"

#include "utils.h"

#include "char.h"


uint32 vga_index;

uint16 cursor_pos = 0, cursor_next_line_index = 1;

static uint32 next_line_index = 1;

uint8 g_fore_color = WHITE, g_back_color = BLACK;


// if running on VirtualBox, VMware or on raw machine, 

// change CALC_SLEEP following to greater than 4

// for qemu it is better for 1

#define CALC_SLEEP 1


/*

this is same as we did in our assembly code for vga_print_char

vga_print_char:

mov di, word[VGA_INDEX]

mov al, byte[VGA_CHAR]

mov ah, byte[VGA_BACK_COLOR]

sal ah, 4

or ah, byte[VGA_FORE_COLOR]

mov [es:di], ax

ret

*/

uint16 vga_entry(unsigned char ch, uint8 fore_color, uint8 back_color) 

{

uint16 ax = 0;

uint8 ah = 0, al = 0;


ah = back_color;

ah <<= 4;

ah |= fore_color;

ax = ah;

ax <<= 8;

al = ch;

ax |= al;


return ax;

}


void clear_vga_buffer(uint16 **buffer, uint8 fore_color, uint8 back_color)

{

uint32 i;

for(i = 0; i < BUFSIZE; i++){

(*buffer)[i] = vga_entry(NULL, fore_color, back_color);

}

next_line_index = 1;

vga_index = 0;

}


void clear_screen()

{

clear_vga_buffer(&vga_buffer, g_fore_color, g_back_color);

cursor_pos = 0;

cursor_next_line_index = 1;

}


void init_vga(uint8 fore_color, uint8 back_color)

{

vga_buffer = (uint16*)VGA_ADDRESS;

clear_vga_buffer(&vga_buffer, fore_color, back_color);

g_fore_color = fore_color;

g_back_color = back_color;

}

void init_vga_fore(uint8 fore_color)

{

vga_buffer = (uint16*)VGA_ADDRESS;

g_fore_color = fore_color;


}

uint8 inb(uint16 port)

{

uint8 data;

asm volatile("inb %1, %0" : "=a"(data) : "Nd"(port));

return data;

}


void outb(uint16 port, uint8 data)

{

asm volatile("outb %0, %1" : : "a"(data), "Nd"(port));

}


void move_cursor(uint16 pos)
{
  outb(0x3D4, 14);
  outb(0x3D5, ((pos >> 8) & 0x00FF));
  outb(0x3D4, 15);
  outb(0x3D5, pos & 0x00FF);
}

void move_cursor_next_line()
{
  cursor_pos = 80 * cursor_next_line_index;
  cursor_next_line_index++;
  move_cursor(cursor_pos);
}

char get_input_keycode()

{

char ch = 0;

while((ch = inb(KEYBOARD_PORT)) != 0){

if(ch > 0)

return ch;

}

return ch;

}


/*

keep the cpu busy for doing nothing(nop)

so that io port will not be processed by cpu

here timer can also be used, but lets do this in looping counter

*/

void wait_for_io(uint32 timer_count)

{

while(1){

asm volatile("nop");

timer_count--;

if(timer_count <= 0)

break;

}

}


void sleep(uint32 timer_count)

{

wait_for_io(timer_count*0x02FFFFFF);

}


void print_new_line()

{

if(next_line_index >= 55){

next_line_index = 0;

clear_vga_buffer(&vga_buffer, g_fore_color, g_back_color);

}

vga_index = 80*next_line_index;

next_line_index++;

move_cursor_next_line();

}


void print_char(char ch)

{

vga_buffer[vga_index] = vga_entry(ch, g_fore_color, g_back_color);

vga_index++;

move_cursor(++cursor_pos);

}


void print_string(char *str)

{

uint32 index = 0;

while(str[index]){

if(str[index] == '\n'){

print_new_line();

index++;

}else{

print_char(str[index]);

index++;

}

}

}


void print_int(int num)

{

char str_num[digit_count(num)+1];

itoa(num, str_num);

print_string(str_num);

}





int read_int()

{

char ch = 0;

char keycode = 0;

char data[32];

int index = 0;

do{ 


keycode = get_input_keycode();

sleep(CALC_SLEEP);

sleep(CALC_SLEEP);

if(keycode == KEY_ENTER){

data[index] = '\0';

print_new_line();

break;

}else{

ch = get_ascii_char(keycode);

print_char(ch);

data[index] = ch;

index++;

}

sleep(CALC_SLEEP);

}while(ch > 0);


return atoi(data);

}



char getchar()

{

char keycode = 0;

sleep(CALC_SLEEP);

keycode = get_input_keycode();

sleep(CALC_SLEEP);

return get_ascii_char(keycode);

}





void read_numbers(int *day, int *month,int *year)
{ 
  
  print_string("Enter a day : ");
  sleep(CALC_SLEEP);
  *day = read_int();
  sleep(CALC_SLEEP);
  print_string("Enter a month : ");
  sleep(CALC_SLEEP);
  *month = read_int();
  sleep(CALC_SLEEP);
  print_string("Enter a year : ");
  sleep(CALC_SLEEP);
  *year = read_int();
}

void array_sum(){

 clear_screen();
init_vga_fore(BLUE);
print_string("\n^^...Enter the size of array ...^^ \n");
sleep(CALC_SLEEP);
init_vga_fore(RED);
int size=read_int();
sleep(CALC_SLEEP);

int a[size];
int sum=0;
int max=0;
sleep(CALC_SLEEP);
for(int i=0;i<size;i++)
 { 
    init_vga_fore(BLACK);
   print_string("\nEnter the elements : ");
   init_vga_fore(RED);
   a[i]=read_int();
   sleep(CALC_SLEEP);
   sum=sum+a[i];
   if(a[i]>max)
      max=a[i];
 }
 sleep(CALC_SLEEP);
 init_vga_fore(GREEN);
print_string("\n\nThe Sum of Elements Of Arrays : ");
print_int(sum);
print_string("\n\nThe maximum Element in Arrays : ");
print_int(max);

}


void calcualte_age(){
 clear_screen();
 int day,month,year,dayl,monthl,yearl,agey,agem,aged;
	//agey :your age of year
	//agem :your age of month
	//aged :your age of days
	

	  
       init_vga_fore(BRIGHT_MAGENTA);
	   print_string("             .....Welcome Dear User..... ");

       init_vga_fore(BLACK);
       print_string("\n\nOK... Now will calculate your ");
       print_string("age and your age stage.......\n");

           
       print_string("\n\n^^Please Enter the data of the current day ,");             
       print_string("then the month ,\nthen the year^^\n\n");       sleep(CALC_SLEEP);

       init_vga_fore(RED);

       read_numbers(&day,&month,&year);     

       init_vga_fore(BLACK);

       print_string("\n\n^^Please Enter the data of birth day ");
       print_string(",then the month ,\nthen the year^^\n\n");

       sleep(CALC_SLEEP);
       init_vga_fore(RED);
       read_numbers(&dayl,&monthl,&yearl);     

    	     if (day<dayl)
    	     {
	    	   day= day + 30 ;
		       month = month - 1 ;
    	     }
 
	         if (month < monthl)
    	    {
		       month = month + 12 ;
               year = year - 1 ;
	         }

	         aged = day - dayl ;
	         agem = month - monthl ;
	         agey = year - yearl ;
			
	
            init_vga_fore( BLACK);

		    print_string("\n\n^^...Your Age :");
            print_int(agey);
            print_string(" year ");
            print_int(agem);
            print_string(" month ");
            print_int(aged);
            print_string(" day .");
            
       
		
		
		   if(agey>=0&&agey<=17)
		        print_string("\n^^..YOU ARE NOW IN CHILDHOOD..^^");
		   else if(agey>=18&&agey<=30)
		        print_string("\n^^..YOU ARE NOW IN ADOLESCENCE..^^");
		   else if(agey>=31&&agey<=60)
		        print_string("\n^^..YOU ARE NOW IN YOUNG STAGE..^^");
		   else 
	        	print_string("\n^^..YOU ARE NOW IN THE OLD AGE STAGE..^^");  

   

    
 

}

void power()
{ 
   clear_screen();
  init_vga_fore(BLACK);
  print_string("\n^^... Enter The Base ...^^^ ");
  sleep(CALC_SLEEP);
  init_vga_fore(RED);
  int base=read_int();
 
  init_vga_fore(BLACK);
  print_string("\n^^... Enter The power ...^^ ");
  sleep(CALC_SLEEP);
  init_vga_fore(RED);
  int power=read_int();
  
  int result=1;
  
  while(power!=0){
    
   result=result*base;
   power--;
  }
  init_vga_fore(RED);
  print_string("\n\n^^...The Result : ");
  print_int(result);
  print_string("  ...^^\n");
}


void display(){


print_string("^^.........................................................................^^");


print_string("\n\n\n            ");
print_string("^^......");
init_vga_fore(BLUE);
print_string("W");init_vga_fore(GREEN);
print_string("E");init_vga_fore(CYAN);
print_string("L");init_vga_fore(RED);
print_string("L");init_vga_fore(MAGENTA);
print_string("C");init_vga_fore(BROWN);
print_string("O");init_vga_fore(GREY);
print_string("M");init_vga_fore(DARK_GREY);

print_string("  ");
print_string("T");init_vga_fore(BRIGHT_GREEN);
print_string("O");init_vga_fore(BRIGHT_CYAN);

print_string("  ");
print_string("M");init_vga_fore(BRIGHT_RED);
print_string("Y");init_vga_fore(BRIGHT_MAGENTA);


print_string("  ");
print_string("O");init_vga_fore(CYAN);
print_string("P");init_vga_fore(CYAN);
print_string("E");init_vga_fore(RED);
print_string("R");init_vga_fore(MAGENTA);
print_string("A");init_vga_fore(BROWN);
print_string("T");init_vga_fore(GREY);
print_string("I");init_vga_fore(DARK_GREY);
print_string("G");init_vga_fore(RED);
print_string("  ");
print_string("S");init_vga_fore(BRIGHT_BLUE);
print_string("Y");init_vga_fore(BRIGHT_RED);
print_string("S");init_vga_fore(BRIGHT_GREEN);
print_string("T");init_vga_fore(BRIGHT_MAGENTA);
print_string("E");init_vga_fore(BLACK);
print_string("M");init_vga_fore(BLACK);

print_string("......^^\n\n\n");

print_string("                      ^^......");
print_string("G");init_vga_fore(BLUE);
print_string("A");init_vga_fore(GREEN);
print_string("I");init_vga_fore(RED);
print_string("N");init_vga_fore(MAGENTA);
print_string("T");init_vga_fore(GREY);
print_string("  ");
print_string("T");init_vga_fore(BRIGHT_CYAN);
print_string("E");init_vga_fore(BRIGHT_MAGENTA);
print_string("A");init_vga_fore(BRIGHT_RED);
print_string("M");init_vga_fore(RED);
print_string("......^^\n\n");

init_vga_fore(GREEN);
print_string("\n\n^^..IT WAS CREATED BY:\n\n    E");init_vga_fore(BRIGHT_MAGENTA);
print_string("N");init_vga_fore(CYAN);
print_string("G");init_vga_fore(BRIGHT_RED);
print_string(" : ");init_vga_fore(RED);
print_string("HUDA  ABDALLA ALJAZZAR");

init_vga_fore(MAGENTA);
print_string("\n\n\n    E");init_vga_fore(BRIGHT_MAGENTA);
print_string("N");init_vga_fore(MAGENTA);
print_string("G");init_vga_fore(CYAN);
print_string(" : ");init_vga_fore(RED);
print_string("NOUR OSAMA ABO ANZA");

    init_vga_fore(BLACK);
    print_string("\n\n\n \n\n\n\n\n\n               >> Press any key to reload screen <<");
    getchar();
    clear_screen();




}































void authorized(){
 init_vga_fore(BRIGHT_MAGENTA);
print_string("\n      ^  ^  Dear User ...! Please Enter Username and passward  ^  ^  ");


 init_vga_fore(RED);
print_string("\n\n\n");
print_string("                 ^_^..........................................^_^\n\n\n");

 init_vga_fore(BLUE);
print_string("                              ^..Enter UserName : ");

 init_vga_fore(BLACK);
 sleep(CALC_SLEEP);
 int t=read_int(); 
 sleep(CALC_SLEEP);


 init_vga_fore(BLUE);
 print_string("\n\n\n                              ^..Enter Password : ");
 init_vga_fore(BLACK);
 int b=read_int();
     

init_vga_fore(RED);
    
print_string("\n\n\n                 ^_^..........................................^_^");


while(b!=1234 || t!=23011)
{            
      print_string("\n\n                               ERROR PASSWORD ...!!");     
      sleep(CALC_SLEEP);
      init_vga_fore(BLACK);
      print_string("\n\n\n\n Press any key to reload authentication screen ...!");
      getchar();
      clear_screen();      
      authorized();           
}


    init_vga_fore(BLACK);
    print_string("\n\n\n\n\n\n\n\n Press any key to reload screen ...!");
    getchar();
      
    clear_screen();

  
  

    
}





void kernel_entry()
{
  init_vga(BLACK, WHITE);
  int num,r;

   display();
   authorized();
sleep(CALC_SLEEP);
while(1 && num!=4 && r!=3){
init_vga_fore(BLACK);
print_string("     ^^... MENU APP ...^^\n\n\n");
init_vga_fore(RED);
print_string("1-^..Calculat Age Aplication..^\n\n");
print_string("2-^..Find Max Element In Array And Sum Of Their Elements..^\n\n");
print_string("3-^..Power && base App..^\n\n4-^..EXIT..^\n\n");
init_vga_fore(BLACK);
print_string("\n^..Enter choice..^\n");
init_vga_fore(RED);
sleep(CALC_SLEEP);
 num=read_int();

switch(num)
 {

  case 1:calcualte_age();break;

  case 2:array_sum();break;

  case 3:power();break;
 
  case 4:print_string("\n\nEXIT..");
         clear_screen();
          print_string("\n\n\n\n\n\n                    ^^....G");init_vga_fore(BLUE);
          print_string("O");init_vga_fore(GREEN);
          print_string("O");init_vga_fore(MAGENTA);
          print_string("D");init_vga_fore(BRIGHT_MAGENTA);
          print_string("  B");init_vga_fore(DARK_GREY);
          print_string("U");init_vga_fore(CYAN);
          print_string("Y....^^");init_vga_fore(RED);  
 
         break;

  default:print_string("...Wrong input...");

 }
    
clear_screen();

sleep(CALC_SLEEP);
sleep(CALC_SLEEP);

 init_vga_fore(RED);
print_string("\n\n\n");
print_string("                 ^_^..........................................^_^\n\n\n");

 init_vga_fore(BLUE);
 print_string("                              1-REBOOT ");
 print_string("\n\n\n                              2-SLEEP ");
  print_string("\n\n\n                              3-SHUTDOWN ");

    
init_vga_fore(RED);
    
print_string("\n\n\n                 ^_^..........................................^_^");

    init_vga_fore(BLACK);
    print_string("\n\n\n  ^^ ... Enter choice :");
    r=read_int();
   if(r==3){
   sleep(CALC_SLEEP);
   sleep(CALC_SLEEP);
  sleep(CALC_SLEEP);
init_vga_fore(BRIGHT_MAGENTA);
    print_string("\n\n\n\n\n\n\n\n\n\nPress any key to reload screen ...!");
    getchar();
   
   }else{
    init_vga_fore(BRIGHT_MAGENTA);
    print_string("\n\n\nPress any key to reload screen ...!");
    getchar();
   }

clear_screen();


}


}
