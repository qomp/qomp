import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import "qrc:///qmlshared"

Rectangle {
	id: root

	signal actNext()
	signal actPrev()
	signal actStop()
	signal actPlay()
	signal actRepeat(var rep)
	signal positionChanged(var pos)

	signal actClearPlaylist()
	signal actLoadPlaylist(var str)
	signal actSavePlaylist(var str)
	signal actDoOpenMenu()
	signal actDoOptions()

	signal itemActivated(var index)

	signal actToggle(var index)
	signal actRemove(var index)
	signal actDownload(var index, var dir)

	property alias currentDuration: position.maximumValue
	property alias currentPosition: position.value
	property alias currentDurationText: totalDurTxt.text
	property alias currentPositionText: curPosTxt.text
	property alias currentFolder:  fileDialog.folder
	property alias playlistModel: playlist.model
	property alias pluginsActions: sideBar.model

	property bool playing: false
	property bool busy: false
	property bool repeat: false
	property string totalDuration;

	readonly property bool active: Stack.status === Stack.Active

	color: "lightblue"

	Keys.onReleased: {
		if (event.key === Qt.Key_Menu) {
			event.accepted = true
			menuButton.expanded = true
		}
		if (event.key === Qt.Key_Back && menuButton.expanded ) {
			event.accepted = true
			menuButton.expanded = false
		}
	}

	PageTitle {
		id: title
		text: "qomp"

		QompMenuButton {
			id: menuButton
			height: title.height
			anchors.left: title.left
			anchors.top: title.top
		}
	}

	ListView {
		id: playlist

		focus: true

		anchors.top: title.bottom
		width: parent.width
		anchors.bottom: controls.top
		anchors.horizontalCenter: parent.horizontalCenter

		snapMode: ListView.SnapToItem
		clip: true
		boundsBehavior: Flickable.StopAtBounds
		highlightFollowsCurrentItem: true
		highlightMoveDuration: 0

		highlight: Rectangle { color: "#68828A" }

		delegate: PlayListDelegate {
			busy: model.current && root.busy && root.active
			playing: model.current && root.playing
			onLongTap: {
				trackMenu.canDownload = false //Temporary disable
				trackMenu.popup()
			}
			onActivated: root.itemActivated(index)
		}
	}

	Rectangle {
		id: controls
		height: 150 * scaler.scaleY
		anchors.bottom: parent.bottom
		color: "SteelBlue"
		width: parent.width
		anchors.horizontalCenter: parent.horizontalCenter

		Item {
			id: posItem
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.right: parent.right
			height: controls.height / 5
			anchors.margins: 10 * scaler.scaleMargins

			Text {
				id: curPosTxt
				height: parent.height
				anchors.left: parent.left
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignLeft
				width: 60 * scaler.scaleX
				font.pixelSize: parent.height / 1.5
				color: "white"
			}

			Text {
				id: totalDurTxt
				height: parent.height
				anchors.right: parent.right
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignRight
				width: curPosTxt.width
				font.pixelSize: curPosTxt.font.pixelSize
				color: "white"
			}

			Slider {
				id: position

				anchors.verticalCenter: parent.verticalCenter
				anchors.left: curPosTxt.right
				anchors.right: totalDurTxt.left
				anchors.margins: 10 * scaler.scaleMargins

				onValueChanged: {
					if (pressed)
						root.positionChanged(Math.round(value))
				}
			}
		}

		Item {
			anchors.top: posItem.bottom
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.bottom: parent.bottom
			anchors.margins: 10 * scaler.scaleMargins

			Row {
				anchors.horizontalCenter: parent.horizontalCenter
				height: parent.height

				QompToolButton {
					id: prev
					icon: "qrc:///icons/prev"
					anchors.verticalCenter: parent.verticalCenter
					onClicked: root.actPrev()
				}
				QompToolButton {
					id: play
					icon:root.playing ? "qrc:///icons/pause" : "qrc:///icons/play"
					anchors.verticalCenter: parent.verticalCenter
					onClicked: root.actPlay()
				}
				QompToolButton {
					id: stop
					icon: "qrc:///icons/stop"
					anchors.verticalCenter: parent.verticalCenter
					onClicked: root.actStop()
				}
				QompToolButton {
					id: next
					icon: "qrc:///icons/next"
					anchors.verticalCenter: parent.verticalCenter
					onClicked: root.actNext()
				}
			}
		}
	}

	MainWinSideBar {
		id: sideBar

		expanded: menuButton.expanded
		anchors.top: title.bottom
		anchors.bottom: parent.bottom

		repeatAll: root.repeat

		onOpen:  {
			menuButton.expanded = false
		}
		onClear: root.actClearPlaylist()
		onOptions: {
			menuButton.expanded = false
			root.actDoOptions()
		}
		onRepeatAllChanged: root.actRepeat(repeatAll)

//		onLoadPlaylist: {
//			fileDialog.title = qsTr("Select Playlist")
//			fileDialog.selectFolder = false
//			fileDialog.selectExisting = true
//			fileDialog.onDialogAccepted = function() {
//				root.actLoadPlaylist(fileDialog.fileUrl)
//			}
//			fileDialog.nameFilters = [(qsTr("qomp playlist (*.qomp)"))]
//			fileDialog.open()
//		}
//		onSavePlaylist: {
//			fileDialog.title = qsTr("Select Playlist")
//			fileDialog.selectFolder = false
//			fileDialog.selectExisting = false
//			fileDialog.onDialogAccepted = function() {
//				root.actSavePlaylist(fileDialog.fileUrl)
//			}
//			fileDialog.nameFilters = [(qsTr("qomp playlist (*.qomp)"))]
//			fileDialog.open()
//		}
	}

//	TrackMenu {
//		id: trackMenu

//		onToggle: root.actToggle(playlist.currentIndex)
//		onRemove: root.actRemove(playlist.currentIndex)
//		onDownload: {
//			fileDialog. forIndex = playlist.currentIndex
//			fileDialog.title = qsTr("Select directory")
//			fileDialog.selectFolder = true
//			fileDialog.selectExisting = true
//			fileDialog.onDialogAccepted = function() {
//				root.actDownload(fileDialog.forIndex, fileDialog.folder)
//			}
//			fileDialog.nameFilters = []
//			fileDialog.open()
//		}

//	}

	FileDialog {
		id: fileDialog

		property var onDialogAccepted
		property int forIndex

		onAccepted: fileDialog.onDialogAccepted()
		visible: false
	}

	function enshureItemVisible(index) {
		playlist.currentIndex = index
		playlist.positionViewAtIndex(index, ListView.Contain)
	}
}
