#define INCL_WIN
#include <os2.h>
#include <stdio.h>
#include <string.h>

BOOL install(char *dll, char *newclass, char *oldclass);
BOOL uninstall(char *newclass, char *oldclass);

int main(int argc, char *argv[])
{
   if (argc > 1)
   {
      char dll[2*CCHMAXPATH] = {0};
      char buffer[2*CCHMAXPATH];
      char currentdir[2*CCHMAXPATH];
      HMODULE hmodule;
      ULONG length;
      BOOL ok = 0;

      if(argc > 2)
      {
         strcpy(dll,argv[2]);
         if (*strchr(dll,'\0') - 1 != '\\')
         {
            *strchr(dll,'\0') = '\\';
            *(strchr(dll,'\0') + 1) = '\0';
         }
      }

      strcat(dll,"wpsexts");

      printf("Warning: the class will register successfully\n"
              "even if the WPS doesn't find the DLL.\n\n");

      if (argv[1][0] == 'i')
      {
         ok += install(dll,"WPSAgentSam","WPObject");
         if(ok) printf("Please reboot or restart the Workplace Shell.\n");
      }
      else if (argv[1][0] == 'u')
      {
         ok += uninstall("WPSAgentSam","WPObject");
         if(ok) printf("Please reboot or restart the Workplace Shell.\n");
      }
      else printf("use i for install and u for uninstall followed by an optional path");
   }
   else printf("use i for install and u for uninstall followed by an optional path");

   return 1;
}

BOOL install(char *dll, char *newclass, char *oldclass)
{
   if (WinRegisterObjectClass(newclass,dll) &&
       WinReplaceObjectClass(oldclass,newclass,TRUE))
   {
      printf("%s successfully registered and %s successfully subclassed.\n",newclass,oldclass);
      return TRUE;
   }
   else
   {
      printf("%s NOT successfully subclassed.\n",oldclass);
      return FALSE;
   }
}

BOOL uninstall(char *newclass, char *oldclass)
{
   if (WinReplaceObjectClass(oldclass,newclass,FALSE) &&
       WinDeregisterObjectClass(newclass))
   {
      printf("%s successfully unregistered.\n",newclass);
      return TRUE;
   }
   else
   {
      printf("%s NOT successfully unregistered.\n",newclass);
      return FALSE;
   }
}
