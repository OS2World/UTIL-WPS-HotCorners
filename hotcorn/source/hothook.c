/* Hot Corners 2.2 (C) 1997-1999 Samuel Audet <guardia@cam.org> */

#define INCL_WIN
#define INCL_DOSMODULEMGR

#include <os2.h>
#include <memory.h>
#include "hotcorn.h"

POINTL old,   /* do nothing if the current and previous pointer is the same to
                 prevent user from overloading the message queue just by draging
                 the mouse over */

       screen;  /* screen size */

HMODULE hmDll; /* us */
HWND mainhwnd; /* our client EXE hwnd */
HAB mainhab;
HWND desktophwnd;

HWNDOPT hwndopt[MAXSELECT];
HWND    hwndtop[MAXSELECT], hwndbottom[MAXSELECT];
BOOL openontop = FALSE;

SHORT margin;

ACTOPT actopt;

HWND bull = 0;

BOOL EXPENTRY HotCornersHookInput(HAB hab,PQMSG pqmsg,USHORT usRemove )
{
   if( (pqmsg->msg == WM_BUTTON1DOWN) ||
       (pqmsg->msg == WM_BUTTON2DOWN) ||
       (pqmsg->msg == WM_BUTTON3DOWN)  )
   {
      memset(&actopt,0,sizeof(actopt));
      if(WinGetKeyState(HWND_DESKTOP,VK_BUTTON1) & 0x8000)
         actopt.bit.mb1 = 1;
      if(WinGetKeyState(HWND_DESKTOP,VK_BUTTON2) & 0x8000)
         actopt.bit.mb2 = 1;
      if(WinGetKeyState(HWND_DESKTOP,VK_BUTTON3) & 0x8000)
         actopt.bit.mb3 = 1;
      if(WinGetKeyState(HWND_DESKTOP,VK_ALT) & 0x8000)
         actopt.bit.alt = 1;
      if(WinGetKeyState(HWND_DESKTOP,VK_CTRL) & 0x8000)
         actopt.bit.ctrl = 1;
      if(WinGetKeyState(HWND_DESKTOP,VK_SHIFT) & 0x8000)
         actopt.bit.shift = 1;

   /* check if we've hit the jack pot and post so */

      /* corners */
      if ((pqmsg->ptl.x <= margin) && (pqmsg->ptl.y >= screen.y - 1 - margin))
         WinPostMsg(mainhwnd,WM_USER,MPFROM2SHORT(ID_UPPERLEFT,ID_NOTIMER),MPFROMCHAR(actopt.number));
      else if ((pqmsg->ptl.x <= margin) && (pqmsg->ptl.y <= margin))
         WinPostMsg(mainhwnd,WM_USER,MPFROM2SHORT(ID_LOWERLEFT,ID_NOTIMER),MPFROMCHAR(actopt.number));
      else if ((pqmsg->ptl.x >= screen.x - 1 - margin) && (pqmsg->ptl.y >= screen.y - 1 - margin))
         WinPostMsg(mainhwnd,WM_USER,MPFROM2SHORT(ID_UPPERRIGHT,ID_NOTIMER),MPFROMCHAR(actopt.number));
      else if ((pqmsg->ptl.x >= screen.x - 1 - margin) && (pqmsg->ptl.y <= margin))
         WinPostMsg(mainhwnd,WM_USER,MPFROM2SHORT(ID_LOWERRIGHT,ID_NOTIMER),MPFROMCHAR(actopt.number));
        /* borders */
      else if (pqmsg->ptl.y == screen.y - 1)
         WinPostMsg(mainhwnd,WM_USER,MPFROM2SHORT(ID_UP,ID_NOTIMER),MPFROMCHAR(actopt.number));
      else if (pqmsg->ptl.y == 0)
         WinPostMsg(mainhwnd,WM_USER,MPFROM2SHORT(ID_DOWN,ID_NOTIMER),MPFROMCHAR(actopt.number));
      else if (pqmsg->ptl.x == screen.x - 1)
         WinPostMsg(mainhwnd,WM_USER,MPFROM2SHORT(ID_RIGHT,ID_NOTIMER),MPFROMCHAR(actopt.number));
      else if (pqmsg->ptl.x == 0)
         WinPostMsg(mainhwnd,WM_USER,MPFROM2SHORT(ID_LEFT,ID_NOTIMER),MPFROMCHAR(actopt.number));
   }

   else if ((pqmsg->msg == WM_MOUSEMOVE) && ((pqmsg->ptl.y != old.y) || (pqmsg->ptl.x != old.x)))
   {
      static int i; /* loop index */

      memset(&actopt,0,sizeof(actopt));
   /* check the alt, ctrt, shift keystate */
      if(WinGetKeyState(HWND_DESKTOP,VK_ALT) & 0x8000)
         actopt.bit.alt = 1;
      if(WinGetKeyState(HWND_DESKTOP,VK_CTRL) & 0x8000)
         actopt.bit.ctrl = 1;
      if(WinGetKeyState(HWND_DESKTOP,VK_SHIFT) & 0x8000)
         actopt.bit.shift = 1;

   /* check if we've hit something and post so */

      /* corners */
      if ((pqmsg->ptl.x <= margin) && (pqmsg->ptl.y >= screen.y - 1 - margin))
         WinPostMsg(mainhwnd,WM_USER,MPFROMSHORT(ID_UPPERLEFT),MPFROMCHAR(actopt.number));
      else if ((pqmsg->ptl.x <= margin) && (pqmsg->ptl.y <= margin))
         WinPostMsg(mainhwnd,WM_USER,MPFROMSHORT(ID_LOWERLEFT),MPFROMCHAR(actopt.number));
      else if ((pqmsg->ptl.x >= screen.x - 1 - margin) && (pqmsg->ptl.y >= screen.y - 1 - margin))
         WinPostMsg(mainhwnd,WM_USER,MPFROMSHORT(ID_UPPERRIGHT),MPFROMCHAR(actopt.number));
      else if ((pqmsg->ptl.x >= screen.x - 1 - margin) && (pqmsg->ptl.y <= margin))
         WinPostMsg(mainhwnd,WM_USER,MPFROMSHORT(ID_LOWERRIGHT),MPFROMCHAR(actopt.number));
      else
      {
       /* borders */
         if (pqmsg->ptl.y != old.y)
         {
            if (pqmsg->ptl.y == screen.y - 1)
               WinPostMsg(mainhwnd,WM_USER,MPFROMSHORT(ID_UP),MPFROMCHAR(actopt.number));
            else if (pqmsg->ptl.y == 0)
               WinPostMsg(mainhwnd,WM_USER,MPFROMSHORT(ID_DOWN),MPFROMCHAR(actopt.number));
         }

         if (pqmsg->ptl.x != old.x)
         {
            if (pqmsg->ptl.x == screen.x - 1)
               WinPostMsg(mainhwnd,WM_USER,MPFROMSHORT(ID_RIGHT),MPFROMCHAR(actopt.number));
            else if (pqmsg->ptl.x == 0)
               WinPostMsg(mainhwnd,WM_USER,MPFROMSHORT(ID_LEFT),MPFROMCHAR(actopt.number));
         }
      }

   /* check for a human error where the user decided he didn't want that activated
      after all and moved off. */

      /* corners */

      if(((old.x <= margin) && (old.y >= screen.y - 1 - margin)) &&
        !((pqmsg->ptl.x <= margin) && (pqmsg->ptl.y >= screen.y - 1 - margin)))
         WinPostMsg(mainhwnd,WM_USER,MPFROM2SHORT(ID_UPPERLEFT,ID_STOPTIMER),NULL);
      else if(((old.x <= margin) && (old.y <= margin)) &&
             !((pqmsg->ptl.x <= margin) && (pqmsg->ptl.y <= margin)))
         WinPostMsg(mainhwnd,WM_USER,MPFROM2SHORT(ID_LOWERLEFT,ID_STOPTIMER),NULL);
      else if(((old.x >= screen.x - 1 - margin) && (old.y >= screen.y - 1 - margin)) &&
             !((pqmsg->ptl.x >= screen.x - 1 - margin) && (pqmsg->ptl.y >= screen.y - 1 - margin)))
         WinPostMsg(mainhwnd,WM_USER,MPFROM2SHORT(ID_UPPERRIGHT,ID_STOPTIMER),NULL);
      else if(((old.x >= screen.x - 1 - margin) && (old.y <= margin)) &&
             !((pqmsg->ptl.x >= screen.x - 1 - margin) && (pqmsg->ptl.y <= margin)))
         WinPostMsg(mainhwnd,WM_USER,MPFROM2SHORT(ID_LOWERRIGHT,ID_STOPTIMER),NULL);
      else
      {
       /* borders */
         if (pqmsg->ptl.y != old.y)
         {
            if((old.y == screen.y - 1) &&
               (pqmsg->ptl.y != screen.y - 1))
               WinPostMsg(mainhwnd,WM_USER,MPFROM2SHORT(ID_UP,ID_STOPTIMER),NULL);
            else if((old.y == 0) &&
                   (pqmsg->ptl.y != 0))
               WinPostMsg(mainhwnd,WM_USER,MPFROM2SHORT(ID_DOWN,ID_STOPTIMER),NULL);
         }

         if (pqmsg->ptl.x != old.x)
         {
            if((old.x == screen.x - 1) &&
              (pqmsg->ptl.x != screen.x - 1))
               WinPostMsg(mainhwnd,WM_USER,MPFROM2SHORT(ID_RIGHT,ID_STOPTIMER),NULL);
            else if((old.x == 0) &&
                   (pqmsg->ptl.x != 0))
               WinPostMsg(mainhwnd,WM_USER,MPFROM2SHORT(ID_LEFT,ID_STOPTIMER),NULL);
         }
      }

      /* this checks if we have a frame to monitor, then checks if the mouse is NOT
         within the border of the frames (or its children in any case) and if we WERE
         within this frame at the last mouse movement */

      if(pqmsg->hwnd)
      for(i = 0; hwndopt[i].hwnd ; i++)
      {
         if(!WinIsChild(pqmsg->hwnd,hwndopt[i].hwnd) && hwndopt[i].wasinframe)
         {
            static HENUM henum;
            static HWND temp;

            if(hwndopt[i].minimize)
            {
               WinSetWindowPos(hwndopt[i].hwnd, 0, 0, 0, 0, 0, SWP_HIDE);

               henum = WinBeginEnumWindows(HWND_DESKTOP);
               /* we get a whole fuckin bunch of invisible windows before the first visible one on top */
               while(temp = WinGetNextWindow(henum))
               {
                  if(WinIsChild(hwndopt[i].hwnd,temp))
                     temp = WinGetNextWindow(henum);

                  if(WinSetActiveWindow(HWND_DESKTOP, temp)) break;
               }
               WinEndEnumWindows(henum);
            }

            if(hwndopt[i].pushback)
            {
               WinSetWindowPos(hwndopt[i].hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_ZORDER);

               henum = WinBeginEnumWindows(HWND_DESKTOP);
               /* we get a whole fuckin bunch of invisible windows before the first visible one on top */
               while(temp = WinGetNextWindow(henum))
               {
                  if(WinIsChild(hwndopt[i].hwnd,temp))
                     temp = WinGetNextWindow(henum);

                  if(WinSetActiveWindow(HWND_DESKTOP, temp)) break;
               }
               WinEndEnumWindows(henum);
            }

            for(; hwndopt[i+1].hwnd; i++)
               hwndopt[i] = hwndopt[i+1];

            hwndopt[i].hwnd = 0;
         }
         hwndopt[i].wasinframe = WinIsChild(pqmsg->hwnd,hwndopt[i].hwnd);
      }

      old = pqmsg->ptl;
   }

   return(FALSE);
}

