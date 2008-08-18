
 This folder (.local) should be put into C:\Document and Settings\<Current User> folder
 so efreet could find it. It countains several icons of different sizes and a
 theme description file.

 How to get this directory from scratch:
   1. Get Tango Icon library from CVS (http://tango.freedesktop.org/Tango_Icon_Library)
   2. Run the following batch outside of the CVS root directory (tango-icon-theme/)

xcopy *.png "C:\Documents and Settings\Admin\.local\share\icons\" /S /Y
cd tango-icon-theme
xcopy index.theme.in "C:\Documents and Settings\Admin\.local\share\icons\tango-icon-theme\index.theme" /Y 

      Where "Admin" is a <Current User>

   3. Modify C:\Documents and Settings\Admin\.local\share\icons\tango-icon-theme\
      index.theme:2

-_Name=Tango
-_Comment=Tango Icon Theme
+Name=Tango
+Comment=Tango Icon Theme

 
 That is enough to pass efreet icon tests.