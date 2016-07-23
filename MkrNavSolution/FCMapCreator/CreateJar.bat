cd bin
if exist MANIFEST.MF del MANIFEST.MF
cd ..
copy MANIFEST.MF bin
cd bin
jar cfm FCNavMapCreator.jar MANIFEST.MF *
copy FCNavMapCreator.jar ../dataExtractionOSM
cd ..

