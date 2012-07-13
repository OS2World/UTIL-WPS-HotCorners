/* Hot Corners 2.2 (C) 1997-1999 Samuel Audet <guardia@cam.org> */

#define TITLE "Hot Corners"
#define INI_FILENAME "HOTCORN.INI"
#define HLP_FILENAME "HOTCORN.HLP"
#define FONT1 "9.WarpSans"
#define FONT2 "8.Helv"
#define CLIENTCLASS "Hot Corners (C) 1997-1999 Samuel Audet"

#define INCL_PM
#define INCL_DOS
#define INCL_WINHELP
#include <os2.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "hotcorn.h"

MRESULT EXPENTRY FrameWndProc(HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2);
HWND CreateNotebook(HWND hwnd);
BOOL _System SwitchToObject(HOBJECT hobj, ULONG view, ULONG timeout, BOOL wait);
BOOL switchTo(char *list, char *liststr, WINOPT options);
BOOL refill(char *list, char *liststr, HWND *hwndlist);
BOOL openObjects(HOBJECT *objlist);
MRESULT EXPENTRY wpWindowList( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY wpStringEntryField( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY wpStringListBox( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
HOBJECT _System HObjectFromID(ULONG ulItemID, BOOL shadowresolve);
MRESULT EXPENTRY wpObjects(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY wpObjectListbox( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY wpOptions( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY wpAbout( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY wpDummy( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
BOOL refreshListbox(HWND hwnd, int input_id);
BOOL addAll(HWND hwnd, int output_id, char *input);
BOOL activateAll(HWND hwnd, int input_id, char **output);
BOOL selectActivated(HWND hwnd, int output_id, char *input);
BOOL activateSelected(HWND hwnd, int input_id, char **output);
BOOL _System QueryPathTitle(HOBJECT hobj, BOOL shadowresolve, BOOL truename, char path[]);
BOOL getObjects(HWND hwnd, int output_id, HOBJECT *objlist);
BOOL setObjects(HWND hwndinput, HOBJECT **objlist);
BOOL loadINIfile(void);
BOOL saveINIfile(void);
BOOL cleardata(void);
BOOL loadWindows(HINI INIhandle, char *app, char *section, char **output);
BOOL storeWindows(HINI INIhandle, char *app, char *section, char *input);
BOOL loadOptions(HINI INIhandle, char *app, char *section, WINOPT *options);
BOOL storeOptions(HINI INIhandle, char *app, char *section, WINOPT options);
BOOL loadObjects(HINI INIhandle, char *app, char *section, HOBJECT **objlist);
BOOL storeObjects(HINI INIhandle, char *app, char *section, HOBJECT *objlist);
char *getINIfile(char *path);
char *getEXEpath(void);
void _System ExeTrap(void);

/* application wide used variables */
HAB hab;
char *EXEpath, *INIfile;

/* font to be used as system font */
char *font;

/* for the window sizes */
#define FRAME 0
#define SAVEPB 1
#define RELOADPB 2
#define CLEARPB 3
#define HELPPB 4

#define PBNUM 5

POINTL size[PBNUM];

/* error display handling */
char errormsg[100]; /* error message */
int errornum;       /* error counter */
RECTL location;     /* error message location in window */

/* activation options */
ACTOPT actopt = {0};

/* Arrays of pointers to the corresponding window list entry
   for activating hot borders and corners.  */
char *up[ACTOPTCOUNT]    = {0}, *upperleft[ACTOPTCOUNT]  = {0},
     *down[ACTOPTCOUNT]  = {0}, *lowerleft[ACTOPTCOUNT]  = {0},
     *left[ACTOPTCOUNT]  = {0}, *upperright[ACTOPTCOUNT] = {0},
     *right[ACTOPTCOUNT] = {0}, *lowerright[ACTOPTCOUNT] = {0},
     *floattop[ACTOPTCOUNT] = {0}, *floatbottom[ACTOPTCOUNT] = {0};

char **curlist = upperleft;

/* contains substrings of window list that the user types in */
char *upstr[ACTOPTCOUNT]    = {0}, *upperleftstr[ACTOPTCOUNT]  = {0},
     *downstr[ACTOPTCOUNT]  = {0}, *lowerleftstr[ACTOPTCOUNT]  = {0},
     *leftstr[ACTOPTCOUNT]  = {0}, *upperrightstr[ACTOPTCOUNT] = {0},
     *rightstr[ACTOPTCOUNT] = {0}, *lowerrightstr[ACTOPTCOUNT] = {0},
     *floattopstr[ACTOPTCOUNT] = {0}, *floatbottomstr[ACTOPTCOUNT] = {0};

char **curliststr = upperleftstr;

/* pointers to the variables in the DLL, set by HookInit() */
HWNDOPT *hwndopt; /* window to minimize/push back on leave */
HWND *hwndtop, *hwndbottom; // windows to make float at bottom or top
BOOL *openontop; // used in a timer to open all windows on top in a given interval
SHORT *margin; /* corner margin */

/* window list window options */
WINOPT  upopt[ACTOPTCOUNT] = {0}, upperleftopt[ACTOPTCOUNT] = {0},
      downopt[ACTOPTCOUNT] = {0}, lowerleftopt[ACTOPTCOUNT] = {0},
      leftopt[ACTOPTCOUNT] = {0}, upperrightopt[ACTOPTCOUNT] = {0},
     rightopt[ACTOPTCOUNT] = {0}, lowerrightopt[ACTOPTCOUNT] = {0},
     floattopopt[ACTOPTCOUNT] = {0}, floatbottomopt[ACTOPTCOUNT] = {0};

WINOPT *curopt = upperleftopt;

/* array of pointers to objects for the object activation */
HOBJECT *oup[ACTOPTCOUNT]    = {0}, *oupperleft[ACTOPTCOUNT]  = {0},
        *odown[ACTOPTCOUNT]  = {0}, *olowerleft[ACTOPTCOUNT]  = {0},
        *oleft[ACTOPTCOUNT]  = {0}, *oupperright[ACTOPTCOUNT] = {0},
        *oright[ACTOPTCOUNT] = {0}, *olowerright[ACTOPTCOUNT] = {0};

ULONG **curobj = oupperleft;

/* timeout delay that needs to be achieved to start operations */
int delay = 300;

/* used in the substring mechanism to catch Enter and Delete keys, and also drag and drop */
FNWP *WinOldStringEntryFieldWindowProc;
FNWP *WinOldStringListBoxWindowProc;

/* keeps a pointer to the currently loaded switch list in Hot Corners */
SWBLOCK *switchlist = NULL;

/* main window handles */
HWND hwndFrame, hwndClient, hwndHelpInstance;

/* needed to bring windows at the mouse position */
POINTL screen;

/* if we can't load the HELP file */
BOOL helpenabled = FALSE;

/* notebook page information, see hotcorn.h */

NBPAGE nbpage[] =
{
   {wpWindowList, "Tab 1 of 4", "~Window List", ID_WINDOWLISTPAGE, FALSE, BKA_MAJOR, 0, 0},
   {wpObjects,    "Tab 2 of 4", "~Objects",     ID_OBJECTPAGE,     FALSE, BKA_MAJOR, 0, 0},
   {wpOptions,    "Tab 3 of 4", "O~ptions",     ID_OPTIONPAGE,     FALSE, BKA_MAJOR, 0, 0},
   {wpAbout,      "Tab 4 of 4", "~About",       ID_ABOUTPAGE,      FALSE, BKA_MAJOR, 0, 0}
};

/* some constants so that we don't have to modify code if we add pages */
#define PAGE_COUNT (sizeof( nbpage ) / sizeof( NBPAGE ))
#define PAGE_WINDOWLIST 0
#define PAGE_OBJECTS    1
#define PAGE_OPTIONS    2
#define PAGE_ABOUT      3


int main()
{
   HMQ hmq;

   ULONG frameFlags = FCF_TITLEBAR | FCF_SYSMENU | FCF_MINBUTTON |/* FCF_MAXBUTTON | */
                      /* FCF_SIZEBORDER  | FCF_MENU | */ FCF_ACCELTABLE | FCF_ICON |
                      /* FCF_SHELLPOSITION | */ FCF_TASKLIST | FCF_DLGBORDER;
   QMSG qmsg;
   HEV hev;

   HPS hps;
   LONG fontcounter = 0;

   /* if this gets the value of NULL, the program should use
      the current dir, ie.: no path assignment before filename */
   EXEpath = getEXEpath();
   /* this adds INI_FILENAME to the previous */
   INIfile = getINIfile(EXEpath);

   /* find screen width once and for all */
   screen.x = WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
   screen.y = WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);

   /* window size in dialog units */
   size[FRAME].x = 295;   size[FRAME].y = 255;

   /* button size in dialog units */
   size[SAVEPB].x = 30;   size[SAVEPB].y = 14;
   size[RELOADPB].x = 36; size[RELOADPB].y = 14;
   size[CLEARPB].x = 31; size[CLEARPB].y = 14;
   size[HELPPB].x = 30; size[HELPPB].y = 14;

   /* convert these sizes in pixel */
   WinMapDlgPoints(HWND_DESKTOP, size, PBNUM, TRUE);

   /* init of error display message handling in the main window*/
   errornum = 1;
   strcpy(errormsg,"");
   location.xRight = 500; /* just enough */

   /* locate error message after the buttons and at the same height */
   location.xLeft = 5+size[SAVEPB].x+5+size[RELOADPB].x+5+size[CLEARPB].x+5+size[HELPPB].x+5;
   location.yBottom = 5;
   location.yTop = 5+3*size[SAVEPB].y/4;

   /* decide which font to use */
   hps = WinGetPS(HWND_DESKTOP);
   if(GpiQueryFonts(hps, QF_PUBLIC,strchr(FONT1,'.')+1, &fontcounter, 0, NULL))
      font = FONT1;
   else
      font = FONT2;
   WinReleasePS(hps);

#if 0
   if(DosCreateEventSem("\\SEM32\\HOTCORNERS\\RUNNING.SEM",&hev,0,FALSE))
   {
      DosBeep(600,200);
      sprintf(errormsg,"%d. Warning, already running",errornum++);
   }
#endif

   /* initialize PM */
   hab = WinInitialize(0);

   hmq = WinCreateMsgQueue(hab, 0);

   WinRegisterClass( hab, CLIENTCLASS, FrameWndProc,
                     CS_CLIPCHILDREN | CS_SIZEREDRAW, 0 );

   hwndFrame = WinCreateStdWindow (HWND_DESKTOP, 0, &frameFlags, CLIENTCLASS,
                                   TITLE, 0, 0, ID_MAIN, &hwndClient);

   if (hwndFrame)
   {
      SWP pos;
      HINI INIhandle;
      ULONG state;

      /* get window position from INI file */
      if (INIhandle = PrfOpenProfile(hab,INIfile))
      {
         ULONG datasize;

         PrfQueryProfileSize(INIhandle, "Position", "state", &datasize);
            if (datasize == sizeof(state))
               PrfQueryProfileData(INIhandle, "Position", "state", &state, &datasize);
            else state = 0;

         PrfQueryProfileSize(INIhandle, "Position", "x", &datasize);
            if (datasize == sizeof(pos.x))
               PrfQueryProfileData(INIhandle, "Position", "x", &pos.x, &datasize);
            else pos.x = 30;

         PrfQueryProfileSize(INIhandle, "Position", "y", &datasize);
            if (datasize == sizeof(pos.y))
               PrfQueryProfileData(INIhandle, "Position", "y", &pos.y, &datasize);
            else pos.y = 30;

         PrfCloseProfile(INIhandle);
      }

      /* set window position */
      WinSetWindowPos( hwndFrame, NULLHANDLE,
                       pos.x, pos.y, size[FRAME].x, size[FRAME].y,
                       SWP_SIZE | SWP_MOVE | SWP_SHOW | state );

      /* if the EXE is killed instead of closed, this insures the hook release */
      DosExitList(EXLST_ADD, (PFNEXITLIST)ExeTrap);

      while( WinGetMsg( hab, &qmsg, 0L, 0, 0 ) )
         WinDispatchMsg( hab, &qmsg );

      HookKill();
      WinDestroyHelpInstance(hwndHelpInstance);
      DosCloseEventSem(hev);

      /* save window position in INI file */
      if (INIhandle = PrfOpenProfile(hab,INIfile))
      {
         if (WinQueryWindowPos(hwndFrame, &pos))
         {
            state = 0;
            if (pos.fl & SWP_MINIMIZE)
            {
               pos.x = (LONG) WinQueryWindowUShort(hwndFrame, QWS_XRESTORE);
               pos.y = (LONG) WinQueryWindowUShort(hwndFrame, QWS_YRESTORE);
               state = SWP_MINIMIZE;
            }
            else if (pos.fl & SWP_MAXIMIZE)
            {
               pos.x = (LONG) WinQueryWindowUShort(hwndFrame, QWS_XRESTORE);
               pos.y = (LONG) WinQueryWindowUShort(hwndFrame, QWS_YRESTORE);
               state = SWP_MAXIMIZE;
            }

            PrfWriteProfileData(INIhandle, "Position","state", &state, sizeof(state));
            PrfWriteProfileData(INIhandle, "Position","x", &pos.x, sizeof(pos.x));
            PrfWriteProfileData(INIhandle, "Position","y", &pos.y, sizeof(pos.y));
         }
         PrfCloseProfile(INIhandle);
      }

      WinDestroyWindow(hwndFrame);        /* Tidy up...                    */
   }
   WinDestroyMsgQueue( hmq );             /* Tidy up...                    */
   WinTerminate( hab );                   /* Terminate the application     */

   return 1;
}

MRESULT EXPENTRY FrameWndProc(HWND hwnd,ULONG msg,MPARAM mp1,MPARAM mp2)
{
   static char curnumber = 0; /* we save which activation option
                                 should be used since sometimes
                                 we have to wait after a timer  */
   switch (msg)
   {

   case WM_USER:

      curnumber = CHAR1FROMMP(mp2);

      if(SHORT2FROMMP(mp1) == ID_STOPTIMER)
      {
         switch (SHORT1FROMMP(mp1))
         {
            case ID_UPPERLEFT:
            case ID_LOWERLEFT:
            case ID_UPPERRIGHT:
            case ID_LOWERRIGHT:

            case ID_UP:
            case ID_DOWN:
            case ID_LEFT:
            case ID_RIGHT:

               WinStopTimer(hab,hwnd,SHORT1FROMMP(mp1));   break;
         }
         return 0;
      }

      else if(SHORT2FROMMP(mp1) == ID_NOTIMER); /* jumps to "case WM_TIMER:" */

      else
      {
         WinStopTimer(hab,hwnd,ID_UPPERLEFT);
         WinStopTimer(hab,hwnd,ID_LOWERLEFT);
         WinStopTimer(hab,hwnd,ID_UPPERRIGHT);
         WinStopTimer(hab,hwnd,ID_LOWERRIGHT);

         WinStopTimer(hab,hwnd,ID_UP);
         WinStopTimer(hab,hwnd,ID_DOWN);
         WinStopTimer(hab,hwnd,ID_LEFT);
         WinStopTimer(hab,hwnd,ID_RIGHT);

         switch (SHORT1FROMMP(mp1))
         {
            case ID_UPPERLEFT:
            case ID_LOWERLEFT:
            case ID_UPPERRIGHT:
            case ID_LOWERRIGHT:

            case ID_UP:
            case ID_DOWN:
            case ID_LEFT:
            case ID_RIGHT:

               WinStartTimer(hab,hwnd,SHORT1FROMMP(mp1),delay); break;

            case ID_FLOATTOP:    refill(floattop[0],floattopstr[0],hwndtop); break;
            case ID_FLOATBOTTOM: refill(floatbottom[0],floatbottomstr[0],hwndbottom); break;

/*               WinStartTimer(hab,hwnd,SHORT1FROMMP(mp1),1000); break; */
         }
         return 0;
      }

   case WM_TIMER:

      switch (SHORT1FROMMP(mp1))
      {
         case ID_UPPERLEFT:  WinStopTimer(hab,hwnd,SHORT1FROMMP(mp1)); switchTo(upperleft[curnumber],upperleftstr[curnumber],upperleftopt[curnumber]);
                                 openObjects(oupperleft[curnumber]); break;
         case ID_LOWERLEFT:  WinStopTimer(hab,hwnd,SHORT1FROMMP(mp1)); switchTo(lowerleft[curnumber],lowerleftstr[curnumber],lowerleftopt[curnumber]);
                                 openObjects(olowerleft[curnumber]); break;
         case ID_UPPERRIGHT: WinStopTimer(hab,hwnd,SHORT1FROMMP(mp1)); switchTo(upperright[curnumber],upperrightstr[curnumber],upperrightopt[curnumber]);
                                 openObjects(oupperright[curnumber]); break;
         case ID_LOWERRIGHT: WinStopTimer(hab,hwnd,SHORT1FROMMP(mp1)); switchTo(lowerright[curnumber],lowerrightstr[curnumber],lowerrightopt[curnumber]);
                                 openObjects(olowerright[curnumber]); break;

         case ID_UP:         WinStopTimer(hab,hwnd,SHORT1FROMMP(mp1)); switchTo(up[curnumber],upstr[curnumber],upopt[curnumber]);
                                 openObjects(oup[curnumber]); break;
         case ID_DOWN:       WinStopTimer(hab,hwnd,SHORT1FROMMP(mp1)); switchTo(down[curnumber],downstr[curnumber],downopt[curnumber]);
                                 openObjects(odown[curnumber]); break;
         case ID_LEFT:       WinStopTimer(hab,hwnd,SHORT1FROMMP(mp1)); switchTo(left[curnumber],leftstr[curnumber],leftopt[curnumber]);
                                 openObjects(oleft[curnumber]); break;
         case ID_RIGHT:      WinStopTimer(hab,hwnd,SHORT1FROMMP(mp1)); switchTo(right[curnumber],rightstr[curnumber],rightopt[curnumber]);
                                 openObjects(oright[curnumber]); break;

/*         case ID_FLOATTOP:    WinStopTimer(hab,hwnd,SHORT1FROMMP(mp1));
                              refill(floattop[0],floattopstr[0],hwndtop); break;
         case ID_FLOATBOTTOM: WinStopTimer(hab,hwnd,SHORT1FROMMP(mp1));
                              refill(floatbottom[0],floatbottomstr[0],hwndbottom); break; */

         case ID_OPENONTOP: *openontop = FALSE; break;
      }
      return 0;

   case WM_CHAR:
      if (!(SHORT1FROMMP(mp1) & KC_KEYUP))
      {
         /* implement tab and arrow movements around the dialog items */
         if (SHORT1FROMMP(mp1) & KC_VIRTUALKEY)
            switch(SHORT2FROMMP(mp2))
            {
               case VK_RIGHT:
                  WinSetFocus(HWND_DESKTOP,WinEnumDlgItem(hwnd,WinQueryFocus(HWND_DESKTOP), EDI_NEXTGROUPITEM));
                  break;
               case VK_LEFT:
                  WinSetFocus(HWND_DESKTOP,WinEnumDlgItem(hwnd,WinQueryFocus(HWND_DESKTOP), EDI_PREVGROUPITEM));
                  break;
               case VK_DOWN:
                  WinSetFocus(HWND_DESKTOP,WinEnumDlgItem(hwnd,WinQueryFocus(HWND_DESKTOP), EDI_NEXTGROUPITEM));
                  break;
               case VK_UP:
                  WinSetFocus(HWND_DESKTOP,WinEnumDlgItem(hwnd,WinQueryFocus(HWND_DESKTOP), EDI_PREVGROUPITEM));
                  break;
               case VK_TAB:
                  WinSetFocus(HWND_DESKTOP,WinEnumDlgItem(hwnd,WinQueryFocus(HWND_DESKTOP), EDI_NEXTTABITEM));
                  break;
               case VK_BACKTAB:
                  WinSetFocus(HWND_DESKTOP,WinEnumDlgItem(hwnd,WinQueryFocus(HWND_DESKTOP), EDI_PREVTABITEM));
                  break;
            }

         /* match mnemonic of buttons, and send BM_CLICK */
         else if (SHORT1FROMMP(mp1) & KC_CHAR)
         {
            HWND itemhwnd = WinEnumDlgItem(hwnd, 0, EDI_FIRSTTABITEM);

            do if ((BOOL)WinSendMsg(itemhwnd, WM_MATCHMNEMONIC, mp2, 0))
                  WinSendMsg(itemhwnd, BM_CLICK, SHORT1FROMMP(FALSE), 0);
            while((itemhwnd = WinEnumDlgItem(hwnd, itemhwnd, EDI_NEXTTABITEM))
                   != WinEnumDlgItem(hwnd, 0, EDI_FIRSTTABITEM));
         }
      }
      break;

   case WM_COMMAND:
   {
      switch (SHORT1FROMMP(mp1))
      {
         /* reload info from INI file */
         case ID_RELOADPB:

            if(loadINIfile())
            {
               /* refresh pages with new data */
               if (nbpage[PAGE_WINDOWLIST].hwnd)
                  WinSendMsg(nbpage[PAGE_WINDOWLIST].hwnd, WM_USER, 0, 0);

               if (nbpage[PAGE_OBJECTS].hwnd)
                  WinSendMsg(nbpage[PAGE_OBJECTS].hwnd, WM_USER, 0, 0);

               if (nbpage[PAGE_OPTIONS].hwnd)
                  WinSendMsg(nbpage[PAGE_OPTIONS].hwnd, WM_USER, 0, 0);
            }
            break;

         case ID_SAVEPB:

            saveINIfile();
            break;

         case ID_CLEARPB:

            if(cleardata())
            {
               /* refresh pages with new data */
               if (nbpage[PAGE_WINDOWLIST].hwnd)
                  WinSendMsg(nbpage[PAGE_WINDOWLIST].hwnd, WM_USER, 0, 0);

               if (nbpage[PAGE_OBJECTS].hwnd)
                  WinSendMsg(nbpage[PAGE_OBJECTS].hwnd, WM_USER, 0, 0);

               if (nbpage[PAGE_OPTIONS].hwnd)
                  WinSendMsg(nbpage[PAGE_OPTIONS].hwnd, WM_USER, 0, 0);
            }
            break;

         /* pseudo alt + mnemonics */
         case ID_WINDOWLISTPAGE:
            WinSendDlgItemMsg(hwnd, ID_NB, BKM_TURNTOPAGE,
                    MPFROMLONG(nbpage[PAGE_WINDOWLIST].ulPageId), 0);
            WinSetFocus(HWND_DESKTOP,WinWindowFromID(hwnd,ID_NB));
            break;
         case ID_OBJECTPAGE:
            WinSendDlgItemMsg(hwnd, ID_NB, BKM_TURNTOPAGE,
                    MPFROMLONG(nbpage[PAGE_OBJECTS].ulPageId), 0);
            WinSetFocus(HWND_DESKTOP,WinWindowFromID(hwnd,ID_NB));
            break;
         case ID_OPTIONPAGE:
            WinSendDlgItemMsg(hwnd, ID_NB, BKM_TURNTOPAGE,
                    MPFROMLONG(nbpage[PAGE_OPTIONS].ulPageId), 0);
            WinSetFocus(HWND_DESKTOP,WinWindowFromID(hwnd,ID_NB));
            break;
         case ID_ABOUTPAGE:
            WinSendDlgItemMsg(hwnd, ID_NB, BKM_TURNTOPAGE,
                    MPFROMLONG(nbpage[PAGE_ABOUT].ulPageId), 0);
            break;

         /* alt-arrows up and down */
         case ID_NBFOCUS:
            WinSetFocus(HWND_DESKTOP,WinWindowFromID(hwnd,ID_NB));
            break;
         case ID_SAVEFOCUS:
            WinSetFocus(HWND_DESKTOP,WinWindowFromID(hwnd,ID_SAVEPB));
            break;
      }
      return 0;
   }

   case WM_CREATE:
   {
      HWND hwndNB;
      HELPINIT hini;

      WinSetPresParam(hwnd, PP_FONTNAMESIZE, strlen(font)+1, font);

      /* load hook in DLL */
      HookInit(hwnd,&hwndopt,&margin,&hwndtop,&hwndbottom,&openontop);

      hwndNB = CreateNotebook(hwnd);
      if(!hwndNB) return (MRESULT) TRUE;

      hini.cb = sizeof(hini);
      hini.ulReturnCode = 0;
      hini.pszTutorialName = NULL;
      hini.phtHelpTable = (HELPTABLE *)MAKEULONG(MAINTABLE,0xFFFF);
      hini.hmodHelpTableModule = 0;
      hini.hmodAccelActionBarModule = 0;
      hini.idAccelTable = 0;
      hini.idActionBar = 0;
      hini.pszHelpWindowTitle = TITLE;
      hini.fShowPanelId = CMIC_HIDE_PANEL_ID /* CMIC_SHOW_PANEL_ID */;
      hini.pszHelpLibraryName = HLP_FILENAME;

      hwndHelpInstance = WinCreateHelpInstance(hab, &hini);
      if(!hini.ulReturnCode && WinAssociateHelpInstance(hwndHelpInstance,hwnd))
         helpenabled = TRUE;
      else
      {
         helpenabled = FALSE;
         sprintf(errormsg,"%d. Cannot find %s, help disabled.",errornum++,hini.pszHelpLibraryName);
         WinInvalidateRect(hwndClient, &location, FALSE );
      }

      loadINIfile();

      /* turn to first notebook page */
      WinSendMsg( hwndNB, BKM_TURNTOPAGE, MPFROMLONG(

         (ULONG) WinSendMsg(hwndNB, BKM_QUERYPAGEID, NULL,
                MPFROM2SHORT(BKA_FIRST, BKA_MAJOR))
                                                              ), NULL );

      return 0;
   }

   case WM_HELP:

      if(helpenabled)
      {
         HWND hwndfocus = WinQueryFocus(HWND_DESKTOP);
         HWND hwndNB = WinWindowFromID(hwnd,ID_NB);
         int i;

         /* this is useful when the user presses Help button and the focus is on
            one of the notebook dialog controls, or on the notebook tabs themselves */

         if(hwndfocus == hwndNB)
         {
   /*       WinSendMsg(hwndNB, WM_HELP, 0, 0); */ /* brings back a BKN_HELP */

            WinSendMsg(
               LONGFROMMR(WinSendMsg(hwndNB, BKM_QUERYPAGEWINDOWHWND,
                  WinSendMsg(hwndNB,BKM_QUERYPAGEID,0,MPFROMSHORT(BKA_TOP))
                                                        , NULL))
                                                           ,WM_HELP,0,0);
            return 0; /* prevents the notebook unfriendly help manager to interfere */
         }

         for(i = 0; i < PAGE_COUNT; i++)
            if(WinIsChild(hwndfocus,nbpage[i].hwnd))
            {
               WinSendMsg(nbpage[i].hwnd,WM_HELP,0,0);
               return 0; /* prevents the notebook unfriendly help manager to interfere */
            }
      }

      break; /* if focus is not on notebook controls or the notebook,
                let the usual help mechanism handle it */

   /* received from the notebook window, allows loading and refreshing of pages */
   case WM_CONTROL:
   {
      switch(SHORT2FROMMP(mp1))
      {

      /* this is useful when the user presses F1 on a notebook tab */
      case BKN_HELP:

         if(helpenabled)
            WinSendMsg(LONGFROMMR(WinSendMsg(WinWindowFromID(hwnd,ID_NB),
                       BKM_QUERYPAGEWINDOWHWND, mp2 /* this is ulPageId */, NULL))
                                                     ,WM_HELP,0,0);
         return 0;

      case BKN_PAGESELECTEDPENDING:
      {
         PAGESELECTNOTIFY *pageselect = PVOIDFROMMP(mp2);
         /* get page window handle */
         HWND FrameHwnd = (HWND) LONGFROMMR(WinSendMsg(pageselect->hwndBook, BKM_QUERYPAGEWINDOWHWND,
                                    MPFROMLONG(pageselect->ulPageIdNew), NULL));

         NBPAGE *pagedata = (NBPAGE *) PVOIDFROMMR(WinSendMsg(pageselect->hwndBook, BKM_QUERYPAGEDATA,
                              MPFROMLONG(pageselect->ulPageIdNew), NULL));

         if (!pagedata) return 0;

         /* skip dummy (major) pages that we want to skip to only show their minor pages */
         if(pagedata->skip)
         {
            ULONG ulPageNext = (ULONG) WinSendMsg( pageselect->hwndBook, BKM_QUERYPAGEID,
                                        MPFROMLONG( pageselect->ulPageIdNew ),
                                        MPFROM2SHORT( BKA_NEXT, 0 ) );

            /* if the next page was the previously viewed page, means the user is
               going backward, so we show the previous page */
            if(ulPageNext == pageselect->ulPageIdCur)
               WinSendMsg(pageselect->hwndBook, BKM_TURNTOPAGE, MPFROMLONG(

                   (ULONG) WinSendMsg(pageselect->hwndBook, BKM_QUERYPAGEID,
                      MPFROMLONG(pageselect->ulPageIdNew), MPFROM2SHORT(BKA_PREV, 0))

                                         ), NULL);
            /* else we want to show the next page */
            else
               WinSendMsg( pageselect->hwndBook, BKM_TURNTOPAGE, MPFROMLONG(ulPageNext), NULL);

            /* prevents the current page from showing at all */
            pageselect->ulPageIdNew = 0;
            break;
         }
         else if (!FrameHwnd)
         {
            HELPINIT hini;
            HWND hwndHelpInstance;
            BOOL rc;
            /* loading dialog window */
            pagedata->hwnd = WinLoadDlg(pageselect->hwndBook, pageselect->hwndBook,
                                        pagedata->pfnwpDlg, 0, pagedata->idDlg, NULL);
            /* set font */
            WinSetPresParam(pagedata->hwnd, PP_FONTNAMESIZE, strlen(font)+1, font);

            /* associate window with notebook page */
            WinSendMsg(pageselect->hwndBook, BKM_SETPAGEWINDOWHWND,
                       MPFROMLONG(pageselect->ulPageIdNew), MPFROMHWND(pagedata->hwnd));

            /* I want the focus on the notebook tab after load */
            WinSetFocus(HWND_DESKTOP, pageselect->hwndBook);
         }
         else
            /* send refresh each time there is a page switch */
            WinSendMsg(FrameHwnd, WM_USER, 0, 0);
         break;
      }
      }
      return 0;
   }

   case WM_ERASEBACKGROUND:
   {
      /* fill leftout (see WM_SIZE) background with gray color */
      WinFillRect(HWNDFROMMP(mp1), PVOIDFROMMP(mp2), SYSCLR_FIELDBACKGROUND);
      return 0;
   }

   case WM_PAINT:
   {
      /* display error messages */
      HPS hps = WinGetPS(hwnd);
      WinFillRect(hps, &location, SYSCLR_FIELDBACKGROUND);
      if (errormsg[0])
         WinDrawText(hps, -1, errormsg, &location, CLR_RED, BM_LEAVEALONE, 0);

      WinReleasePS(hps);
      break; /* to let the default window procedure draw the notebook */
   }

   case WM_BUTTON1CLICK:
      /* erase error message */
      if( (SHORT1FROMMP(mp1) > location.xLeft) &&
          (SHORT1FROMMP(mp1) < location.xRight) &&
          (SHORT2FROMMP(mp1) > location.yBottom) &&
          (SHORT2FROMMP(mp1) < location.yTop) )
      {
         errormsg[0] = '\0';
         WinInvalidateRect( hwnd, &location, FALSE );
      }
      break;

   case WM_SIZE:
   {
      /* position notebook in the main client window over the buttons */
      WinSetWindowPos(WinWindowFromID(hwnd, ID_NB), 0,
                                        5, /* x position */
                        size[SAVEPB].y+10, /* y position */
                     SHORT1FROMMP(mp2)-10, /* x width */
      SHORT2FROMMP(mp2)-size[SAVEPB].y-15, /* y width */
      SWP_SIZE | SWP_SHOW | SWP_MOVE);

      return 0;
   }

   case WM_MINMAXFRAME:
   {
      /* make the app just behind us active when we are minimizing */
      SWP *swp = PVOIDFROMMP(mp1);
      if (swp->fl & SWP_MINIMIZE)
      /* first gets the next frame on top (frame behind this frame, this API
         is a tricky one) then sets that window active */
         WinSetActiveWindow(HWND_DESKTOP, WinQueryWindow(hwnd, QW_NEXTTOP));
      return 0;
   }

#ifdef crazy

      case WM_USER+1:
      {
         FILE* file;
         file = fopen("f:\\loggy","a");
         if(LONGFROMMP(mp2) == 0x666)
            fprintf(file,"hwnd created: %x\n",LONGFROMMP(mp1));
         else if(LONGFROMMP(mp2) == 0x6666)
            fprintf(file,"hwnd moving: %x ",LONGFROMMP(mp1));
         else
            fprintf(file,"hwndInsertBehind: %x  flags: %x\n",LONGFROMMP(mp1),LONGFROMMP(mp2));
         fclose(file);
         break;
      }
#endif
   case ID_SETZORDER:
         WinSetWindowPos(HWNDFROMMP(mp1),HWNDFROMMP(mp2),0,0,0,0,SWP_ZORDER);
         break;
   case ID_SETACTIVEWINDOW:
         WinSetActiveWindow(HWND_DESKTOP,HWNDFROMMP(mp1));
         break;

   }

   return (MRESULT)   WinDefWindowProc(hwnd,msg,mp1,mp2);
}

/* switch to app titles the are in selectionList, accepts null terminated "linear arrays" */
BOOL switchTo(char *list, char *liststr, WINOPT options)
{
   int i,e;  /* loop index */
   char *listpos, *liststrpos;

   /* switch list stuff */
   ULONG count,buffersize;

   POINTL mouse,pos;
   SWP frame;

   count = WinQuerySwitchList(hab,NULL,0);
   buffersize = count * sizeof(SWENTRY) + sizeof(ULONG);
   if(_msize(switchlist) < buffersize)
   {
      free(switchlist);
      switchlist = (PSWBLOCK) malloc(2*buffersize);
   }
   count = WinQuerySwitchList(hab,switchlist,buffersize);
   if (switchlist)
   {
      /* go into all switch list items */
      for (i = 0 ; i < count ; i++)
      {
         /* look into all the user specified strings if some matches with an app title */
         listpos = list;
         liststrpos = liststr;
         while((listpos && *listpos) || (liststrpos && *liststrpos))
         {
            if ((listpos && *listpos && !strcmp(switchlist->aswentry[i].swctl.szSwtitle, listpos)) ||
                (liststrpos && *liststrpos && strstr(switchlist->aswentry[i].swctl.szSwtitle, liststrpos)))
            {
               /* switch */
               WinSwitchToProgram(switchlist->aswentry[i].hswitch);
               /* the rest can only apply to PM windows */
               if(WinQueryWindowPos(switchlist->aswentry[i].swctl.hwnd,&frame))
               {
                  /* if hidden, show it */
                  if(frame.fl & SWP_HIDE)
                     WinSetWindowPos(switchlist->aswentry[i].swctl.hwnd, 0, 0, 0, 0, 0, SWP_SHOW);

                  /* bring to mouse position */
                  if (options.move)
                  {
                     WinQueryPointerPos(HWND_DESKTOP,&mouse);
                     pos.x = mouse.x - frame.cx/2;
                     pos.y = mouse.y - frame.cy/2;
                     if(pos.x + frame.cx > screen.x) pos.x = screen.x - frame.cx;
                     if(pos.y + frame.cy > screen.y) pos.y = screen.y - frame.cy;
                     if(pos.x < 0) pos.x = 0;
                     if(pos.y < 0) pos.y = 0;
                     WinSetWindowPos(switchlist->aswentry[i].swctl.hwnd, 0, pos.x, pos.y, 0, 0, SWP_MOVE);
                  }

                  /* include window hwnd in the "minimize/push back on pointer leave" array */
                  if(options.minimize || options.pushback)
                  {
                     for(e = 0; hwndopt[e].hwnd ; e++) /* find the end */
                     memset(&hwndopt[e+1],0,sizeof(hwndopt[e+1]));
                     hwndopt[e].hwnd = switchlist->aswentry[i].swctl.hwnd;
                     hwndopt[e].wasinframe = 0;
                     hwndopt[e].minimize = options.minimize;
                     hwndopt[e].pushback = options.pushback;
                  }
               }
            }

            if(listpos && *listpos) listpos = strchr(listpos,'\0') + 1;
            if(liststrpos && *liststrpos) liststrpos = strchr(liststrpos,'\0') + 1;
         }
      }
      return TRUE;
   }
   return FALSE;
}

/* finds the HWND from the titles in list and liststr and puts them in hwndlist */
/* also places the windows in zorder */
BOOL refill(char *list, char *liststr, HWND *hwndlist)
{
   int i;  /* loop index */
   char *listpos, *liststrpos;
   HWND *hwndlistpos = hwndlist + 1; /* don't erase hwndlist[0], see HookInit() */

   /* switch list stuff */
   ULONG count,buffersize;

   POINTL mouse,pos;
   SWP frame;

   count = WinQuerySwitchList(hab,NULL,0);
   buffersize = count * sizeof(SWENTRY) + sizeof(ULONG);
   if(_msize(switchlist) < buffersize)
   {
      free(switchlist);
      switchlist = (PSWBLOCK) malloc(2*buffersize);
   }
   count = WinQuerySwitchList(hab,switchlist,buffersize);
   if (switchlist)
   {
      /* go into all switch list items */
      for (i = 0 ; i < count ; i++)
      {
         /* look into all the user specified strings if some matches with an app title */
         listpos = list;
         liststrpos = liststr;
         while((listpos && *listpos) || (liststrpos && *liststrpos))
         {
            if ((listpos && *listpos && !strcmp(switchlist->aswentry[i].swctl.szSwtitle, listpos)) ||
                (liststrpos && *liststrpos && strstr(switchlist->aswentry[i].swctl.szSwtitle, liststrpos)))
            {
               *hwndlistpos = switchlist->aswentry[i].swctl.hwnd;
               WinSetWindowPos(*hwndlistpos,*(hwndlistpos-1),0,0,0,0,SWP_ZORDER);
               hwndlistpos++;
            }

            if(listpos && *listpos) listpos = strchr(listpos,'\0') + 1;
            if(liststrpos && *liststrpos) liststrpos = strchr(liststrpos,'\0') + 1;
         }
      }
      *hwndlistpos = NULLHANDLE;
      return TRUE;
   }
   return FALSE;
}


BOOL openObjects(HOBJECT *objlist)
{
   int i;     /* loop index */
   int objnum = _msize(objlist)/sizeof(HOBJECT);  /* number of objects in objlist */

   // using some other kludge than a WPS class now, see below
#if 0
   /* go through every hobj found for this listbox */
   for (i = 0 ; i < objnum ; i++)
      if(WinSetObjectData(objlist[i],"OPEN=DEFAULT"))
         SwitchToObject(objlist[i], 0, 10, FALSE);
#endif

   for (i = 0 ; i < objnum ; i++)
      WinSetObjectData(objlist[i],"OPEN=DEFAULT");

   *openontop = TRUE;
   WinStartTimer(hab,hwndClient,ID_OPENONTOP,3000); // 3 seconds?

   return TRUE;
}

HWND CreateNotebook(HWND hwnd)
{
   HWND hwndNB;
   ULONG xspace;  /* the space at the left for the next button */
   int i;

   /* Create buttons */

   /* since I'm not in a dialog box, the tab are not automatically prosseced,
      and it seems WinEnumDlgItem processes the groups backward from their load, so
      I load them from the last to first one of the group.  Warp 4 style notebook
      processes these buttons at one time when tabbing around, but Warp 3 style
      does not, but it doesn't really matter, since we can still navigate with
      alt-arrows accelerator keys I have implented */

   xspace = 5 + size[SAVEPB].x + 5 + size[RELOADPB].x + 5 + size[CLEARPB].x + 5;
   WinCreateWindow(hwnd, WC_BUTTON, "~Help", BS_PUSHBUTTON | WS_TABSTOP | WS_VISIBLE | BS_HELP | BS_NOPOINTERFOCUS,
                   xspace, 5, size[HELPPB].x, size[HELPPB].y, hwnd, HWND_TOP, ID_HELPPB, NULL, NULL);

   xspace -= size[CLEARPB].x + 5;
   WinCreateWindow(hwnd, WC_BUTTON, "~Clear", BS_PUSHBUTTON | WS_TABSTOP | WS_VISIBLE,
                   xspace, 5, size[CLEARPB].x, size[CLEARPB].y, hwnd, HWND_TOP, ID_CLEARPB, NULL, NULL);

   xspace -= size[RELOADPB].x + 5;
   WinCreateWindow(hwnd, WC_BUTTON, "~Reload", BS_PUSHBUTTON | WS_TABSTOP | WS_VISIBLE,
                   xspace, 5, size[RELOADPB].x, size[RELOADPB].y, hwnd, HWND_TOP, ID_RELOADPB, NULL, NULL);

   xspace -= size[SAVEPB].x + 5;
   WinCreateWindow(hwnd, WC_BUTTON, "~Save", BS_PUSHBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP,
                   xspace, 5, size[SAVEPB].x, size[SAVEPB].y, hwnd, HWND_TOP, ID_SAVEPB, NULL, NULL);

   /* create notebook, here if the warp 4 doesn't show, I am trying to minic it
      with the old style one */
   hwndNB =  WinCreateWindow( hwnd, WC_NOTEBOOK, NULL,
             WS_VISIBLE | 0x800 | WS_GROUP | WS_TABSTOP |
             /* needed for old style customizeable notebook */
             BKS_BACKPAGESTR | BKS_MAJORTABTOP | BKS_ROUNDEDTABS |
             BKS_STATUSTEXTCENTER | BKS_SPIRALBIND | BKS_TABTEXTLEFT,
             0, 0, 0, 0, hwnd, HWND_TOP, ID_NB, NULL, NULL );

   if(!hwndNB) return FALSE;

   /* change colors for old style notebook not to look ugly */
   WinSendMsg(hwndNB, BKM_SETNOTEBOOKCOLORS, MPFROMLONG(SYSCLR_FIELDBACKGROUND),
              MPFROMSHORT(BKA_BACKGROUNDPAGECOLORINDEX));

   /* change tab width for old style notebook to something OK for most font size */
   WinSendMsg(hwndNB, BKM_SETDIMENSIONS, MPFROM2SHORT(80,25),
              MPFROMSHORT(BKA_MAJORTAB));

   /* no minor */
   WinSendMsg(hwndNB, BKM_SETDIMENSIONS, 0, MPFROMSHORT(BKA_MINORTAB));

   /* load notebook pages */
   for(i = 0; i < PAGE_COUNT; i++)
   {
      nbpage[i].ulPageId = (LONG)WinSendMsg(hwndNB,  BKM_INSERTPAGE, NULL,
           MPFROM2SHORT((BKA_STATUSTEXTON | BKA_AUTOPAGESIZE | nbpage[i].usTabType),
           BKA_LAST));

      if (!nbpage[i].ulPageId)
         return FALSE;

      if (!WinSendMsg(hwndNB, BKM_SETSTATUSLINETEXT, MPFROMLONG(nbpage[i].ulPageId),
                      MPFROMP(nbpage[i].szStatusLineText)))
         return FALSE;

      if (!WinSendMsg(hwndNB, BKM_SETTABTEXT, MPFROMLONG(nbpage[i].ulPageId),
                      MPFROMP(nbpage[i].szTabText)))
         return FALSE;

      if (!WinSendMsg(hwndNB, BKM_SETPAGEDATA, MPFROMLONG(nbpage[i].ulPageId),
                      MPFROMP(&nbpage[i])))
         return FALSE;
   }

   /* return success (notebook window handle) */
   return hwndNB;
}

MRESULT EXPENTRY wpWindowList( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   /* prevents the mess that comes from the system sending messages and
      starting writing to a variable while it is also reading from the
      same variable */
   static nottheuser = FALSE;

   switch(msg)
   {
      /* this is useful when the user presses F1 in any of the dialog controls owned
         by this window.  Also, all the previous kludge sends WM_HELP here */
      case WM_HELP:
         if(helpenabled)
            WinSendMsg(WinQueryHelpInstance(hwnd),
                       HM_DISPLAY_HELP,MPFROMP("WindowList"),
                       MPFROMSHORT(HM_PANELNAME));
         return 0; /* grrr, beat off Help Manager */

      case WM_INITDLG:

         WinOldStringEntryFieldWindowProc = WinSubclassWindow(WinWindowFromID(hwnd,ID_STRING_FIELD),(PFNWP)wpStringEntryField);
         WinOldStringListBoxWindowProc = WinSubclassWindow(WinWindowFromID(hwnd,ID_STRING_LIST),(PFNWP)wpStringListBox);

         refreshListbox(hwnd, ID_LISTBOX);
         WinSendDlgItemMsg(hwnd, ID_UPPERLEFT, BM_SETCHECK, MPFROMSHORT(1),0);

      case WM_USER:

         nottheuser = TRUE;

         WinSendDlgItemMsg(hwnd, ID_MB1, BM_SETCHECK, MPFROMSHORT(actopt.bit.mb1),0);
         WinSendDlgItemMsg(hwnd, ID_MB2, BM_SETCHECK, MPFROMSHORT(actopt.bit.mb2),0);
         WinSendDlgItemMsg(hwnd, ID_MB3, BM_SETCHECK, MPFROMSHORT(actopt.bit.mb3),0);
         WinSendDlgItemMsg(hwnd, ID_ALT, BM_SETCHECK, MPFROMSHORT(actopt.bit.alt),0);
         WinSendDlgItemMsg(hwnd, ID_CTRL, BM_SETCHECK, MPFROMSHORT(actopt.bit.ctrl),0);
         WinSendDlgItemMsg(hwnd, ID_SHIFT, BM_SETCHECK, MPFROMSHORT(actopt.bit.shift),0);

         WinSendDlgItemMsg(hwnd, ID_MOVE, BM_SETCHECK, MPFROMSHORT(curopt[actopt.number].move),0);
         WinSendDlgItemMsg(hwnd, ID_MIN, BM_SETCHECK, MPFROMSHORT(curopt[actopt.number].minimize),0);
         WinSendDlgItemMsg(hwnd, ID_PUSHBACK, BM_SETCHECK, MPFROMSHORT(curopt[actopt.number].pushback),0);

         WinEnableWindowUpdate(WinWindowFromID(hwnd, ID_LISTBOX),FALSE);
         selectActivated(hwnd, ID_LISTBOX, curlist[actopt.number]);
         WinShowWindow(WinWindowFromID(hwnd, ID_LISTBOX),TRUE);

         WinEnableWindowUpdate(WinWindowFromID(hwnd, ID_STRING_LIST),FALSE);
         addAll(hwnd, ID_STRING_LIST, curliststr[actopt.number]);
         WinShowWindow(WinWindowFromID(hwnd, ID_STRING_LIST),TRUE);

         nottheuser = FALSE;

         if(SHORT1FROMMR(WinSendDlgItemMsg(hwnd,ID_FLOATTOP, BM_QUERYCHECK, 0, 0)) ||
            SHORT1FROMMR(WinSendDlgItemMsg(hwnd,ID_FLOATBOTTOM, BM_QUERYCHECK, 0, 0)))
         {
            WinEnableWindow(WinWindowFromID(hwnd,ID_MB1),FALSE);
            WinEnableWindow(WinWindowFromID(hwnd,ID_MB2),FALSE);
            WinEnableWindow(WinWindowFromID(hwnd,ID_MB3),FALSE);
            WinEnableWindow(WinWindowFromID(hwnd,ID_ALT),FALSE);
            WinEnableWindow(WinWindowFromID(hwnd,ID_CTRL),FALSE);
            WinEnableWindow(WinWindowFromID(hwnd,ID_SHIFT),FALSE);
            WinEnableWindow(WinWindowFromID(hwnd,ID_MOVE),FALSE);
            WinEnableWindow(WinWindowFromID(hwnd,ID_MIN),FALSE);
            WinEnableWindow(WinWindowFromID(hwnd,ID_PUSHBACK),FALSE);
         }
         else
         {
            WinEnableWindow(WinWindowFromID(hwnd,ID_MB1),TRUE);
            WinEnableWindow(WinWindowFromID(hwnd,ID_MB2),TRUE);
            WinEnableWindow(WinWindowFromID(hwnd,ID_MB3),TRUE);
            WinEnableWindow(WinWindowFromID(hwnd,ID_ALT),TRUE);
            WinEnableWindow(WinWindowFromID(hwnd,ID_CTRL),TRUE);
            WinEnableWindow(WinWindowFromID(hwnd,ID_SHIFT),TRUE);
            WinEnableWindow(WinWindowFromID(hwnd,ID_MOVE),TRUE);
            WinEnableWindow(WinWindowFromID(hwnd,ID_MIN),TRUE);
            WinEnableWindow(WinWindowFromID(hwnd,ID_PUSHBACK),TRUE);
         }

         return 0;

      case WM_CONTROL:
         if(!nottheuser)
         {
            if((SHORT2FROMMP(mp1) == BN_CLICKED) || (SHORT2FROMMP(mp1) == BN_DBLCLICKED))
               switch (SHORT1FROMMP(mp1))
               {
                  /* radio buttons */
                  case ID_UPPERLEFT: curlist = upperleft; curliststr = upperleftstr; curopt = upperleftopt; WinSendMsg(hwnd,WM_USER,0,0); break;
                  case ID_UPPERRIGHT: curlist = upperright; curliststr = upperrightstr; curopt = upperrightopt; WinSendMsg(hwnd,WM_USER,0,0); break;
                  case ID_LOWERLEFT: curlist = lowerleft; curliststr = lowerleftstr; curopt = lowerleftopt; WinSendMsg(hwnd,WM_USER,0,0); break;
                  case ID_LOWERRIGHT: curlist = lowerright; curliststr = lowerrightstr; curopt = lowerrightopt; WinSendMsg(hwnd,WM_USER,0,0); break;
                  case ID_UP: curlist = up; curopt = upopt; curliststr = upstr; WinSendMsg(hwnd,WM_USER,0,0); break;
                  case ID_DOWN: curlist = down; curopt = downopt; curliststr = downstr; WinSendMsg(hwnd,WM_USER,0,0); break;
                  case ID_LEFT: curlist = left; curopt = leftopt; curliststr = leftstr; WinSendMsg(hwnd,WM_USER,0,0); break;
                  case ID_RIGHT: curlist = right; curopt = rightopt; curliststr = rightstr; WinSendMsg(hwnd,WM_USER,0,0); break;

                  case ID_FLOATTOP: curlist = floattop; curopt = floattopopt; curliststr = floattopstr; WinSendMsg(hwnd,WM_USER,0,0); break;
                  case ID_FLOATBOTTOM: curlist = floatbottom; curopt = floatbottomopt; curliststr = floatbottomstr; WinSendMsg(hwnd,WM_USER,0,0); break;

                  /* check buttons */
                  case ID_MB1: actopt.bit.mb1 = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, SHORT1FROMMP(mp1), BM_QUERYCHECK, 0, 0)); WinSendMsg(hwnd,WM_USER,0,0); break;
                  case ID_MB2: actopt.bit.mb2 = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, SHORT1FROMMP(mp1), BM_QUERYCHECK, 0, 0)); WinSendMsg(hwnd,WM_USER,0,0); break;
                  case ID_MB3: actopt.bit.mb3 = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, SHORT1FROMMP(mp1), BM_QUERYCHECK, 0, 0)); WinSendMsg(hwnd,WM_USER,0,0); break;
                  case ID_ALT: actopt.bit.alt = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, SHORT1FROMMP(mp1), BM_QUERYCHECK, 0, 0)); WinSendMsg(hwnd,WM_USER,0,0); break;
                  case ID_CTRL: actopt.bit.ctrl = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, SHORT1FROMMP(mp1), BM_QUERYCHECK, 0, 0)); WinSendMsg(hwnd,WM_USER,0,0); break;
                  case ID_SHIFT: actopt.bit.shift = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, SHORT1FROMMP(mp1), BM_QUERYCHECK, 0, 0)); WinSendMsg(hwnd,WM_USER,0,0); break;

                  case ID_MOVE: curopt[actopt.number].move = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, SHORT1FROMMP(mp1), BM_QUERYCHECK, 0, 0)); break;
                  case ID_MIN: curopt[actopt.number].minimize = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, SHORT1FROMMP(mp1), BM_QUERYCHECK, 0, 0)); break;
                  case ID_PUSHBACK: curopt[actopt.number].pushback = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, SHORT1FROMMP(mp1), BM_QUERYCHECK, 0, 0)); break;
               }

            if((SHORT2FROMMP(mp1) == LN_ENTER) || (SHORT2FROMMP(mp1) == LN_SELECT))
               switch(SHORT1FROMMP(mp1))
               {
                  /* listbox */
                  case ID_LISTBOX:
                     activateSelected(hwnd, SHORT1FROMMP(mp1), &curlist[actopt.number]);
                     if(curlist == floattop)
                        refill(floattop[0],floattopstr[0],hwndtop);
                     else if(curlist == floatbottom)
                        refill(floatbottom[0],floatbottomstr[0],hwndbottom);
                     break;
               }
         }
         return 0;

      case WM_COMMAND:
         switch (SHORT1FROMMP(mp1))
         {
            /* push buttons */
            case ID_REFRESH:
               WinEnableWindowUpdate(WinWindowFromID(hwnd, ID_LISTBOX),FALSE);
               nottheuser = TRUE;
               refreshListbox(hwnd, ID_LISTBOX);
               nottheuser = FALSE;
               WinShowWindow(WinWindowFromID(hwnd, ID_LISTBOX),TRUE);
               break;

            /* substring entryfield and listbox */
            case ID_STRING_ADD:
            {
               char *buffer;
               USHORT item,size;

               /* find text */
               size = WinQueryDlgItemTextLength(hwnd,ID_STRING_FIELD)+1;
               buffer = alloca(size);
               WinQueryDlgItemText(hwnd,ID_STRING_FIELD,size,buffer);
               if(*buffer)
               {
                  /* insert item as first item to make it seem pushed down */
                  item = (SHORT) WinSendDlgItemMsg(hwnd,ID_STRING_LIST, LM_INSERTITEM, 0, MPFROMP(buffer));
                  if((item != LIT_ERROR) && (item != LIT_MEMERROR))
                  {
                     activateAll(hwnd, ID_STRING_LIST, &curliststr[actopt.number]);
                     WinSetDlgItemText(hwnd,ID_STRING_FIELD,"");
                  }
               }
            }
            break;

            case ID_STRING_DEL:
            {
               SHORT item = (SHORT) WinSendDlgItemMsg(hwnd,ID_STRING_LIST,LM_QUERYSELECTION,MPFROMSHORT(LIT_FIRST),0);

               if(item != LIT_NONE)
               {
                  WinSendDlgItemMsg(hwnd, ID_STRING_LIST, LM_DELETEITEM, MPFROMSHORT(item),0);
                  activateAll(hwnd, ID_STRING_LIST, &curliststr[actopt.number]);
               }
            }
         }

      return 0;

      case WM_CHAR:
         if(SHORT2FROMMP(mp2) == VK_ESC) return 0;
   }

   return WinDefDlgProc( hwnd, msg, mp1, mp2 );
}

