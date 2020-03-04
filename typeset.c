#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#define MAX_FILE_SIZE (64*1024*1024)
char data[MAX_FILE_SIZE];
int data_len=0;

char token[MAX_FILE_SIZE];
char token_type[MAX_FILE_SIZE];
char word_def[MAX_FILE_SIZE];
char word_example[MAX_FILE_SIZE];
char word_syns[MAX_FILE_SIZE];
char word_type[MAX_FILE_SIZE];
int token_len=0;
int in_token=0;

FILE *o=NULL;

void emit_definition(void)
{
  fprintf(o,"\$\\bullet\$ \\ (%s) {%s.}\n",
	 word_type,word_def);
  if(word_example[0]) fprintf(o," \\textit{``%s''}\n",word_example);
  if (word_syns[0]) {
    fprintf(o," \\textsc{Synonyms:} %s\n",word_syns);
  }
  if (word_def) {
    
  }
}

void token_char(char c)
{
  if (in_token)
    token[token_len++]=c;
}

void token_start(int depth)
{
  in_token=1;
  token_len=0;
}

void token_end(int depth, int colonP)
{
  token[token_len]=0;
  if (colonP) strcpy(token_type,token);
  if ((!colonP)&&(depth==3)) {
    // Remember field
    if (!strcmp(token_type,"def")) strcpy(word_def,token);
    else if (!strcmp(token_type,"example")) strcpy(word_example,token);
    else if (!strcmp(token_type,"speech_part")) strcpy(word_type,token);
    else if (!strcmp(token_type,"speech\\_part")) strcpy(word_type,token);
    else if (!strcmp(token_type,"synonyms")) strcpy(word_syns,token);
    else {
      //      fprintf(stderr,"Encountered unknown token type '%s'\n",token);
      //      exit(-1);
    }
  }
  //   printf("'%s' (%s) @ %d, %d\n",token,token_type,depth,colonP);
  if ((!colonP)&&(depth==2)&&(!strcmp(token_type,"word")))
    {
      fprintf(o,"\\par \\markboth{%s}{%s}\\textbf{%s}\n",
	     token,token,token);
    }

  
  token_len=0;
  in_token=0;
}

void parse_file(FILE *f)
{
  int depth=0;
  
  data_len=fread(data,1,MAX_FILE_SIZE,f);
  fprintf(stderr,"%d bytes\n",data_len);
  for(int i=0;i<data_len;i++) {
    switch(data[i]) {
    case '{':
      depth++;
      if (depth==3) {
	word_example[0]=0;
	word_type[0]=0;
	word_syns[0]=0;
	word_def[0]=0;
      }
      break;
    case '}':
      if (depth) depth--;
      if (depth==2) {
	emit_definition();
      }
      break;
    case '$': case '_': case '%': case '^': case '@': case '#': case '&':
      token_char('\\');
      token_char(data[i]);
      break;
    case '\"':
      if (data[i+1]==':') {
	token_end(depth,1);
	i++;
      } else {
	if (!token_len)
	  token_start(depth);
	else token_end(depth,0);
      }
      break;
    case '\\':
      if (data[i+1]=='"') {
	// Escaped quote
	token_char(data[i+1]);
	i++;
      } else if (data[i+1]=='n') {
	// \n
	token_char('\n');
	i++;
      } else {
	fprintf(stderr,"Illegal escape sequence \\%c\n",data[i+1]);
	exit(-3);
      }
      break;
    default:
      token_char(data[i]);
      break;
    }
  }
}

int main(int argc,char **argv)
{

  DIR *d=opendir("data");
  if (!d) {
    fprintf(stderr,"Could not open data directory for reading.\n");
    exit(-1);
  }

  struct dirent *de;

  while((de=readdir(d))!=NULL) {
    if (strlen(de->d_name)>strlen(".json")) {
      char name[1024];
      strcpy(name,de->d_name);
      name[strlen(de->d_name)-5]=0;
      
      char filename[1024];
      snprintf(filename,1024,"data/%s",de->d_name);
      FILE *f=fopen(filename,"r");
      if (!f) {
	fprintf(stderr,"ERROR: Could not read '%s'\n",filename);
	exit(-3);
      }
      snprintf(filename,1024,"%s.tex",name);
      o=fopen(filename,"w");
      fprintf(o,
	      "\\section*{%s}\n"
	      "\\begin{multicols}{2}\n",
	      name);	      
      
      parse_file(f);

      fprintf(o,"\\end{multicols}\n");
      fprintf(o,"\\clearpage\n");
      
      fprintf(stderr,"Scanning %s\n",de->d_name);
      
      fclose(f);
    }
  }

  closedir(d);
  
}
