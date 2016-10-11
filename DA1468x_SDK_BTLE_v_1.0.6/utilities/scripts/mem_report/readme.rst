======================
Memory Reporting Tools
======================

These are Python scripts that generate Excel files containing memory utilization
reports for projects created with the SDK.

Prerequisites
-------------

The scripts have been developped using Python 2.7. Other than that the only 
thing needed is XlsxWriter_

.. _XlsxWriter: http://xlsxwriter.readthedocs.io/

XlsWriter Windows installation::

> c:\Python27\python.exe –m pip install XlsxWriter

XlsWriter Linux installation::

> python pip install XlsxWriter

Usage
-----

Memory Reporter
~~~~~~~~~~~~~~~

Memory Reporter takes as input a linker map file from a project and generates an
Excel file containing the memory utilization report for it::

> python mem_reporter.py <path to map file>

Note that the output Excel name will consist of the directory name containing 
the map file and the name of the map file (so it ends up something like 
Debug_ftdf_dts.xlsx), based on the assumption that map files are inside
Eclipe's build configuration folders. Have that in mind to understand the
implications of moving the map file elsewhere before running the script.

Create Sunmmary
~~~~~~~~~~~~~~~
It is also possible to create an Excel file containing a brief summary of the
sizes of several projects. For that you will need to run this script::

> python create_summary.py <path that contains projects' map files> <name of output Excel file>

Known Issues
------------

This concerns mostly Linux users. You may see that some cells containing formulas have ‘0’s 
instead of the expected number. Press Ctrl+Shft+F9 to fix this or try Tools->Options-> under 
LibreOfficeCalc->Formula and in “Recalculation on file load” select “Always recalculate” for 
a permanent solution.

`Here you can find details for this issue`_ (it applies also for some other spreadsheet 
applications)

.. _Here you can find details for this issue: http://stackoverflow.com/questions/32205927/xlsxwriter-and-libreoffice-not-showing-formulas-result