MRESULT EXPENTRY wpStringEntryField( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   switch( msg )
   {
      case WM_CHAR:

         if(!(SHORT1FROMMP(mp1) & KC_KEYUP) &&
              ((SHORT2FROMMP(mp2) == VK_NEWLINE) ||
               (SHORT2FROMMP(mp2) == VK_ENTER)))
         {
            WinSendMsg(nbpage[PAGE_WINDOWLIST].hwnd,WM_COMMAND,MPFROMSHORT(ID_STRING_ADD),0);
            return 0;
         }
      break;
   }

   return WinOldStringEntryFieldWindowProc( hwnd, msg, mp1, mp2 );
}

MRESULT EXPENTRY wpStringListBox( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   if(  (msg == WM_CHAR) &&
       !(SHORT1FROMMP(mp1) & KC_KEYUP) &&
        (SHORT2FROMMP(mp2) == VK_DELETE)  )
      {
         WinSendMsg(nbpage[PAGE_WINDOWLIST].hwnd,WM_COMMAND,MPFROMSHORT(ID_STRING_DEL),0);
         return 0;
      }
   return WinOldStringListBoxWindowProc( hwnd, msg, mp1, mp2 );
}

MRESULT EXPENTRY wpObjects( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   /* prevents the mess that comes from the system sending messages and
      starting writing to a variable while it is also reading from the
      same variable */
   static nottheuser = FALSE;

   switch(msg)
   {
      /* this is useful when the user presses F1 in any of the dialog controls owned
         by this window.  Also, all the previous kludge sends WM_HELP here */
      case WM_HELP:
         if(helpenabled)
            WinSendMsg(WinQueryHelpInstance(hwnd),
                       HM_DISPLAY_HELP,MPFROMP("Objects"),
                       MPFROMSHORT(HM_PANELNAME));
         return 0; /* grrr, beat off Help Manager */

      case WM_INITDLG:
         /* subclass listbox to allow drag and drop */
         WinOldStringListBoxWindowProc = WinSubclassWindow(WinWindowFromID(hwnd,ID_LISTBOX),(PFNWP)wpObjectListbox);
         WinSendDlgItemMsg(hwnd, ID_UPPERLEFT, BM_SETCHECK, MPFROMSHORT(1),0);

      case WM_USER:

         nottheuser = TRUE;

         WinSendDlgItemMsg(hwnd, ID_MB1, BM_SETCHECK, MPFROMSHORT(actopt.bit.mb1),0);
         WinSendDlgItemMsg(hwnd, ID_MB2, BM_SETCHECK, MPFROMSHORT(actopt.bit.mb2),0);
         WinSendDlgItemMsg(hwnd, ID_MB3, BM_SETCHECK, MPFROMSHORT(actopt.bit.mb3),0);
         WinSendDlgItemMsg(hwnd, ID_ALT, BM_SETCHECK, MPFROMSHORT(actopt.bit.alt),0);
         WinSendDlgItemMsg(hwnd, ID_CTRL, BM_SETCHECK, MPFROMSHORT(actopt.bit.ctrl),0);
         WinSendDlgItemMsg(hwnd, ID_SHIFT, BM_SETCHECK, MPFROMSHORT(actopt.bit.shift),0);

         WinEnableWindowUpdate(WinWindowFromID(hwnd, ID_LISTBOX),FALSE);
         getObjects(hwnd,ID_LISTBOX,curobj[actopt.number]);
         WinShowWindow(WinWindowFromID(hwnd, ID_LISTBOX),TRUE);

         nottheuser = FALSE;
         return 0;

      case WM_CONTROL:

         if(((SHORT2FROMMP(mp1) == BN_CLICKED) || (SHORT2FROMMP(mp1) == BN_DBLCLICKED) ||
             (SHORT2FROMMP(mp1) == LN_ENTER) || (SHORT2FROMMP(mp1) == LN_SELECT)) &&
            !nottheuser)
            switch (SHORT1FROMMP(mp1))
            {
               /* radio buttons */
               case ID_UPPERLEFT: curobj = oupperleft; WinSendMsg(hwnd,WM_USER,0,0); break;
               case ID_UPPERRIGHT: curobj = oupperright; WinSendMsg(hwnd,WM_USER,0,0); break;
               case ID_LOWERLEFT: curobj = olowerleft; WinSendMsg(hwnd,WM_USER,0,0); break;
               case ID_LOWERRIGHT: curobj = olowerright; WinSendMsg(hwnd,WM_USER,0,0); break;
               case ID_UP: curobj = oup; WinSendMsg(hwnd,WM_USER,0,0); break;
               case ID_DOWN: curobj = odown; WinSendMsg(hwnd,WM_USER,0,0); break;
               case ID_LEFT: curobj = oleft; WinSendMsg(hwnd,WM_USER,0,0); break;
               case ID_RIGHT: curobj = oright; WinSendMsg(hwnd,WM_USER,0,0); break;

               /* check buttons */
               case ID_MB1: actopt.bit.mb1 = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, SHORT1FROMMP(mp1), BM_QUERYCHECK, 0, 0)); WinSendMsg(hwnd,WM_USER,0,0); break;
               case ID_MB2: actopt.bit.mb2 = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, SHORT1FROMMP(mp1), BM_QUERYCHECK, 0, 0)); WinSendMsg(hwnd,WM_USER,0,0); break;
               case ID_MB3: actopt.bit.mb3 = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, SHORT1FROMMP(mp1), BM_QUERYCHECK, 0, 0)); WinSendMsg(hwnd,WM_USER,0,0); break;
               case ID_ALT: actopt.bit.alt = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, SHORT1FROMMP(mp1), BM_QUERYCHECK, 0, 0)); WinSendMsg(hwnd,WM_USER,0,0); break;
               case ID_CTRL: actopt.bit.ctrl = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, SHORT1FROMMP(mp1), BM_QUERYCHECK, 0, 0)); WinSendMsg(hwnd,WM_USER,0,0); break;
               case ID_SHIFT: actopt.bit.shift = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, SHORT1FROMMP(mp1), BM_QUERYCHECK, 0, 0)); WinSendMsg(hwnd,WM_USER,0,0); break;
            }

         return 0;

      case WM_CHAR:
         if(SHORT2FROMMP(mp2) == VK_ESC) return 0;
   }
   return WinDefDlgProc( hwnd, msg, mp1, mp2 );
}

