/**
 * @file    UI.c
 * @brief   Describes how to use a command line interface and interact with users to execute
 *  		actions based on their commands. To include a new command, user has to append the
 *   		Command name, function name, and the String to the command line table as an entry
 *   		above help entry. Also, the user has to define a function with the function name
 *     		as entered into the command table.
 *
 *
 * @author	Venkat Sai Krishna Tata
 * @Date	05/02/2021
 */

//INCLUDES
#include"UART.h"
#include "UI.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include<stdio.h>
#include "touch.h"
#include "LEDs.h"
#include "mma8451.h"
#include <MKL25Z4.h>
#include <math.h>
#include "extra_switch.h"

//MACROS
#define LEN_MAX (640)
#define BS (8)
#define CR (13)
#define SP (32)
#define CMD_ARG (0)
#define FOUND (0)
#define NO_COMMAND (0)

//Prototype for command handler functions
typedef void (*command_handler_t)(int, char *argv[]);
typedef struct
{
	   const char *name;
	   command_handler_t handler;
	   const char *help_string;
} command_table_t;



/*
 * @Name		measure
 * @Description	Handler function for the command 'measure' which measures the orientation of the
 *				FRDM board with respect to surface on which it is placed. This orientation as an
 *				angle is provided by the integrated accelerometer. The same angle is displayed
 *				on the terminal window. The left side of the board is considered to be the vertex
 *				for the angle measurement.
 *
 *				Touching on the TSI slider will calibrate the device to 0 degrees and will be the
 *					basis for all angular measurements
 *				Pressing the push-button switch will terminate the measure functionality.
 * @parameters	int, char*
 *
 * @Returns		None
 */
static void measure(int argc,char *argv[])
{
	//Initially the switch to terminate the measure functionality is false
	switch_pressed=false;
	//Initializing the angle and calibrated position as 0
	int degree=0,set_value=0;

	//Until switch is pressed, tilt sensor (accelerometer) measures the orientation and prints it
	while(!switch_pressed)
	{

		int roll=compute_angle();
//		convert_xyz_to_roll_pitch();
		TSI0->DATA |= TSI_DATA_SWTS_MASK;
		if(touch_val>100)
		{
			touch_val=0;
			set_value=fabs(roll);
		}

		degree=fabs(roll)-set_value;
		printf("Measured angle: %3d\r",degree);
	}
	printf("\n\r");
}

/*
 * @Name		dump
 * @Description	Command Handler function for command dump which dumps memory contents
 *				from location specified by first argument of command and as many elements
 *				as specified by second argument of the command
 * @parameters	int, char*
 *
 * @Returns		None
 */
static void user(int argc,char *argv[])
{
	switch_pressed=false;
	int degree=0,set_value=0;
	uint16_t user_angle=0;
	char* ptr;
	user_angle = strtol(argv[1],&ptr,10);
	if(user_angle>180)
	{
		printf("Invalid angle input\n\r");
		return;
	}
	printf("Blue LED glows when the device is oriented at %d degrees\n\r",user_angle);
	while(!switch_pressed)
	{
		int roll=compute_angle();
		TSI0->DATA |= TSI_DATA_SWTS_MASK;
		if(touch_val>100)
		{
			touch_val=0;
			set_value=fabs(roll);
		}
		degree=fabs(roll)-set_value;
		Control_RGB_LEDs(0,0,0);
		if(degree==user_angle)
			Control_RGB_LEDs(0,0,1);
	}
	Control_RGB_LEDs(0,0,0);
}
static void fixed(int argc,char *argv[])
{
	switch_pressed=false;
	int degree=0,set_value=0;
	printf("If device oriented at 45,60 or 90 degrees, LED lights with purple,cyan or brown respectively\n\r");
	while(!switch_pressed)
	{
		int roll=compute_angle();
		TSI0->DATA |= TSI_DATA_SWTS_MASK;
		if(touch_val>100)
		{
			touch_val=0;
			set_value=fabs(roll);
		}
		degree=fabs(roll)-set_value;
		if(degree==45)
			Control_RGB_LEDs(0,1,1);
		else if(degree==60)
			Control_RGB_LEDs(1,0,1);
		else if(degree==90)
			Control_RGB_LEDs(1,1,0);
		else
			Control_RGB_LEDs(0,0,0);
	}
	Control_RGB_LEDs(0,0,0);
}
static void level(int argc,char *argv[])
{
	printf("Green LED indicates that the surface is level or plumb\n\r");
	switch_pressed=false;
	int degree=0,set_value=0;
	while(!switch_pressed)
	{
		int roll=compute_angle();
		TSI0->DATA |= TSI_DATA_SWTS_MASK;
		if(touch_val>100)
		{
			touch_val=0;
			set_value=fabs(roll);
		}
		degree=fabs(roll)-set_value;
		if(degree==0 || degree==90)
			Control_RGB_LEDs(0,1,0);
		else
			Control_RGB_LEDs(0,0,0);
	}
	Control_RGB_LEDs(0,0,0);
}
/*
 * @Name		help
 * @Description	Command Handler function for command help which prints out the
 *				description regarding each functionality to the user
 * @parameters	int, char*
 *
 * @Returns		None
 */
static void help(int argc,char *argv[])
{
	//Loop over all help commands and print the command name and equivalent help string
	for(int i=0;i<argc;i++)
	{
		printf("Command %s  :  %s\n\r",argv[i],argv[i+argc]);
	}
}

/*
 * @Name		handle_unknown
 * @Description	handler function which handles unknown commands and reports the same
 *				back to the user
 * @parameters	char*
 *
 * @Returns		None
 */
static void handle_unknown(char *argv[])
{
	//Display to user the command is unknown and specify the command entered
	printf("\n\rUnknown Command: ");
	char *p = argv[CMD_ARG];
	while(*p!='\0')
	{
		putchar(*p++);
	}
	printf("\n\r");
}