VOID EXPENTRY HotCornersHookSend(HAB hab, PSMHSTRUCT psmh, BOOL fInterTask)
{
   /* filled up with windows that later get the desktop as parent, but that should
      be on top of floating windows, like menus */
   static HWND skiphwnd[10] = {0}, newhwnd[10] = {0};
   static int  skiphwndi    =  0 , newhwndi    =  0;

   switch(psmh->msg)
   {
      case WM_CREATE:
      {
         int i;
         CREATESTRUCT *createstruct = PVOIDFROMMP(psmh->mp2);
         /* can you believe EPM actually sends a NULL pointer ?? */
         if(!createstruct) break;

#ifdef crazy
         if(desktophwnd == WinQueryWindow(psmh->hwnd, QW_PARENT))
            WinPostMsg(mainhwnd,WM_USER+1,MPFROMLONG(psmh->hwnd),MPFROMLONG(0x666));
#endif
         if(desktophwnd == createstruct->hwndParent)
         {
            newhwnd[newhwndi++] = psmh->hwnd;
            if(newhwndi >= sizeof(newhwnd)/sizeof(newhwnd[0])) newhwndi = 0;
            break;
         }
         else for(i=1; hwndtop[i]; i++)
            if(WinIsChild(createstruct->hwndParent, hwndtop[i]))
            {
               skiphwnd[skiphwndi++] = psmh->hwnd;
               if(skiphwndi >= sizeof(skiphwnd)/sizeof(skiphwnd[0])) skiphwndi = 0;
            }
         break;
      }

      case WM_DESTROY:
      {
         int i;

         if(desktophwnd == WinQueryWindow(psmh->hwnd,QW_PARENT))
         {
            for(i=0; hwndtop[i]; i++)
               if(psmh->hwnd == hwndtop[i])
               {
                  WinPostMsg(mainhwnd,WM_USER,MPFROMSHORT(ID_FLOATTOP),MPFROMCHAR(0));
                  break;
               }

            for(i=0; hwndbottom[i]; i++)
               if(psmh->hwnd == hwndbottom[i])
               {
                  WinPostMsg(mainhwnd,WM_USER,MPFROMSHORT(ID_FLOATBOTTOM),MPFROMCHAR(0));
                  break;
               }
         }

         for(i=0; i < sizeof(skiphwnd)/sizeof(skiphwnd[0]); i++)
            if(psmh->hwnd == skiphwnd[i])
            {
               skiphwnd[i] = 0;
               break;
            }

         for(i=0; i < sizeof(newhwnd)/sizeof(newhwnd[0]); i++)
            if(psmh->hwnd == newhwnd[i])
            {
               newhwnd[i] = 0;
               break;
            }

         break;
      }

      // this code is slow and is only used if some windows need to float on top
      // so that is a another window gets over a floating window it is reput on top.
      case WM_WINDOWPOSCHANGED:

         if(hwndtop[1] != 0)
         if(desktophwnd == WinQueryWindow(psmh->hwnd,QW_PARENT))
         {
            HWND temp;
            int i;
            HENUM henum;

            /* check if it's a window we need to skip */
            for(i=0; i < sizeof(skiphwnd)/sizeof(skiphwnd[0]); i++)
               if(psmh->hwnd == skiphwnd[i])
                  goto done;

            henum = WinBeginEnumWindows(HWND_DESKTOP);
            while(temp = WinGetNextWindow(henum))
            {
               for(i=1; hwndtop[i]; i++)
                  if(temp == hwndtop[i])
                     goto done;

               if(WinIsWindowShowing(temp))
               {
                  for(i=1; hwndtop[i]; i++)
                     /* post msg to client since send hook doesn't like winsetwindowpos */
                     WinPostMsg(mainhwnd,ID_SETZORDER,MPFROMLONG(hwndtop[i]),MPFROMLONG(hwndtop[i-1]));
                  goto done;
               }
            }
            WinEndEnumWindows(henum);
         }

      case WM_ADJUSTWINDOWPOS:
      {
         SWP *swp = PVOIDFROMMP(psmh->mp1);

         /* we only process windows that have the desktop window as parent */
         if(swp && (desktophwnd == WinQueryWindow(swp->hwnd,QW_PARENT)))
         {
            HWND lasttop = 0;
            int i;

            for(i = 0; i < sizeof(newhwnd)/sizeof(newhwnd[0]); i++)
               if((swp->hwnd == newhwnd[i]) && (swp->fl & SWP_SHOW))
               {
                  newhwnd[i] = 0;
                  if(openontop)
                  {
                     if(swp->hwndInsertBehind != HWND_TOP || !(swp->fl & SWP_ZORDER) )
                        WinPostMsg(mainhwnd,ID_SETACTIVEWINDOW,swp->hwnd,HWND_TOP);
                     swp->hwndInsertBehind = HWND_TOP;
                     swp->fl |= SWP_ZORDER;
                     // WinFocusChange(HWND_DESKTOP,swp->hwnd,0);
                  }

                  WinPostMsg(mainhwnd,WM_USER,MPFROMSHORT(ID_FLOATTOP),MPFROMCHAR(0));
                  WinPostMsg(mainhwnd,WM_USER,MPFROMSHORT(ID_FLOATBOTTOM),MPFROMCHAR(0));
               }

#ifdef crazy
            WinPostMsg(mainhwnd,WM_USER+1,MPFROMLONG(psmh->hwnd),MPFROMLONG(0x6666));
            WinPostMsg(mainhwnd,WM_USER+1,MPFROMLONG(swp->hwndInsertBehind),MPFROMLONG(swp->fl));
#endif

            /* check if it's a window we need to skip */
            for(i=0; i < sizeof(skiphwnd)/sizeof(skiphwnd[0]); i++)
               if(swp->hwnd == skiphwnd[i])
                  goto done;

            /* check if it's a window we need to make float on top */
            for(i=1; hwndtop[i]; i++)
            {
               if(hwndtop[i] == swp->hwnd)
               {
                  swp->fl |= SWP_ZORDER;
                  if( (swp->fl & SWP_ACTIVATE) || (swp->fl & SWP_FOCUSACTIVATE) )
                  {
                     swp->fl &= ~SWP_ACTIVATE & ~SWP_FOCUSACTIVATE;
                     WinFocusChange(HWND_DESKTOP,swp->hwnd,FC_NOBRINGTOTOP);
                  }
                  swp->hwndInsertBehind = hwndtop[i-1]; /* 0 is set to HWND_TOP, see HookInit() */
                  goto done;
               }
               else
               {
/*                  ULONG tid;
                  ULONG pid;
                  MQINFO mqinfo = {0};

                  mqinfo.cb = sizeof(mqinfo);

                  WinQueryWindowProcess(hwndtop[i],&pid,&tid);
                  WinQueryQueueInfo(HMQ_CURRENT, &mqinfo, sizeof(mqinfo));
                  if(tid == mqinfo.tid)
                     goto done; */
               }
            }
            if(--i)
               lasttop = hwndtop[i];

            /* the bottom most are at the end */
            for(i=1; hwndbottom[i]; i++);
            for(i-- ; i > 0 ; i--)
               if(hwndbottom[i] == swp->hwnd)
               {
                  swp->fl |= SWP_ZORDER;
                  if( (swp->fl & SWP_ACTIVATE) || (swp->fl & SWP_FOCUSACTIVATE) )
                  {
                     swp->fl &= ~SWP_ACTIVATE & ~SWP_FOCUSACTIVATE;
                     WinFocusChange(HWND_DESKTOP,swp->hwnd,FC_NOBRINGTOTOP);
                  }
                  if(hwndbottom[i+1])
                     swp->hwndInsertBehind = WinQueryWindow(hwndbottom[i+1],QW_PREVTOP);
                  else
                     swp->hwndInsertBehind = hwndbottom[0]; /* ya I know, kinda weird, but this isn't straight forward in the first place */
                  goto done;
               }

            /* if it's some other window, let's place them properly */
            if(lasttop)
            {
               /* looks like windows being opened need that */
               if(swp->fl & SWP_SHOW)
               {
                  swp->fl |= SWP_ZORDER;
                  swp->hwndInsertBehind = lasttop;
               }
               else if(swp->fl & SWP_SIZE)
                  swp->hwndInsertBehind = lasttop;

               if( (swp->fl & SWP_ACTIVATE) || (swp->fl & SWP_FOCUSACTIVATE) )
               {
                  swp->fl |= SWP_ZORDER;
                  swp->fl &= ~SWP_ACTIVATE & ~SWP_FOCUSACTIVATE;
                  swp->hwndInsertBehind = lasttop;
                  WinFocusChange(HWND_DESKTOP,swp->hwnd,FC_NOBRINGTOTOP);
                  goto done;
               }
               else for(i=0; hwndtop[i] != lasttop; i++)
                  if(swp->hwndInsertBehind == hwndtop[i])
                  {
                     swp->fl |= SWP_ZORDER;
                     swp->hwndInsertBehind = lasttop;
                     goto done;
                  }
            }

            if(hwndbottom[1])
               for(i=0; hwndbottom[i]; i++)
                  if(swp->hwndInsertBehind == hwndbottom[i])
                  {
                     HWND upmosthwnd = WinQueryWindow(hwndbottom[1],QW_PREVTOP);

                     if( (swp->fl & SWP_ACTIVATE) || (swp->fl & SWP_FOCUSACTIVATE) )
                     {
                        swp->fl &= ~SWP_ACTIVATE & ~SWP_FOCUSACTIVATE;
                        WinFocusChange(HWND_DESKTOP,swp->hwnd,FC_NOBRINGTOTOP);
                     }

                     if(upmosthwnd == swp->hwnd)
                        swp->fl &= ~SWP_ZORDER;
                     else
                     {
                        swp->fl |= SWP_ZORDER;
                        swp->hwndInsertBehind = upmosthwnd;
                     }
                     goto done;
                  }

         }
         break;
      }
   }
   done : ;

}