MRESULT EXPENTRY wpObjectListbox( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   switch( msg )
   {
      case DM_DRAGOVER:
      {
         DRAGINFO *draginfo = (DRAGINFO *)mp1;
         USHORT cItems,i,accept;
         DRAGITEM *dragitem;

         DrgAccessDraginfo(draginfo);
         cItems = DrgQueryDragitemCount(draginfo);
         accept = DOR_DROP;

         for (i = 0; i < cItems; i++)
         {
            dragitem = DrgQueryDragitemPtr(draginfo, i);

            /* Check the rendering format */
            if (  !DrgVerifyRMF(dragitem, "DRM_OS2FILE", NULL) &&
                  !DrgVerifyRMF(dragitem, "DRM_OBJECT",  NULL)     )
               accept = DOR_NEVERDROP;
         }

         DrgFreeDraginfo(draginfo);

         return (MRFROM2SHORT(accept, DO_COPY));
      }
      case DM_DROP:
      {
         DRAGINFO *draginfo = (DRAGINFO *)mp1;
         USHORT cItems,i;
         DRAGITEM *dragitem;
         char buffer[2*CCHMAXPATH] = {0};
         ULONG hobj = 0;

         DrgAccessDraginfo(draginfo);
         cItems = DrgQueryDragitemCount(draginfo);

         for (i = 0; i < cItems; i++, buffer[0] = '\0')
         {
            dragitem = DrgQueryDragitemPtr(draginfo, i);

            /* Check the rendering format */
            if (DrgVerifyRMF(dragitem, "DRM_OS2FILE", NULL))
            {
               DrgQueryStrName(dragitem->hstrContainerName,2*CCHMAXPATH,buffer);
               DrgQueryStrName(dragitem->hstrSourceName,CCHMAXPATH,strchr(buffer,'\0'));

               hobj = WinQueryObject(buffer);
            }
            else if (DrgVerifyRMF(dragitem, "DRM_OBJECT", NULL))
            {
               if(hobj = HObjectFromID(dragitem->ulItemID,TRUE))
                  QueryPathTitle(hobj,FALSE,FALSE,buffer);
            }

            /* add item + hobj attached to it */
            if (buffer[0])
            {
               WinSendMsg(hwnd, LM_SETITEMHANDLE, MPFROMSHORT(

                   (SHORT) WinSendMsg(hwnd, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP(buffer))

               ), MPFROMLONG(hobj));

               setObjects(hwnd, &curobj[actopt.number]);
            }
         }

         DrgFreeDraginfo(draginfo);

         return 0;
      }
      case WM_CONTEXTMENU:
      {
         if (SHORT2FROMMP(mp2) == TRUE)
         WinPopupMenu(hwnd, hwnd, WinLoadMenu(hwnd, 0, ID_OPOPUPMENU), 10, 10, 0,
                      PU_HCONSTRAIN | PU_VCONSTRAIN | PU_MOUSEBUTTON1 | PU_KEYBOARD);
         else
         WinPopupMenu(hwnd, hwnd, WinLoadMenu(hwnd, 0, ID_OPOPUPMENU), SHORT1FROMMP(mp1), SHORT2FROMMP(mp1), 0,
                      PU_HCONSTRAIN | PU_VCONSTRAIN | PU_MOUSEBUTTON1 | PU_KEYBOARD);
         return 0;
      }
      case WM_COMMAND:
         switch(SHORT1FROMMP(mp1))
         {
            case ID_ADD:
            {
               int i;
               FILEDLG filedlginfo = {0};
               filedlginfo.cbSize = sizeof(FILEDLG);
               filedlginfo.fl = FDS_CENTER | FDS_OPEN_DIALOG | FDS_MULTIPLESEL;
               filedlginfo.pszTitle = "Add Data File Objects";
               strcpy(filedlginfo.szFullFile,EXEpath);
               filedlginfo.pszOKButton = "Add";

               WinFileDlg(HWND_DESKTOP,hwnd,&filedlginfo);

               /* add item + hobj attached to it */
               for(i = 0; i < filedlginfo.ulFQFCount ; i++)
                  WinSendMsg(hwnd, LM_SETITEMHANDLE, MPFROMSHORT(

                        (SHORT) WinSendMsg(hwnd, LM_INSERTITEM, MPFROMSHORT(LIT_END),
                                      MPFROMP(*filedlginfo.papszFQFilename[i]))

                  ), MPFROMLONG(WinQueryObject(*filedlginfo.papszFQFilename[i])));

               setObjects(hwnd, &curobj[actopt.number]);
               WinFreeFileDlgList(filedlginfo.papszFQFilename);

            }
            case ID_DELETE:
            {
               /* call up first numbah */
               SHORT selectIndex = SHORT1FROMMR(WinSendMsg(hwnd, LM_QUERYSELECTION,
                                                MPFROMSHORT(LIT_FIRST), 0));

               while(selectIndex != LIT_NONE)
               {
                  WinSendMsg(hwnd, LM_DELETEITEM, MPFROMSHORT(selectIndex), 0);

                  /* call up remaining numbah */
                  selectIndex = SHORT1FROMMR(WinSendMsg(hwnd, LM_QUERYSELECTION,
                                              MPFROMSHORT(LIT_FIRST), 0));
               }
               setObjects(hwnd, &curobj[actopt.number]);
               break;
            }
         }
      return 0;

   case WM_CHAR:
      if (!(SHORT1FROMMP(mp1) & KC_KEYUP))
         if (SHORT1FROMMP(mp1) & KC_VIRTUALKEY)
            switch (SHORT2FROMMP(mp2))
            {
               case VK_DELETE:
                  WinSendMsg(hwnd, WM_COMMAND, MPFROMSHORT(ID_DELETE), 0); break;
               case VK_INSERT:
                  WinSendMsg(hwnd, WM_COMMAND, MPFROMSHORT(ID_ADD), 0); break;
               case VK_F11:
                  if (SHORT1FROMMP(mp1) & KC_SHIFT)
                     WinSendMsg(hwnd, WM_CONTEXTMENU, 0, MPFROM2SHORT(0,TRUE)); break;
            }
      break;
   }
   return (MRESULT)  WinOldStringListBoxWindowProc(hwnd,msg,mp1,mp2);
}

