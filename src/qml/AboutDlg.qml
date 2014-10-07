import QtQuick 2.3
import QtQuick.Dialogs 1.2

MessageDialog {
	id: root

	title: "qomp"
	text:qsTr("Quick(Qt) Online Music Player\n"+
				  "Listening to the music from different music hostings")
	informativeText: qsTr("<html><head/><body><p><a href=\"http://qomp.sourceforge.net/\"><span style=\" text-decoration: underline; color:#0000ff;\">http://qomp.sourceforge.net/</span></a></p></body></html>")
}

