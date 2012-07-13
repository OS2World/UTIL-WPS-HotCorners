:userdoc.
:docprof toc=12.
:title.Hot Corners Help
.*
:h1 res=1000 name=Usage.Usage
:p.
This section includes help on
:ul.
:li.:link reftype=hd res=1100.Buttons,:elink.
:li.the :link reftype=hd res=1200.Window List Page:elink.,
:li.the :link reftype=hd res=1300.Object Page:elink.,
:li.the :link reftype=hd res=1400.Option Page:elink., and
:li.the :link reftype=hd res=1500.About Page:elink..
:eul.

:h2 res=1100 name=Buttons.Buttons
:h3.:hp2.Save:ehp2.
:p.
Saves all settings from all pages, including Options.
:h3.:hp2.Reload:ehp2.
:p.
Reloads from the INI file on disk all settings for all pages, including Options.
:h3.:hp2.Clear:ehp2.
:p.
Clears all the data for all pages, except Options.
:h2 res=1200 name=WindowList.Window List
:h3.:hp2.Screen:ehp2.
:p.
This group contains buttons that represent screen borders and corners.  The are placed in square to facilitate navigation.  Any changes made to one of them will be conserved when switching to others.

:h3.:hp2.Float on Top and Bottom:ehp2.
:p.
When selecting one of these buttons, you will be able to select windows from the Window List that you want to make float on top or on bottom.

:h3.:hp2.Mouse Buttons:ehp2.
:p.
You can select a combination of either of them to setup all new settings.  Previous settings setup without those will be conserved.
:p.
When selecting Mouse Buttons, to activate the selected windows at screen border or corner, you will have to press the selected buttons down, and the Timeout from the Options page does not affect them.
:h3.:hp2.Alt, Ctrl and Shift:ehp2.
:p.
You can select a combination of either of them to setup all new settings.  Previous settings setup without those will be conserved.
:p.
When selecting at least one of them, to activate the selected windows at screen border or corner, these keys will have to be down before hitting the edges.
:h3.:hp2.Window List:ehp2.
:p.
This contains the complete list of all windows found in the window list.  You can select any combinations of them for any combination of the previous.  The "Refresh" button allows refreshing of the listbox.  :hp1.Note&colon.:ehp1. If a window that was selected does not appear anymore, until you change the selection, Hot Corners will keep it memory and watch for it.
:p.
Weird Window List entries you might encounter.
:ul.
:li.PMSPOOL.EXE
:p.:lm margin=10.Huum... beats me why PMSPOOL.EXE includes such an ugly name in the window list, maybe the printers need to find the spooler in the window list.  In any case, it cannot come foreground (invisible).

:li.Workplace Shell
:p.:lm margin=10.This entry is added by Workplace Shell process.  I don't know much more about it, except that it is not of any use to try to switch to it.  This is not the same as "Desktop".

:li." "
:p.:lm margin=10.This is cute gray elephant registration window, ARTCHRON, but it does not want to come up foreground (invisible also).

:li.Switch to
:p.:lm margin=10.This is the Window List!  You can make the window list come up this way.  It is always the last entry.
:eul.

:h3.:hp2.Window Substring Search:ehp2.
:p.
This listbox has the same function as the previous one, however instead of looking for an exact match, each case-sensitive substring specified here will be searched in the Window List when activated.
This allows dynamic titles, like agendas with time and date, to be recognized.

:h3.:hp2.Bring to pointer position:ehp2.
:p.
Once selected, windows that are switched to will be moved to the mouse pointer position.
:h3.:hp2.Minimize on pointer leave:ehp2.
:p.
Once selected, the windows that has been brought up will be minimized/hidden when the mouse pointer leaves their window frame after entering it.
:h3.:hp2.Push Back on pointer leave:ehp2.
:p.
Once selected, the windows that has been brought up will be pushed back down the Z order when the mouse pointer leaves their window frame after entering it.
:h2 res=1300 name=Objects.Objects
:h3.:hp2.Screen:ehp2.
:p.
This group contains buttons that represent screen borders and corners.  The are placed in square to facilitate navigation.  Any changes made to one of them will be conserved when switching to others.
:h3.:hp2.Mouse Buttons:ehp2.
:p.
You can select a combination of either of them to setup all new settings.  Previous settings setup without those will be conserved.
:p.
When selecting Mouse Buttons, to launch the selected objects at screen border or corner, you will have to press the selected buttons down, and the Timeout from the Options page does not affect them.
:h3.:hp2.Alt, Ctrl and Shift:ehp2.
:p.
You can select a combination of either of them to setup all new settings.  Previous settings setup without those will be conserved.
:p.
When selecting at least one of them, to launch the selected objects at screen border or corner, these keys will have to be down before hitting the edges.
:h3.:hp2.Object List:ehp2.
:p.
Objects included in this listbox will be opened on activation.  There are two methods to add objects.
:ul.
:li.Drag and Drop objects from the Workplace Shell directly into the listbox.
:li.Use the File Dialog to insert Data File Objects using the popup menu or the Insert key.
:eul.
:p.
You can remove objects using the popup menu or by pressing Delete.
:p.
Note also that the title showns in the listbox do not indicate the way used to search for the object.  If an object moves, it will be found back and with the new title.
:p.
:hp3.Important:ehp3.
:p.
Before panicking and sending me e-mail about objects not working, please make sure the following has been done first.
:ol.
:li."regagent.exe i" has been run to register WPSExtS.dll.
:li.WPSExtS.dll is loaded by the WPS (you can verify this by trying to delete it when Hot Corners is closed).  If not, the simpliest way is to copy it in x&colon.\os2\dll before running "regagent i".
:eol.
:h2 res=1400 name=Options.Options
:h3.:hp2.Timeout Delay:ehp2.
:p.
This option specifies the amount of time in milliseconds needed to be achieved by a still pointer to activate the corresponding corner or border selection.  It does not have any effect on the Mouse Button activation.
:h3.:hp2.Corner Margin:ehp2.
:p.
This options specifies the the square side length that is reasonable for Hot Corners to assume to be a corner.  For example, with a side of 10, Hot Corners will see a corner as a "small 10x10 pixels square on each of the four corners of the screen".
:h2 res=1500 name=About.About
:h3.:hp2.My E-Mail:ehp2.
:p.
Copy and Paste that address and send me an e-mail saying that you like this program!
:euserdoc.
