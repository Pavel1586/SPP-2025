#pragma once
#include <fstream>
#define LEX_SEPARATORS	 'S'	
#define	LEX_ID_TYPE    	 't'	 		
#define	LEX_ID			 'i'	 		
#define	LEX_LITERAL		 'l'	 		
#define	LEX_FUNCTION     'f'		
#define	LEX_MAIN		 'm'		
#define	LEX_SEPARATOR	 ';'	  
#define	LEX_COMMA		 ','	   		
#define	LEX_LEFTBRACE	 '['	 		
#define	LEX_BRACELET	 ']'			
#define LEX_LEFT         '{'     
#define LEX_RIGHT        '}'     
#define	LEX_LEFTHESIS	 '('	   		
#define	LEX_RIGHTTHESIS	 ')'	   		
#define	LEX_PLUS		 '+'	 	
#define	LEX_MINUS		 '-'	 
#define	LEX_STAR		 '*'	 
#define LEX_DIRSLASH	 '/'	
#define LEX_PERSENT		 '%'	
#define	LEX_EQUAL		 '='	 				
#define LEX_LITERAL_HEX	 'h'	
#define LEX_CYCLE		 'c'	
#define LEX_WRITE		 'o'	// output
#define LEX_SWITCH		 's'	// switch
#define LEX_CASE		 'C'	// case
#define LEX_DEFAULT		 'D'	// default
#define LEX_BREAK		 'B'	// break
#define LEX_LITERAL_OCT	 'O'
#define LEX_RETURN		 'e'	
#define LEX_VOID		 'p'	
#define LEX_VAR			 'n'	 
#define LEX_MORE		 '>'	 
#define LEX_LESS		 '<'	
#define LEX_EQUALS		 '='	 
#define LEX_NOTEQUALS	 'N'	  
#define LEX_MOREEQUALS	 'G'    
#define LEX_LESSEQUALS	 'L'	   
#define	LEXEMA_FIXSIZE   1	   
#define	MAXSIZE_LT		 4096	 
#define	NULLDX_TI	 0xffffffff	


namespace LT	 
{
	struct Entry
	{
		char lexema;					
		int sn;							  
		int idxTI;						

		Entry();
		Entry(char lexema, int snn, int idxti = NULLDX_TI);
	};

	struct LexTable						
	{
		int maxsize;					 
		int size;						 
		Entry* table;					 
	};

	LexTable Create(int size);		            
	void Add(LexTable& lextable, Entry entry);	
	void writeLexTable(std::ostream* stream, LT::LexTable& lextable);		 
	void writeLexemsOnLines(std::ostream* stream, LT::LexTable& lextable);	
};
