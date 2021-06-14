/* 
 * Copyright (c) 2021 Lucas Cordiviola 
 *
 * Pd external to change file(s) "mtime"
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "m_pd.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>   
#endif

typedef struct _filemtime {
  t_object  x_obj;
  t_canvas  *x_canvas;
  t_outlet *x_outlet1;
  int year;
  int month;
  int day;
  const char *dir;
  const char *wild;
  char fullpath[MAXPDSTRING];
  int haveargs;
  
   
  } t_filemtime;





t_class *filemtime_class;

static void filemtime_now(t_filemtime *x);
static void filemtime_fullpath(t_filemtime *x);







void filemtime_main(t_filemtime *x, t_symbol *s, int argc, t_atom *argv) 
{
    
    const char *buff;
    x->haveargs = 1;
    x->dir = "./";
    x->wild ="*.pd";
    filemtime_now(x);
	
    
    
    int i;

        for (i = 0; i < (argc); i++) {
            
            if (argv[i].a_type == A_SYMBOL) {    
            buff = argv[i].a_w.w_symbol->s_name;   
            } else {
                post("invalid args");   
                return;
            }
             
            if (!strcmp(buff, "-dir") ) {
            x->dir = argv[i+1].a_w.w_symbol->s_name;
            i++;
            } else if (!strcmp(buff, "-pattern") ) {
                x->wild = argv[i+1].a_w.w_symbol->s_name;
            i++;
            } else if (!strcmp(buff, "-year") ) {
                x->year = (int)argv[i+1].a_w.w_float;
            i++;
            } else if (!strcmp(buff, "-month") ) {
                x->month = (int)argv[i+1].a_w.w_float;
            i++;
            } else if (!strcmp(buff, "-day") ) {
                x->day = (int)argv[i+1].a_w.w_float;
            i++;
            
            } else { 
            
            break;
        }
  
    }
   
    filemtime_fullpath(x);
    return;
 
}
 


static void filemtime_do(t_filemtime *x) {

    if (!x->haveargs) {
		logpost(x,2,"[filemtime]: no arguments yet");
		return; 
	}
    
    char buf[MAXPDSTRING+200]= {"/0"};

#ifdef _WIN32 
    sprintf(buf, "powershell -command \" & {dir -Path \'%s\' -Filter %s\
      -Recurse | foreach-object {$_.LastWriteTime = New-object\
        DateTime %d,%02d,%02d}} \"",\
          x->fullpath, x->wild, x->year, x->month,x->day);  
    system(buf);
#else   
    sprintf(buf, "find \'%s\' -name %s -exec touch -t %d%02d%02d1200 {} + ;",\
      x->fullpath, x->wild, x->year, x->month,x->day);
    system(buf);
#endif  
     logpost(x,2,"[filemtime] did: %s",buf);   
    
}



static void filemtime_fullpath(t_filemtime *x) {
    
    char completefolder[MAXPDSTRING];   

    
    /* taken from iem's soundfile_info */
  
    if(x->dir[0] == '/')/*make complete path + folder*/
    {
        strcpy(completefolder, x->dir);
    }
    else if( (((x->dir[0] >= 'A')&&(x->dir[0] <= 'Z')) || ((x->dir[0]\
     >= 'a')&&(x->dir[0] <= 'z'))) && (x->dir[1] == ':')\
      && (x->dir[2] == '/') )
    {
        strcpy(completefolder, x->dir);
    }
    else
    {
        strcpy(completefolder, canvas_getdir(x->x_canvas)->s_name);
        strcat(completefolder, "/");
        strcat(completefolder, x->dir);
    }
    
    
    strcpy(x->fullpath, completefolder);


    /* </soundfile_info> */
	
    logpost(x,2,"dir: %s\npattern: %s\nyear: %d\nmonth: %d\nday: %d\n"\
    ,x->fullpath, x->wild, x->year, x->month, x->day );
}


static void filemtime_now(t_filemtime *x) {
    
#ifdef _WIN32
    SYSTEMTIME t; // Declare SYSTEMTIME struct
    GetLocalTime(&t); // Fill out the struct so that it can be used
    x->year = t.wYear;
    x->month = t.wMonth;
    x->day = t.wDay; 

    /*

    post("Year: %d, Month: %d, Day: %d, Hour: %d, Minute:%d, Second: %d, \
     Millisecond: %d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, \
     t.wSecond, t.wMilliseconds);
    */
#else
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    x->year = tm.tm_year + 1900;
    x->month = tm.tm_mon + 1;
    x->day = tm.tm_mday;
    
    /*
    post("now: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, \
    tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);   
    */
#endif
    
}

static void filemtime_free(t_filemtime *x) {
    
}

static void *filemtime_new(void)
{
  t_filemtime *x = (t_filemtime *)pd_new(filemtime_class);

  x->x_canvas = canvas_getcurrent();
  
  
  x->haveargs = 0;  
 
  return (void *)x;
}




void filemtime_setup(void) {

  filemtime_class = class_new(gensym("filemtime"),      
                   (t_newmethod)filemtime_new,
                   (t_method)filemtime_free,                          
                   sizeof(t_filemtime),       
                   CLASS_DEFAULT,                  
                   0);                        

    
  class_addmethod(filemtime_class, (t_method)filemtime_main, \
   gensym("args"), A_GIMME, 0);
  class_addmethod(filemtime_class, (t_method)filemtime_do, \
   gensym("doit"), 0);

}


