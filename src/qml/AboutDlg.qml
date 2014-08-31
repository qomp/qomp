import QtQuick 2.3
import QtQuick.Dialogs 1.2

MessageDialog {
	id: root

	title: "qomp"
	text:qsTr("Quick(Qt) Online Music Player\n"+
				  "Listening to the music from different music hostings")
	informativeText: qsTr("<html><head/><body><p><a href=\"http://code.google.com/p/qomp/\"><span style=\" text-decoration: underline; color:#0000ff;\">http://code.google.com/p/qomp/</span></a></p></body></html>")
}

