#define MAINTABLE 1000
#define SUBTABLE_MAIN 2000

#define MAXSELECT 100

// used in Send hook where WinSetWindowPos and WinSetActiveWindow cannot be used
#define ID_SETZORDER       WM_USER+2
#define ID_SETACTIVEWINDOW WM_USER+3

/* main client window frame and notebook */
#define ID_MAIN 1
#define ID_NB 42

#define ID_STOPTIMER 666
#define ID_NOTIMER 999

#define ID_DUMMY 20000

/* notebook page frame */
#define ID_WINDOWLISTPAGE 100

   #define SUBTABLE_LISTBOX 101
   #define SUBTABLE_SCREEN 102
   #define SUBTABLE_MBS_KEYS 103
   #define SUBTABLE_OPTIONS 104
   #define SUBTABLE_SUBSTRING 105

#define ID_OBJECTPAGE 110
#define ID_OPTIONPAGE 120
#define ID_ABOUTPAGE 130

/* radio buttons */
#define ID_UPPERLEFT 200
#define ID_LOWERLEFT 201
#define ID_UPPERRIGHT 203
#define ID_LOWERRIGHT 204

#define ID_UP 205
#define ID_DOWN 206
#define ID_LEFT 207
#define ID_RIGHT 208

#define ID_FLOATTOP 209
#define ID_FLOATBOTTOM 210

#define ID_OPENONTOP 211  // not a button

/* main listbox */

#define ID_LISTBOX 300

/* substring item group */
#define ID_STRING_ADD 400
#define ID_STRING_DEL 401
#define ID_STRING_FIELD 402
#define ID_STRING_LIST 403

/* checkboxes and button */

#define ID_MOVE 500
#define ID_MIN 501
#define ID_PUSHBACK 502

#define ID_ALT 503
#define ID_CTRL 504
#define ID_SHIFT 505
#define ID_MB1 506
#define ID_MB2 507
#define ID_MB3 508

#define ID_REFRESH 509

/* object listbox pop-up menu */
#define ID_OPOPUPMENU 600
#define ID_ADD        601
#define ID_DELETE     602

/* options */
#define ID_SPIN1 700
#define ID_SPINMARGIN 701

/* main window push buttons */
#define ID_SAVEPB 800
#define ID_RELOADPB 801
#define ID_CLEARPB 802
#define ID_HELPPB 803

#define ID_NBFOCUS 900
#define ID_SAVEFOCUS 901

/* notebook page info structure */

typedef struct _NBPAGE
{
    PFNWP    pfnwpDlg;              /* Window procedure address for the dialog */
    PSZ      szStatusLineText;      /* Text to go on status line */
    PSZ      szTabText;             /* Text to go on major tab */
    ULONG    idDlg;                 /* ID of the dialog box for this page */
    BOOL     skip;                  /* skip this page (for major pages with minor pages) */
    USHORT   usTabType;             /* BKA_MAJOR or BKA_MINOR */
    ULONG    ulPageId;              /* notebook page ID */
    HWND     hwnd;                  /* set when page frame is loaded */

} NBPAGE, *PNBPAGE;

/* window list options */
typedef struct _WINOPT
{
   int move : 1;
   int minimize : 1;
   int pushback : 1;
}  WINOPT;

/* temporary list to keep track of windows to minimize or push back */
typedef struct _HWNDOPT
{
   HWND hwnd;           /* the windows to minimize/push back on pointer leave */
   int wasinframe : 1;  /* to know if the moose ever came in the frame
                           if it is initially not moving over it */
   int pushback : 1;
   int minimize : 1;
}  HWNDOPT;

/* activation options needed to know where in the arrays is the right data */
typedef union _ACTOPT
{
   char number;
   struct
   {
      int mb1 : 1;
      int mb2 : 1;
      int mb3 : 1;
      int alt : 1;
      int ctrl : 1;
      int shift : 1;
   } bit;
} ACTOPT;

#define ACTOPTCOUNT 64

/* DLL hook functions */
BOOL EXPENTRY HotCornersHookInput(HAB hab,PQMSG pqmsg,USHORT usRemove );
VOID EXPENTRY HotCornersHookSend(HAB hab, PSMHSTRUCT psmh, BOOL fInterTask);
BOOL EXPENTRY HookInit(HWND hwnd, HWNDOPT **sethwndopt, SHORT **setmargin, HWND **sethwndtop, HWND **sethwndbottom, BOOL **setopenontop);
BOOL EXPENTRY HookKill(void);