MRESULT EXPENTRY wpOptions( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   switch( msg )
   {
      /* this is useful when the user presses F1 in any of the dialog controls owned
         by this window.  Also, all the previous kludge sends WM_HELP here */
      case WM_HELP:
         if(helpenabled)
            WinSendMsg(WinQueryHelpInstance(hwnd),
                       HM_DISPLAY_HELP,MPFROMP("Options"),
                       MPFROMSHORT(HM_PANELNAME));
         return 0; /* grrr, beat off Help Manager */

      case WM_INITDLG:
         WinSendDlgItemMsg(hwnd, ID_SPIN1, SPBM_OVERRIDESETLIMITS, MPFROMLONG(10000),0);
         WinSendDlgItemMsg(hwnd, ID_SPINMARGIN, SPBM_OVERRIDESETLIMITS, MPFROMLONG(1000),0);
      /* refresh */
      case WM_USER:
         WinSendDlgItemMsg(hwnd, ID_SPIN1, SPBM_SETCURRENTVALUE, MPFROMLONG(delay),0);
         WinSendDlgItemMsg(hwnd, ID_SPINMARGIN, SPBM_SETCURRENTVALUE, MPFROMLONG(*margin),0);
         return 0;

      case WM_CONTROL:
         switch(SHORT1FROMMP(mp1))
         {
            case ID_SPIN1:
               switch(SHORT2FROMMP(mp1))
               {
                  case SPBN_CHANGE:
                     WinSendDlgItemMsg(hwnd, ID_SPIN1, SPBM_QUERYVALUE, MPFROMP(&delay),0);
                     break;
                  /* jump up and down by 10 */
                  case SPBN_UPARROW:
                     WinSendDlgItemMsg(hwnd, ID_SPIN1, SPBM_QUERYVALUE, MPFROMP(&delay),0);
                     WinSendDlgItemMsg(hwnd, ID_SPIN1, SPBM_SETCURRENTVALUE, MPFROMLONG(delay+9),0);
                     break;
                  case SPBN_DOWNARROW:
                     WinSendDlgItemMsg(hwnd, ID_SPIN1, SPBM_QUERYVALUE, MPFROMP(&delay),0);
                     WinSendDlgItemMsg(hwnd, ID_SPIN1, SPBM_SETCURRENTVALUE, MPFROMLONG(delay-9),0);
                     break;
               }
               break;

            case ID_SPINMARGIN:
               switch(SHORT2FROMMP(mp1))
               {
                  case SPBN_CHANGE:
                  {
                     ULONG temp;
                     WinSendDlgItemMsg(hwnd, ID_SPINMARGIN, SPBM_QUERYVALUE, MPFROMP(&temp),0);
                     *margin = temp;
                     break;
                  }
                  case SPBN_DOWNARROW:
                     if(*margin == 1000) WinSendDlgItemMsg(hwnd, ID_SPINMARGIN, SPBM_SPINUP, MPFROMLONG(1),0);
                     break;
               }
               break;
         }
      return 0;

      case WM_CHAR:
         if(SHORT2FROMMP(mp2) == VK_ESC) return 0;
   }
   return WinDefDlgProc( hwnd, msg, mp1, mp2 );
}

