import QtQuick 2.3
import "qrc:///qmlshared"

Item {
	id: root

	property alias title: qompVer.text

	anchors.fill: parent

	visible: opacity > 0
	focus: visible
	opacity: 0
	Behavior on opacity { NumberAnimation {} }

	Keys.onReleased: {
		if (event.key === Qt.Key_Back && visible) {
			event.accepted = true
			opacity = 0
		}
	}

	function open() {
		opacity = 1
	}

	Rectangle {
		id: fader

		anchors.fill: parent
		color: "black"
		opacity: 0.7
	}

	MouseArea {
		anchors.fill: parent
		onClicked: {}
	}

	Rectangle {
		id: content

		anchors.centerIn: parent
		width: parent.height > parent.width ? parent.width * 0.9 : parent.width * 0.7
		height: parent.height > parent.width ? parent.height * 0.7 : parent.height * 0.9
		radius: 8 * scaler.scaleMargins

		QompImage {
			id: img

			anchors.horizontalCenter: parent.horizontalCenter
			anchors.top: parent.top
			anchors.margins: 15 * scaler.scaleMargins
			height: parent.height * 0.4
			width: height
			source: "qrc:///icons/icons/qomp.png"
		}

		Text {
			id: qompVer

			anchors.top: img.bottom
			anchors.margins: 10 * scaler.scaleMargins
			anchors.horizontalCenter: parent.horizontalCenter
			font.pixelSize: 30 * scaler.scaleFont
		}

		Text {
			id: descr

			anchors.top: qompVer.bottom
			anchors.margins: 10 * scaler.scaleMargins
			anchors.horizontalCenter: parent.horizontalCenter
			width: parent.width * 0.9

			horizontalAlignment: Text.AlignHCenter
			font.pixelSize: 22 * scaler.scaleFont
			wrapMode: Text.WordWrap
			clip: true
			text: qsTr("Quick(Qt) Online Music Player\n"+
							  "one player for different online music hostings")
		}

		Text {
			id: home

			anchors.bottom: parent.bottom
			anchors.bottomMargin: 15 * scaler.scaleY
			anchors.horizontalCenter: parent.horizontalCenter
			font.pixelSize: 22 * scaler.scaleFont
			text: qsTr("<html><head/><body><p><a href=\"http://qomp.sourceforge.net/\">"+
					   "<span style=\" text-decoration: underline; color:#0000ff;\">qomp home page</span>"+
					   "</a></p></body></html>")

			onLinkActivated: Qt.openUrlExternally(link)
		}
	}
	onVisibleChanged: if(visible) descr.doLayout() //fix text word wrap
}
