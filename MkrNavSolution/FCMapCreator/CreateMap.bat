REM the JVM. With the below settings the heap size (Available memory for the application)
REM will range from 64 megabyte up to 720 megabyte.

::start javaw.exe -Djava.util.logging.config.file=logging.properties -Xms64M -Xmx1512M -cp "./FCNavMapCreator.jar;./lib/*.jar" net.flcnav.swing.OsmExtractionUI
::start C:\Java64\jdk1.7.0_07\bin\javaw.exe -Djava.util.logging.config.file=logging.properties -Xms4G -Xmx7G -cp "./FCNavMapCreator.jar;./lib/*.jar" net.flcnav.swing.OsmExtractionUI

start javaw.exe -Djava.util.logging.config.file=logging.properties -Xms4G -Xmx7G -cp "./FCNavMapCreator.jar;./lib/*.jar" net.flcnav.swing.OsmExtractionUI