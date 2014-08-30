import QtQuick 2.3
import QtQuick.Controls 1.2
import "qrc:///qmlshared"

Rectangle {
	id: root

	property ListView list: ListView.view
	property bool busy: false
	property bool playing: false
	readonly property int animDuration: 100

	signal activated()
	signal longTap()

	width: parent.width
	height: 100 * scaler.scaleY

	color: "transparent"

	Text {
		id: mainText

		height: parent.height / 2
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: 5 * scaler.scaleMargins
		anchors.leftMargin: 15 * scaler.scaleMargins

		text: {
			var txt = model.title
			if (txt === undefined || txt.length === 0)
					txt = model.url
			if(txt === undefined || txt.length === 0)
					txt = model.file
			if(txt === undefined)
				txt = ""

			return txt
		}

		clip: true
		elide: Text.ElideMiddle
		font.bold: model.current === undefined ? false : model.current
		font.pixelSize: parent.height / 4
		horizontalAlignment: Text.AlignLeft
		verticalAlignment: Text.AlignVCenter
	}

	Text {
		id: addText

		anchors.bottom: parent.bottom
		anchors.top: mainText.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: mainText.anchors.margins
		anchors.leftMargin: mainText.anchors.leftMargin

		text: model.artist + '  ' + model.duration
		horizontalAlignment: Text.AlignLeft
		verticalAlignment: Text.AlignTop
		clip: true
		font.pixelSize: parent.height / 5.5
	}

	QompImage {
		id: image

		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		height: root.height * 0.7
		width: height
		source: "qrc:///icons/tune"
		opacity: 0

		Behavior on opacity {
			PropertyAnimation {
				duration: root.animDuration
			}
		}
	}

	BusyIndicator {
		id: busy

		opacity: 0
		visible: opacity > 0
		//running: visible
		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		height: root.height * 0.7
		width: height

		Behavior on opacity {
			PropertyAnimation {
				duration: root.animDuration
			}
		}
	}


	Rectangle {
		anchors.bottom: parent.bottom
		width: parent.width
		border.width: 1
		height: 1
	}

	MouseArea {
		anchors.fill: parent
		onClicked: list.currentIndex = index
		onDoubleClicked: root.activated()
		onPressAndHold: {
			list.currentIndex = index
			root.longTap()
		}
	}

	states: [
		State {
			name: "playing"
			when: root.playing == true && root.busy === false

			PropertyChanges {
				target: image
				opacity: 1
			}

			PropertyChanges {
				target: busy
				opacity: 0
			}
		},

		State {
			name: "busy"
			when: root.busy === true

			PropertyChanges {
				target: image
				opacity: 0
			}

			PropertyChanges {
				target: busy
				opacity: 1
			}
		},

		State {
			name: "normal"
			when: root.busy === false && root.playing === false

			PropertyChanges {
				target: image
				opacity: 0
			}

			PropertyChanges {
				target: busy
				opacity: 0
			}
		}
	]
}
