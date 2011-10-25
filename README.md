Reflect for Palm OS
===================

Reflect turns your handheld into a mirror. By turning the screen black, you can
see yourself on the shiny, reflective screen of most handhelds. Reflect has many
features and open source code.


Features
--------

* Animations that black out the screen.
* Preferences that are saved between sessions.
* Monochrome, greyscale, color, and hi-res support.
* Infrared beaming from within the application.
* Runs on Palm OS 3.0 or greater.
* Free, open source software.


Download
--------

* [reflect-palmos.zip][zip]
* [reflect-palmos.tar.gz][tar]

[zip]: https://github.com/jbarker/reflect-palmos/zipball/master
[tar]: https://github.com/jbarker/reflect-palmos/tarball/master


Running
-------

Load the release file -- Reflect.prc -- onto your Palm OS device. You may use
any supported file transfer method, such as USB or Bluetooth, offered by your
operating system or application software like Palm Desktop, HotSync Manager,
or Install Tool.


Requirements
------------

This application requires Palm OS 3.0 or greater.

Some optional functionality requires the following:

* An infrared port, for the Beam command.
* Palm OS 3.5, for Graffiti ShortCut icons.


Development
-----------

This version of Reflect was created using:

* Garnet OS Development Suite v1.2.1.02
* Palm OS 5 SDK R4

This version of Reflect was tested with:

* Palm OS Garnet Simulator v5.4.0.1
* Palm OS Emulator v3.5


Release History
---------------

v1.2 - 05 Jul 2008

* Updated default preferences
  * Splash screen on load is now disabled
* Minor code clean-up
* Switched to Simplified BSD License. Reflect remains free, open source
  software.

v1.1.1 - 16 Oct 2005

* Updated application category
  * Reflect now installs to the "Main" category.
* Updated UI to better conform to Palm guidelines
  * Slightly updated position and size of multiple elements based on validation
    in Palm OS Resource Editor.

v1.1 - 23 Aug 2002

* Added do a Refresh when the main screen area is tapped.
* Added do a Refresh when the Page Down key is pressed.
* Added go to Preferences when the Page Up key is pressed.
* Updated Preferences
  * Added "Random" Style option as the default value.
  * Added "Show splash screen on startup" checkbox. Default is checked.
  * Added "Save prefs across sessions" checkbox. Default is checked.
  * Added "Load Default Values" option with warning.
* Updated icons and bitmaps
  * Added support for HiRes screens.
  * Added support for greyscale screens.
* Updated UI to better conform to Palm guidelines
  * Removed Exit menu command.
  * Updated Grafitti Shortcuts to match other Palm applications.
  * Updated menu names and rearranged commands.
  * Updated buttons to system standard sizes and spacing.

v1.0 - 16 Mar 2002

* Reflect now refreshes the screen after preferences are changed.
* Added two style choices: None and Outside to Inside.
* Updated style choice Inside to Outside.  It now draws a rectangle instead of
  a circle.
* Organized the source code for future changes.

v0.5 - 19 Sep 2000

* Initial public release.