MRESULT EXPENTRY wpAbout( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   switch( msg )
   {
      /* this is useful when the user presses F1 in any of the dialog controls owned
         by this window.  Also, all the previous kludge sends WM_HELP here */
      case WM_HELP:
         if(helpenabled)
            WinSendMsg(WinQueryHelpInstance(hwnd),
                       HM_DISPLAY_HELP,MPFROMP("About"),
                       MPFROMSHORT(HM_PANELNAME));
         return 0; /* grrr, beat off Help Manager */
   }

   return WinDefDlgProc( hwnd, msg, mp1, mp2 );
}


#ifdef NEED_DUMMY_WP
/* used for windows with no action to give focus to the next dialog item after
   the notebook and with a tabstop to prevent a keyboard pitfall */
MRESULT EXPENTRY wpDummy( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
   if(msg == WM_CHAR)
      WinSetFocus(HWND_DESKTOP,WinEnumDlgItem(hwndClient,WinWindowFromID(hwndClient,ID_NB),EDI_NEXTTABITEM));
   return WinDefDlgProc( hwnd, msg, mp1, mp2 );
}
#endif

/* refresh listboxes with new window list data */
         /* listboxes parent handle and its id */
BOOL refreshListbox(HWND hwnd, int input_id)
{
   int i;              /* loop index */
   char *temp = NULL;  /* temporary string buffers to reselect the entries after refresh */

   /* switch list stuff */
   ULONG count,buffersize;

   /* get switch list data */
   count = WinQuerySwitchList(hab,NULL,0);
   buffersize = count * sizeof(SWENTRY) + sizeof(ULONG);
   if(_msize(switchlist) < buffersize)
   {
      free(switchlist);
      switchlist = (PSWBLOCK) malloc(2*buffersize);
   }
   count = WinQuerySwitchList(hab,switchlist,buffersize);

   if (switchlist)
   {
      /* backup the selected items */
      activateSelected(hwnd, input_id, &temp);
      /* delete all entries in case some entries were already there */
      WinSendDlgItemMsg(hwnd, input_id, LM_DELETEALL, 0, 0);

      /* go through the whole switch list items */
      for (i = 0 ; i < count ; i++)
      {
         /* insert entry before the first one */
         WinSendDlgItemMsg(hwnd, input_id, LM_INSERTITEM, 0,
               MPFROMP(switchlist->aswentry[i].swctl.szSwtitle));
      }
      /* restore selected items */
      selectActivated(hwnd, input_id, temp);
   }

   free(temp);

   return TRUE;
}

