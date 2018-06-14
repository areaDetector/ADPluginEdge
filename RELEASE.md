ADPluginEdge Releases
=====================

The latest untagged master branch can be obtained at
https://github.com/areaDetector/ADPluginEdge.

Tagged source code releases from R2-0 onward can be obtained at 
https://github.com/areaDetector/ADPluginEdge/releases.

The versions of EPICS base, asyn, and other synApps modules used for each release can be obtained from 
the EXAMPLE_RELEASE_PATHS.local, EXAMPLE_RELEASE_LIBS.local, and EXAMPLE_RELEASE_PRODS.local
files respectively, in the configure/ directory of the appropriate release of the 
[top-level areaDetector](https://github.com/areaDetector/areaDetector) repository.


Release Notes
=============
R1-3alpha0 (14-June-2018)
----
* Converted return call of NDEdgeConfigure to include thread start
* Add autoconverted OPI file
* Add protection against bad input
* Add handling of RGB3 input
* Fail more gracefully if input is not required format

R1-2 (05-July-2017)
----
* Changed number of arguments passed to NDPluginDriver base class in constructor for ADCore R3-0.
* Changed size of medm screen for new version of NDPluginBase.adl in ADCore R3-0.


R1-1 (20-February-2017)
----
* Added plugin version information.
* Added release tags for R1-0 and R1-1.
* Increased size of medm screen for new version of NDPluginBase.adl in ADCore R2-6.
* No longer pass number of plugin parameters, not required in ADCore R2-6.


R1-0 (18-September-2015)
----
* Original release.
