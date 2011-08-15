#include <windows.h>
#include <stdio.h>

static HANDLE hSerial;

void initSerial(char *dev)
{
  hSerial = CreateFile(dev,
			    GENERIC_WRITE,
			    0,
			    0,
			    OPEN_EXISTING,
			    FILE_ATTRIBUTE_NORMAL,
			    0);

     if(hSerial==INVALID_HANDLE_VALUE)
       {
	 if(GetLastError()==ERROR_FILE_NOT_FOUND)
	   {
	     printf("serial port %s does not exist\n", dev);
	   }
	 printf("something went wrong\n");
       }
     else
       {
	 printf("serial port %s seems okay\n", dev);
       }

     DCB dcbSerialParams = {0};
     dcbSerialParams.DCBlength=sizeof(dcbSerialParams);
     if (!GetCommState(hSerial, &dcbSerialParams))
       {
	 printf("error getting serial port state\n");
       }
     else
       {
	 printf("COMState gotten\n");
       }
     dcbSerialParams.BaudRate=CBR_9600;
     dcbSerialParams.ByteSize=7;
     dcbSerialParams.StopBits=ONESTOPBIT;
     dcbSerialParams.Parity=NOPARITY;

     dcbSerialParams.fOutxDsrFlow = FALSE;
     dcbSerialParams.fOutxCtsFlow = FALSE;
     dcbSerialParams.fOutX = FALSE;
     dcbSerialParams.fDtrControl =  DTR_CONTROL_DISABLE;
     dcbSerialParams.fRtsControl =  RTS_CONTROL_DISABLE;

     if(!SetCommState(hSerial, &dcbSerialParams))
       {
	 printf("error setting serial port state\n");
       }
     else
       {
	 printf("COMState set 9600 8 1 no\n");
       }
}

void sendSerial(char x)
{
  char c[10];
  DWORD dwBytesWrote;
  int i;
  dwBytesWrote = 0;
  c[0] = x;

  c[1] = (char) 0;

  if(!WriteFile(hSerial, (LPCVOID)c, (DWORD)1, &dwBytesWrote, NULL))
    {
      printf("Write serial failed (%d)\n", GetLastError());
    }
  else
    {
      printf("[%s] ",c);
    }
}