/* dumps an array "array" of strings into a listbox */
BOOL addAll(HWND hwnd, int output_id, char *input)
{
   char *inputpos = input;

   WinSendDlgItemMsg(hwnd, output_id, LM_DELETEALL, 0, 0);

   /* go through all the strings and insert them all */
   while(inputpos && *inputpos)
   {
      WinSendDlgItemMsg(hwnd, output_id, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP(inputpos));
      inputpos = strchr(inputpos,'\0') + 1;
   }

   return TRUE;
}

/* dumps a listbox into an "array" of strings */
BOOL activateAll(HWND hwnd, int input_id, char **output)
{
   SHORT Length;
   char *outputpos;
   int  i;   /* loop index */
   SHORT count = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, input_id, LM_QUERYITEMCOUNT,0,0));

   Length = 0;
   for(i = 0; i < count ; i++)
      Length += SHORT1FROMMR(WinSendDlgItemMsg(hwnd, input_id, LM_QUERYITEMTEXTLENGTH,
                                               MPFROMSHORT(i), 0)) + 1;
   free(*output);
   outputpos = *output = calloc(++Length,sizeof(char));

   for(i = 0; i < count ; i++)
   {
      Length = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, input_id, LM_QUERYITEMTEXTLENGTH,
                                              MPFROMSHORT(i), 0)) + 1;

      WinSendDlgItemMsg(hwnd, input_id, LM_QUERYITEMTEXT,
                        MPFROM2SHORT(i, Length), MPFROMP(outputpos));

      outputpos += Length;
   }

   return TRUE;
}

