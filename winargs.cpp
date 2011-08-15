#include <windows.h>

int cmdline(char* command_line, char ***myArgv)
{

    int    argc;
    char** argv;

    char*  arg;
    int    index;
    int    result;

    // count the arguments
    
    argc = 1;
    arg  = command_line;
    
    while (arg[0] != 0) {

        while (arg[0] != 0 && arg[0] == ' ') {
            arg++;
        }

        if (arg[0] != 0) {
        
            argc++;
        
            while (arg[0] != 0 && arg[0] != ' ') {
                arg++;
            }
        
        }
    
    }    
    
    // tokenize the arguments

    *myArgv = argv = (char**)malloc(argc * sizeof(char*));

    arg = command_line;
    index = 1;

    while (arg[0] != 0)
      {

        while (arg[0] != 0 && arg[0] == ' ')
	  {
            arg++;
	  }
        if (arg[0] != 0)
	  {
        
            argv[index] = arg;
            index++;
        
            while (arg[0] != 0 && arg[0] != ' ')
	      {
                arg++;
	      }
            if (arg[0] != 0)
	      {
                arg[0] = 0;    
                arg++;
	      }
	  }
      }    
    char *filename = (char *)malloc(_MAX_PATH); 
    GetModuleFileName(NULL, filename, _MAX_PATH);
    argv[0] = filename;
    return argc;
}
