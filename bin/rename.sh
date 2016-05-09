#rename tool
#!/bin/bash

APP=maven.app/Contents/MacOS/maven
mkdir maven.app/Contents/Frameworks
mkdir maven.app/Contents/plugins
mkdir maven.app/Contents/plugins/sqldrivers
cp *.csv *.db *.model maven.app/Contents/Resources
 

cp -R /Developer/Applications/Qt/plugins/sqldrivers/libqsqlite.dylib maven.app/Contents/plugins/sqldrivers/ 
cp /Library/Frameworks/QtCore.framework/Versions/4/QtCore  maven.app/Contents/Frameworks/QtCore
cp /Library/Frameworks/QtGui.framework/Versions/4/QtGui  maven.app/Contents/Frameworks/QtGui
cp /Library/Frameworks/QtSql.framework/Versions/4/QtSql  maven.app/Contents/Frameworks/QtSql
cp /Library/Frameworks/QtSvg.framework/Versions/4/QtSvg  maven.app/Contents/Frameworks/QtSvg
cp /Library/Frameworks/QtNetwork.framework/Versions/4/QtNetwork  maven.app/Contents/Frameworks/QtNetwork
cp /Library/Frameworks/QtXml.framework/Versions/4/QtXml  maven.app/Contents/Frameworks/QtXml
cp /Library/Frameworks/QtXmlPatterns.framework/Versions/4/QtXmlPatterns  maven.app/Contents/Frameworks/QtXmlPatterns

install_name_tool -change QtCore.framework/Versions/4/QtCore  @executable_path/../Frameworks/QtCore $APP

install_name_tool -change QtGui.framework/Versions/4/QtGui  @executable_path/../Frameworks/QtGui $APP
install_name_tool -change QtSql.framework/Versions/4/QtSql  @executable_path/../Frameworks/QtSql $APP
install_name_tool -change QtSvg.framework/Versions/4/QtSvg  @executable_path/../Frameworks/QtSvg $APP
install_name_tool -change QtNetwork.framework/Versions/4/QtNetwork  @executable_path/../Frameworks/QtNetwork $APP
install_name_tool -change QtXml.framework/Versions/4/QtXml  @executable_path/../Frameworks/QtXml $APP
install_name_tool -change QtXmlPatterns.framework/Versions/4/QtXmlPatterns  @executable_path/../Frameworks/QtXmlPatterns $APP


install_name_tool -id  @executable_path/../Frameworks/QtSql maven.app/Contents/plugins/sqldrivers/libqsqlite.dylib
install_name_tool -change QtSql.framework/Versions/4/QtSql  @executable_path/../Frameworks/QtSql maven.app/Contents/plugins/sqldrivers/libqsqlite.dylib
install_name_tool -change QtCore.framework/Versions/4/QtCore  @executable_path/../Frameworks/QtCore  maven.app/Contents/plugins/sqldrivers/libqsqlite.dylib 
perl rename.pl