/* selects the entries present in the listbox to reflect the current active state
   of the hot corners and borders, normally used after loading from INI file */
/* parent handle of the lisbox, listbox ID, input arrays of pointers */
BOOL selectActivated(HWND hwnd, int output_id, char *input)
{
   char *inputpos = input;
   SHORT selectIndex;

   WinSendDlgItemMsg(hwnd, output_id, LM_SELECTITEM, MPFROMSHORT(LIT_NONE), MPFROMSHORT(FALSE));

   while(inputpos && *inputpos)
   {
      selectIndex = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, output_id, LM_SEARCHSTRING,
                                 MPFROM2SHORT(0,LIT_FIRST),MPFROMP(inputpos)));

      if((selectIndex != LIT_NONE) && (selectIndex != LIT_ERROR))
         WinSendDlgItemMsg(hwnd, output_id, LM_SELECTITEM,
                           MPFROMSHORT(selectIndex), MPFROMSHORT(TRUE));

      inputpos = strchr(inputpos,'\0') + 1;
   }

   return TRUE;
}

/* activates listbox data by storing them in an array of pointers */
/* *output[] MUST be NULL terminated */
BOOL activateSelected(HWND hwnd, int input_id, char **output)
{
   SHORT selectIndex, Length;
   char *outputpos;
   int  i;   /* loop index */

   /* getting the first number */
   selectIndex = LIT_FIRST;
   Length = 0;
   selectIndex = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, input_id,
                 LM_QUERYSELECTION, MPFROMSHORT(selectIndex), 0));

   for(i = 0; selectIndex != LIT_NONE ; i++)
   {
      Length += SHORT1FROMMR(WinSendDlgItemMsg(hwnd, input_id, LM_QUERYITEMTEXTLENGTH,
                MPFROMSHORT(selectIndex), 0)) + 1;

      /* call up the next numbah */
      selectIndex = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, input_id,
                    LM_QUERYSELECTION, MPFROMSHORT(selectIndex), 0));
   }

   free(*output);
   outputpos = *output = calloc(++Length,sizeof(char));

   /* getting the first number */
   selectIndex = LIT_FIRST;
   selectIndex = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, input_id,
                 LM_QUERYSELECTION, MPFROMSHORT(selectIndex), 0));

   for(i = 0; selectIndex != LIT_NONE ; i++)
   {
      Length = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, input_id, LM_QUERYITEMTEXTLENGTH,
                            MPFROMSHORT(selectIndex), 0)) + 1;

      SHORT1FROMMR(WinSendDlgItemMsg(hwnd, input_id, LM_QUERYITEMTEXT,
                   MPFROM2SHORT(selectIndex, Length), MPFROMP(outputpos)));

      outputpos += Length;

      /* call up the next numbah */
      selectIndex = SHORT1FROMMR(WinSendDlgItemMsg(hwnd, input_id,
                    LM_QUERYSELECTION, MPFROMSHORT(selectIndex), 0));
   }
   return TRUE;
}

/* refresh object listbox with new titles */
BOOL getObjects(HWND hwnd, int output_id, HOBJECT *objlist)
{
   int i;     /* loop index */
   int objnum = _msize(objlist)/sizeof(HOBJECT);  /* number of objects in objlist */
   char pathtitle[2*CCHMAXPATH];

   /* delete all entries in case some entries were already there */
   WinSendDlgItemMsg(hwnd, output_id, LM_DELETEALL, 0, 0);

   /* go through every hobj found for this listbox */
   for (i = 0 ; i < objnum ; i++, pathtitle[0] = '\0')
   {
      QueryPathTitle(objlist[i],FALSE,FALSE,pathtitle);
      if(pathtitle[0])
         WinSendDlgItemMsg(hwnd, output_id, LM_SETITEMHANDLE, MPFROMSHORT(

                  (SHORT) WinSendDlgItemMsg(hwnd, output_id, LM_INSERTITEM,
                                      MPFROMSHORT(LIT_END), MPFROMP(pathtitle))

              ), MPFROMLONG(objlist[i]));
   }
   return TRUE;
}

/* put the objects in the listbox into activation */
BOOL setObjects(HWND hwnd, HOBJECT **objlist)
{
   SHORT count;

   free(*objlist); *objlist = NULL;

   if (count = (SHORT) WinSendMsg(hwnd, LM_QUERYITEMCOUNT, 0, 0))
   {
      int  i;   /* loop index */

      *objlist = malloc(count*sizeof(HOBJECT));

      for(i = 0; i < count ; i++)
         (*objlist)[i] = LONGFROMMP(WinSendMsg(hwnd, LM_QUERYITEMHANDLE, MPFROMSHORT(i), 0));
   }

   return TRUE;
}