//Alter the command table to include new commands
//Steps to alter, include the command name as first argument of new structure element
//Include the name of the handler function for the command as second argument
//Include a string which describes the functionality of the command and display it
//When user calls for help
static const command_table_t commands[] = {
		{"measure", measure,"Measures and displays instantaneous angle measurements on the"\
				" terminal window"},
		{"user", user,"Syntax: user <Arg1> ; \n\r\t\tBlue LED glows when the device "\
				"is oriented at the angle (Arg1) input by the user"},
		{"fixed",fixed,"LED glows with colors purple, brown or cyan to indicate that the device"\
				" orientation is exactly at 45,60, or 90 degrees respectively"},
		{"level",level,"green LED indicates the surface is perfectly level or plumb (horizontally flat)."\
						"\n\r\t\t(must be calibrated to 0 degree first)"},
		{"help",help,"Provides information about all supported commands"},
};


/*
 * @Name		user_input
 * @Description	Accepts the character from user and generates string after the input
 * 				is committed by the user by pressing enter key
 *
 * @parameters	None
 *
 * @Returns		char* - Final string committed by user
 */
char* user_input()
{
	//Store the start of input string and parse
	char ui_buf[LEN_MAX];
	memset(ui_buf,0,LEN_MAX);
	char ch=' ',*ui_buf_ptr,*start;
	ui_buf_ptr=ui_buf;
	start=ui_buf;
	while(1)
	{
		putchar(ch=getchar());
		//If user enters enter signifying end of input, break from loop and terminate string
		if(ch==CR)
		{
			*ui_buf_ptr='\0';
			break;
		}
		//If user enters backspace, clear the previous entered character and remove the character
		//from the string
		else if(ch==BS)
			{
			printf(" \b");
			ui_buf_ptr--;
			*ui_buf_ptr=0;
			}
		//IF neither of these entered by user, store the characters in string
		else{
			*ui_buf_ptr=ch;
			ui_buf_ptr++;
			}
	}
	//Return the parsed string
	return(start);
}


/*
 * @Name		process_command
 * @Description	Parses input from the user and performs lexical analysis to tokenize the input
 * 				as arguments
 *
 * @parameters	char* input- user input provided from terminal
 *
 * @Returns		None
 *
 * Reference : code is taken as reference and developed over it. Author : Howdy Pierce
 */
void process_command(char *input)
{
  char *p = input;
  char *end;
  // Calculate end of string
  for (end = input; *end != '\0'; end++)
    ;
  // Tokenize input in place
  bool in_token = false;
  char *argv[10];
  int argc = 0;
  memset(argv, 0, sizeof(argv));
  //Parse till end of input string
  for (p = input; p < end; p++)
  {
	  //If user entered tab or new line, insert a NULL character
	  if(*p=='\r' || *p=='\t' || *p=='\n')
	  {
		  in_token=false;
		  *p='\0';
		  continue;
	  }
	  //If parsing middle of token
	  if(!in_token)
	  {
		  //If space entered, signifies end of token
		  //else store the characters of the token as arguments and tokenize the string
		  if(*p!=SP)
		  {
		  argv[argc]=p;
		  argc++;
		  in_token=true;
		  continue;
		  }
	  }
	  else
	  {
		  //If not in the middle of token, spaces are ignored and inserted NULL characters
		  if(*p==SP)
		  {
			  in_token=false;
			  *p='\0';
			  continue;
		  }

	  }
  }
  //If no command entered, returns
  if (argc == NO_COMMAND)
    return;

  static const int num_commands =  sizeof(commands) / sizeof(command_table_t);
   bool command_found=false;
   //Loop over the commands to find if a valid command is entered
   for (int i=0; i < num_commands; i++)
   {
	   //If command found ,call the handler
	   if (strcasecmp(argv[CMD_ARG], commands[i].name) == FOUND)
	   {
		   command_found=true;
		   //For command user, if arguments more than 2,
		   if((strcasecmp(argv[CMD_ARG], "user") == 0)&&(argc!=2))
		   {
			   printf("\n\rInvalid number of arguments to command '%s', refer help for correct"\
					   " syntax\n\r",argv[CMD_ARG]);
			   continue;
		   }
		   else if(!(strcasecmp(argv[CMD_ARG], "user") == 0)&&(argc>1))
		   {
			   printf("\n\rInvalid number of arguments to command '%s', refer help for correct"\
					   " syntax\n\r",argv[CMD_ARG]);
			   break;
		   }
		   //If command found is help command, pass the command names and handler strings
		   //as arguments
		   if (strcasecmp(argv[CMD_ARG], "help") == FOUND)
		   {
			   //First argument to function specifies the number of commands in the table
			   argc=num_commands;
			   //Include the commands and helper strings as arguments to help handler function
			   for(int j=0;j<num_commands;j++)
			   {
				   argv[j]=(char*)commands[j].name;
				   argv[j+num_commands]=(char*)commands[j].help_string;
			   }
		   }
		   printf("\n\r");
		   //Call the appropriate handler function
		   if(command_found==true)
			   commands[i].handler(argc, argv);
		   printf("\r");
		   //Set flag indicating the command is found
//		   command_found=true;
		   break;
	   }
   }
   //If command entered did not match with any of the valid commands, handle unknown commands
   if(!command_found)
   {
	   handle_unknown(argv);
   }
}

/*
 * See function description in UI.h
 */
void command_interface()
{
	Control_RGB_LEDs(0, 0, 0);
	printf("Welcome to Digital Angle Gauge Device!\n\r");
	while (1)
	{
		printf("? ");
		//Accept input from user and parse it
		process_command(user_input());
	}
}

