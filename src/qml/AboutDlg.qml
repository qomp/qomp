import QtQuick 2.9
import "qrc:///qmlshared"

QompBaseDialog {
	id: root

	property alias title: qompVer.text


	content: [

		QompImage {
			id: img

			anchors.horizontalCenter: parent.horizontalCenter
			anchors.top: parent.top
			anchors.margins: 15 * scaler.scaleMargins
			height: parent.height * 0.4
			width: height
			source: "qrc:///icons/icons/qomp.png"
		},

		Text {
			id: qompVer

			anchors.top: img.bottom
			anchors.margins: 10 * scaler.scaleMargins
			anchors.horizontalCenter: parent.horizontalCenter
			font.pixelSize: 40 * scaler.scaleFont
		},

		Text {
			id: descr

			anchors.top: qompVer.bottom
			anchors.margins: 10 * scaler.scaleMargins
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9

			horizontalAlignment: Text.AlignHCenter
			font.pixelSize: 30 * scaler.scaleFont
			wrapMode: Text.WordWrap
			clip: true
			text: qsTr("Quick(Qt) Online Music Player\n"+
							  "one player for different online music hostings")
		},

		Text {
			id: home

			anchors.bottom: parent.bottom
			anchors.bottomMargin: 15 * scaler.scaleY
			anchors.horizontalCenter: parent.horizontalCenter
			font.pixelSize: 26 * scaler.scaleFont
			text: qsTr("<html><head/><body><p><a href=\"http://qomp.sourceforge.net/\">"+
					   "<span style=\" text-decoration: underline; color:#0000ff;\">qomp home page</span>"+
					   "</a></p></body></html>")

			onLinkActivated: Qt.openUrlExternally(link)
		}
	]
	onVisibleChanged: if(visible) descr.doLayout() //fix text word wrap
}