/* load INI file */
BOOL loadINIfile()
{
   HINI INIhandle;

   if (INIhandle = PrfOpenProfile(hab,INIfile))
   {
      ULONG datasize;
      int i;
      char buffer[100];
      BOOL rc = 0;

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Windows %d",i);
         rc += loadWindows(INIhandle, "Upper Left", buffer, &upperleft[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Windows %d",i);
         rc += loadWindows(INIhandle, "Lower Left", buffer, &lowerleft[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Windows %d",i);
         rc += loadWindows(INIhandle, "Upper Right", buffer, &upperright[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Windows %d",i);
         rc += loadWindows(INIhandle, "Lower Right", buffer, &lowerright[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Windows %d",i);
         rc += loadWindows(INIhandle, "Up", buffer, &up[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Windows %d",i);
         rc += loadWindows(INIhandle, "Down", buffer, &down[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Windows %d",i);
         rc += loadWindows(INIhandle, "Left", buffer, &left[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Windows %d",i);
         rc += loadWindows(INIhandle, "Right", buffer, &right[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Substring %d",i);
         rc += loadWindows(INIhandle, "Upper Left", buffer, &upperleftstr[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Substring %d",i);
         rc += loadWindows(INIhandle, "Lower Left", buffer, &lowerleftstr[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Substring %d",i);
         rc += loadWindows(INIhandle, "Upper Right", buffer, &upperrightstr[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Substring %d",i);
         rc += loadWindows(INIhandle, "Lower Right", buffer, &lowerrightstr[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Substring %d",i);
         rc += loadWindows(INIhandle, "Up", buffer, &upstr[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Substring %d",i);
         rc += loadWindows(INIhandle, "Down", buffer, &downstr[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Substring %d",i);
         rc += loadWindows(INIhandle, "Left", buffer, &leftstr[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Substring %d",i);
         rc += loadWindows(INIhandle, "Right", buffer, &rightstr[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Options %d",i);
         rc += loadOptions(INIhandle, "Upper Left", buffer, &upperleftopt[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Options %d",i);
         rc += loadOptions(INIhandle, "Lower Left", buffer, &lowerleftopt[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Options %d",i);
         rc += loadOptions(INIhandle, "Upper Right", buffer, &upperrightopt[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Options %d",i);
         rc += loadOptions(INIhandle, "Lower Right", buffer, &lowerrightopt[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Options %d",i);
         rc += loadOptions(INIhandle, "Up", buffer, &upopt[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Options %d",i);
         rc += loadOptions(INIhandle, "Down", buffer, &downopt[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Options %d",i);
         rc += loadOptions(INIhandle, "Left", buffer, &leftopt[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Options %d",i);
         rc += loadOptions(INIhandle, "Right", buffer, &rightopt[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Objects %d",i);
         rc += loadObjects(INIhandle, "Upper Left", buffer, &oupperleft[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Objects %d",i);
         rc += loadObjects(INIhandle, "Lower Left", buffer, &olowerleft[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Objects %d",i);
         rc += loadObjects(INIhandle, "Upper Right", buffer, &oupperright[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Objects %d",i);
         rc += loadObjects(INIhandle, "Lower Right", buffer, &olowerright[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Objects %d",i);
         rc += loadObjects(INIhandle, "Up", buffer, &oup[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Objects %d",i);
         rc += loadObjects(INIhandle, "Down", buffer, &odown[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Objects %d",i);
         rc += loadObjects(INIhandle, "Left", buffer, &oleft[i]);  }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Objects %d",i);
         rc += loadObjects(INIhandle, "Right", buffer, &oright[i]); }

      PrfQueryProfileSize(INIhandle, "Options", "Delay", &datasize);
         if (datasize == sizeof(delay))
            PrfQueryProfileData(INIhandle, "Options", "Delay", &delay, &datasize);

      PrfQueryProfileSize(INIhandle, "Options", "Margin", &datasize);
         if (datasize == sizeof(*margin))
            PrfQueryProfileData(INIhandle, "Options", "Margin", margin, &datasize);

      PrfCloseProfile(INIhandle);

      if (rc > 0)
      {
         sprintf(errormsg,"%d. Error loading INI file.",errornum++);
         WinInvalidateRect(hwndClient, &location, FALSE );
         return FALSE;
      }
   }
   else
   {
      sprintf(errormsg,"%d. Error opening INI file.",errornum++);
      WinInvalidateRect(hwndClient, &location, FALSE );
      return FALSE;
   }

   return TRUE;
}

/* save INI file */
BOOL saveINIfile()
{
   HINI INIhandle;

   if (INIhandle = PrfOpenProfile(hab,INIfile))
   {
      int i;
      char buffer[100];
      BOOL rc = 0;

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Windows %d",i);
         rc += storeWindows(INIhandle, "Upper Left", buffer, upperleft[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Windows %d",i);
         rc += storeWindows(INIhandle, "Lower Left", buffer, lowerleft[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Windows %d",i);
         rc += storeWindows(INIhandle, "Upper Right", buffer, upperright[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Windows %d",i);
         rc += storeWindows(INIhandle, "Lower Right", buffer, lowerright[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Windows %d",i);
         rc += storeWindows(INIhandle, "Up", buffer, up[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Windows %d",i);
         rc += storeWindows(INIhandle, "Down", buffer, down[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Windows %d",i);
         rc += storeWindows(INIhandle, "Left", buffer, left[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Windows %d",i);
         rc += storeWindows(INIhandle, "Right", buffer, right[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Substring %d",i);
         rc += storeWindows(INIhandle, "Upper Left", buffer, upperleftstr[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Substring %d",i);
         rc += storeWindows(INIhandle, "Lower Left", buffer, lowerleftstr[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Substring %d",i);
         rc += storeWindows(INIhandle, "Upper Right", buffer, upperrightstr[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Substring %d",i);
         rc += storeWindows(INIhandle, "Lower Right", buffer, lowerrightstr[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Substring %d",i);
         rc += storeWindows(INIhandle, "Up", buffer, upstr[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Substring %d",i);
         rc += storeWindows(INIhandle, "Down", buffer, downstr[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Substring %d",i);
         rc += storeWindows(INIhandle, "Left", buffer, leftstr[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Substring %d",i);
         rc += storeWindows(INIhandle, "Right", buffer, rightstr[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Options %d",i);
         rc += storeOptions(INIhandle, "Upper Left", buffer, upperleftopt[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Options %d",i);
         rc += storeOptions(INIhandle, "Lower Left", buffer, lowerleftopt[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Options %d",i);
         rc += storeOptions(INIhandle, "Upper Right", buffer, upperrightopt[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Options %d",i);
         rc += storeOptions(INIhandle, "Lower Right", buffer, lowerrightopt[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Options %d",i);
         rc += storeOptions(INIhandle, "Up", buffer, upopt[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Options %d",i);
         rc += storeOptions(INIhandle, "Down", buffer, downopt[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Options %d",i);
         rc += storeOptions(INIhandle, "Left", buffer, leftopt[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Options %d",i);
         rc += storeOptions(INIhandle, "Right", buffer, rightopt[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Objects %d",i);
         rc += storeObjects(INIhandle, "Upper Left", buffer, oupperleft[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Objects %d",i);
         rc += storeObjects(INIhandle, "Lower Left", buffer, olowerleft[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Objects %d",i);
         rc += storeObjects(INIhandle, "Upper Right", buffer, oupperright[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Objects %d",i);
         rc += storeObjects(INIhandle, "Lower Right", buffer, olowerright[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Objects %d",i);
         rc += storeObjects(INIhandle, "Up", buffer, oup[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Objects %d",i);
         rc += storeObjects(INIhandle, "Down", buffer, odown[i]); }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Objects %d",i);
         rc += storeObjects(INIhandle, "Left", buffer, oleft[i]);  }

      for(i = 0; i < ACTOPTCOUNT; i++) {
         sprintf(buffer,"Objects %d",i);
         rc += storeObjects(INIhandle, "Right", buffer, oright[i]); }

      rc += !PrfWriteProfileData(INIhandle, "Options","Delay", &delay, sizeof(delay));
      rc += !PrfWriteProfileData(INIhandle, "Options","Margin", margin, sizeof(*margin));

      PrfCloseProfile(INIhandle);

      if (rc > 0)
      {
         sprintf(errormsg,"%d. Error saving INI file",errornum++);
         WinInvalidateRect(hwndClient, &location, FALSE );
         return FALSE;
      }
   }
   else
   {
      sprintf(errormsg,"%d. Error opening INI file",errornum++);
      WinInvalidateRect(hwndClient, &location, FALSE );
      return FALSE;
   }

   return TRUE;
}

/* clears all data */
BOOL cleardata()
{
   int i;

   for(i = 0; i < ACTOPTCOUNT; i++) { free(upperleft[i]); upperleft[i] = NULL; }
   for(i = 0; i < ACTOPTCOUNT; i++) { free(lowerleft[i]); lowerleft[i] = NULL; }
   for(i = 0; i < ACTOPTCOUNT; i++) { free(upperright[i]); upperright[i] = NULL; }
   for(i = 0; i < ACTOPTCOUNT; i++) { free(lowerright[i]); lowerright[i] = NULL; }
   for(i = 0; i < ACTOPTCOUNT; i++) { free(up[i]); up[i] = NULL; }
   for(i = 0; i < ACTOPTCOUNT; i++) { free(down[i]); down[i] = NULL; }
   for(i = 0; i < ACTOPTCOUNT; i++) { free(left[i]); left[i] = NULL; }
   for(i = 0; i < ACTOPTCOUNT; i++) { free(right[i]); right[i] = NULL; }

   for(i = 0; i < ACTOPTCOUNT; i++) { free(upperleftstr[i]); upperleftstr[i] = NULL; }
   for(i = 0; i < ACTOPTCOUNT; i++) { free(lowerleftstr[i]); lowerleftstr[i] = NULL; }
   for(i = 0; i < ACTOPTCOUNT; i++) { free(upperrightstr[i]); upperrightstr[i] = NULL; }
   for(i = 0; i < ACTOPTCOUNT; i++) { free(lowerrightstr[i]); lowerrightstr[i] = NULL; }
   for(i = 0; i < ACTOPTCOUNT; i++) { free(upstr[i]); upstr[i] = NULL; }
   for(i = 0; i < ACTOPTCOUNT; i++) { free(downstr[i]); downstr[i] = NULL; }
   for(i = 0; i < ACTOPTCOUNT; i++) { free(leftstr[i]); leftstr[i] = NULL; }
   for(i = 0; i < ACTOPTCOUNT; i++) { free(rightstr[i]); rightstr[i] = NULL; }

   for(i = 0; i < ACTOPTCOUNT; i++) { memset(&upperleftopt[i],0,sizeof(WINOPT)); }
   for(i = 0; i < ACTOPTCOUNT; i++) { memset(&lowerleftopt[i],0,sizeof(WINOPT)); }
   for(i = 0; i < ACTOPTCOUNT; i++) { memset(&upperrightopt[i],0,sizeof(WINOPT)); }
   for(i = 0; i < ACTOPTCOUNT; i++) { memset(&lowerrightopt[i],0,sizeof(WINOPT)); }
   for(i = 0; i < ACTOPTCOUNT; i++) { memset(&upopt[i],0,sizeof(WINOPT)); }
   for(i = 0; i < ACTOPTCOUNT; i++) { memset(&downopt[i],0,sizeof(WINOPT)); }
   for(i = 0; i < ACTOPTCOUNT; i++) { memset(&leftopt[i],0,sizeof(WINOPT)); }
   for(i = 0; i < ACTOPTCOUNT; i++) { memset(&rightopt[i],0,sizeof(WINOPT)); }

   for(i = 0; i < ACTOPTCOUNT; i++) { free(oupperleft[i]); oupperleft[i] = NULL; }
   for(i = 0; i < ACTOPTCOUNT; i++) { free(olowerleft[i]); olowerleft[i] = NULL; }
   for(i = 0; i < ACTOPTCOUNT; i++) { free(oupperright[i]); oupperright[i] = NULL; }
   for(i = 0; i < ACTOPTCOUNT; i++) { free(olowerright[i]); olowerright[i] = NULL; }
   for(i = 0; i < ACTOPTCOUNT; i++) { free(oup[i]); oup[i] = NULL; }
   for(i = 0; i < ACTOPTCOUNT; i++) { free(odown[i]); odown[i] = NULL; }
   for(i = 0; i < ACTOPTCOUNT; i++) { free(oleft[i]); oleft[i] = NULL; }
   for(i = 0; i < ACTOPTCOUNT; i++) { free(oright[i]); oright[i] = NULL; }

   return TRUE;
}

/* loads a Window List "linear string array" from INI file */
BOOL loadWindows(HINI INIhandle, char *app, char *section, char **output)
{
   ULONG size = 0;

   PrfQueryProfileSize(INIhandle,app,section,&size);
   free(*output);
   if(size)
   {
      *output = malloc(size);
      PrfQueryProfileData(INIhandle,app,section,*output,&size);
   }
   else
      *output = NULL;

   return 0;
}

/* stores a Window List "linear string array" from INI file */
BOOL storeWindows(HINI INIhandle, char *app, char *section, char *input)
{
   BOOL rc = TRUE;
   ULONG size = 0;

   if(input && *input)
      rc = PrfWriteProfileData(INIhandle,app,section,input,_msize(input));
   else
   {
      PrfQueryProfileSize(INIhandle,app,section,&size);
      if(size)
         rc = PrfWriteProfileData(INIhandle,app,section,NULL,0);
   }

   return !rc;
}

BOOL loadOptions(HINI INIhandle, char *app, char *section, WINOPT *options)
{
   ULONG datasize;

   PrfQueryProfileSize(INIhandle, app, section, &datasize);
      if (datasize == sizeof(*options))
         PrfQueryProfileData(INIhandle, app, section, options, &datasize);
      else
         memset(options,0,sizeof(*options));

   return 0;
}

BOOL storeOptions(HINI INIhandle, char *app, char *section, WINOPT options)
{
   WINOPT zero = {0};
   BOOL rc = TRUE;
   ULONG size = 0;

   if(memcmp(&zero,&options,sizeof(options)))
      rc = PrfWriteProfileData(INIhandle,app,section,&options,sizeof(options));
   else
   {
      PrfQueryProfileSize(INIhandle,app,section,&size);
      if(size)
         rc = PrfWriteProfileData(INIhandle,app,section,NULL,0);
   }

   return !rc;
}

BOOL loadObjects(HINI INIhandle, char *app, char *section, HOBJECT **objlist)
{
   ULONG strsize;

   free(*objlist);
   *objlist = NULL;

   PrfQueryProfileSize(INIhandle,app,section,&strsize);
   if(strsize)
   {
      *objlist = (HOBJECT *) malloc(strsize);
      PrfQueryProfileData(INIhandle,app,section,*objlist,&strsize);
   }

   return 0;
}

BOOL storeObjects(HINI INIhandle, char *app, char *section, HOBJECT *objlist)
{
   ULONG size = 0;
   BOOL rc = TRUE;

   if(objlist)
      rc = PrfWriteProfileData(INIhandle,app,section,objlist,_msize(objlist));
   else
   {
      PrfQueryProfileSize(INIhandle,app,section,&size);
      if(size)
         rc = PrfWriteProfileData(INIhandle,app,section,NULL,0);
   }

   return !rc;
}

char *getINIfile(char *path)
{
   char *INIfile;

   if(path)
   {
      INIfile = malloc(strlen(path) + sizeof(INI_FILENAME) + 1);
      strcpy(INIfile,path);
      strcat(INIfile,"\\");
      strcat(INIfile,INI_FILENAME);
   }
   else
   {
      INIfile = malloc(sizeof(INI_FILENAME) + 1);
      strcpy(INIfile,INI_FILENAME);
   }

   return INIfile;
}

char *getEXEpath()
{
   ULONG PathLength;
   char *Path = NULL;

   /* query max path length */

   if (!DosQuerySysInfo(QSV_MAX_PATH_LENGTH, QSV_MAX_PATH_LENGTH, &PathLength, sizeof(ULONG)))
      Path = (char *) malloc(2*PathLength+1);

   /* multiplied by 2 to include the filename length too */

   if(Path)
   {
      PPIB ppib;
      PTIB ptib;

      DosGetInfoBlocks(&ptib, &ppib);
      DosQueryModuleName(ppib->pib_hmte, 2*PathLength+1, Path);

      *(strrchr(Path,'\\')) = 0;

      Path = (char *)realloc(Path, strlen(Path)+1);
   }

   return Path;
}

/* who knows? */
void _System ExeTrap()
{
    HookKill();
    DosExitList(EXLST_EXIT, (PFNEXITLIST)ExeTrap);
}