BOOL EXPENTRY HookInit(HWND hwnd, HWNDOPT **sethwndopt, SHORT **setmargin, HWND **sethwndtop, HWND **sethwndbottom, BOOL **setopenontop)
{
   mainhwnd = hwnd;
   mainhab = WinQueryAnchorBlock(hwnd);

   margin = 10;
   memset(hwndopt,0,sizeof(hwndopt));
   memset(hwndtop,0,sizeof(hwndtop));
   memset(hwndtop,0,sizeof(hwndbottom));
   hwndtop[0] = HWND_TOP;  /* hiearchy of floating windows, see WM_ADJUSTFRAMEPOS and refill() */
   hwndbottom[0] = HWND_BOTTOM;
/*   hwndtop[0] = WinQueryWindow(HWND_DESKTOP,QW_TOP); */
   /* give the addresses to the main client EXE */
   *sethwndopt = hwndopt;
   *setmargin = &margin;
   *sethwndtop = hwndtop;
   *sethwndbottom = hwndbottom;
   *setopenontop = &openontop;

   /* let's get desktop coordinates while we are there */
   screen.x = WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
   screen.y = WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);

   /* let's also get initial pointer position */
   WinQueryPointerPos(HWND_DESKTOP,&old);

   /* needed for float on top/bottom to filter windows */
   desktophwnd = WinQueryDesktopWindow(mainhab,NULLHANDLE);

   /* setting hook */
   if(!DosQueryModuleHandle("hothook", &hmDll) &&
      WinSetHook(mainhab, NULLHANDLE, HK_INPUT, (PFN)HotCornersHookInput, hmDll) &&
      WinSetHook(mainhab, NULLHANDLE, HK_SENDMSG, (PFN)HotCornersHookSend, hmDll))
      return TRUE;
   else
      return FALSE;
}

BOOL EXPENTRY HookKill()
{
   if(WinReleaseHook(mainhab, NULLHANDLE, HK_INPUT, (PFN)HotCornersHookInput, hmDll) &&
      WinReleaseHook(mainhab, NULLHANDLE, HK_SENDMSG, (PFN)HotCornersHookSend, hmDll) )
      return TRUE;
   else
      return FALSE;
}
