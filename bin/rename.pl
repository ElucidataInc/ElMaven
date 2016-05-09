 
my @list = ("QtCore", "QtGui", "QtXml", "QtXmlPatterns", "QtNetwork", "QtSvg", "QtSql");
foreach my $X (@list) { 
	foreach my $Y (@list) {
		system("install_name_tool -change $X.framework/Versions/4/$X \@executable_path/../Frameworks/$X   maven.app/Contents/Frameworks/$Y");
	}
}
