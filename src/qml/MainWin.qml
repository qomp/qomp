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
	signal actExit()

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
	property alias pluginsActions: openTunes.model

	property bool playing: false
	property bool busy: false
	property bool repeat: false

	color: "lightblue"

	Keys.onReleased: {
		if (event.key === Qt.Key_Back) {
			root.actExit()
			event.accepted
		}
		else if (event.key === Qt.Key_Menu) {
			root.doMainMenu()
			event.accepted
		}
	}

	ListView {
		id: playlist

		focus: true

		anchors.top: parent.top
		width: parent.width
		anchors.bottom: controls.top
		anchors.horizontalCenter: parent.horizontalCenter

		snapMode: ListView.SnapToItem
		boundsBehavior: Flickable.StopAtBounds
		highlightFollowsCurrentItem: true
		highlightMoveDuration: 0

		highlight: Rectangle { color: Qt.darker(root.color) }

		delegate: PlayListDelegate {
			busy: model.current && root.busy
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
		height: 150
		anchors.bottom: parent.bottom
		color: "SteelBlue"
		width: parent.width
		anchors.horizontalCenter: parent.horizontalCenter

		Item {
			id: posItem
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.right: parent.right
			height: 30
			anchors.margins: 10

			Text {
				id: curPosTxt
				height: parent.height
				anchors.left: parent.left
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignLeft
				width: 50
				font.pixelSize: 18
				color: "white"
			}

			Text {
				id: totalDurTxt
				height: parent.height
				anchors.right: parent.right
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignRight
				width: 50
				font.pixelSize: 18
				color: "white"
			}

			Slider {
				id: position

				anchors.verticalCenter: parent.verticalCenter
				anchors.left: curPosTxt.right
				anchors.right: totalDurTxt.left
				anchors.margins: 10

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
			anchors.margins: 10

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
					icon: "qrc:///icons/play"
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

	MainMenu {
		id: mainMenu

		repeatAll: root.repeat

		onExit: root.actExit()
		onOpen: root.actDoOpenMenu()
		onClear: root.actClearPlaylist()
		onLoadPlaylist: {
			fileDialog.title = qsTr("Select Playlist")
			fileDialog.selectFolder = false
			fileDialog.selectExisting = true
			fileDialog.onDialogAccepted = function() {
				root.actLoadPlaylist(fileDialog.fileUrl)
			}
			fileDialog.nameFilters = [(qsTr("qomp playlist (*.qomp)"))]
			fileDialog.open()
		}
		onSavePlaylist: {
			fileDialog.title = qsTr("Select Playlist")
			fileDialog.selectFolder = false
			fileDialog.selectExisting = false
			fileDialog.onDialogAccepted = function() {
				root.actSavePlaylist(fileDialog.fileUrl)
			}
			fileDialog.nameFilters = [(qsTr("qomp playlist (*.qomp)"))]
			fileDialog.open()
		}

		onRepeatAllChanged: root.actRepeat(repeatAll)
	}

	OpenTunesMenu {
		id: openTunes
	}

	TrackMenu {
		id: trackMenu

		onToggle: root.actToggle(playlist.currentIndex)
		onRemove: root.actRemove(playlist.currentIndex)
		onDownload: {
			fileDialog. forIndex = playlist.currentIndex
			fileDialog.title = qsTr("Select directory")
			fileDialog.selectFolder = true
			fileDialog.selectExisting = true
			fileDialog.onDialogAccepted = function() {
				root.actDownload(fileDialog.forIndex, fileDialog.folder)
			}
			fileDialog.nameFilters = []
			fileDialog.open()
		}

	}

	FileDialog {
		id: fileDialog

		property var onDialogAccepted
		property int forIndex

		onAccepted: fileDialog.onDialogAccepted()
		visible: false
	}

	function setPlayIcon(playing) {
		play.icon = playing ? "qrc:///icons/pause" : "qrc:///icons/play"
	}

	function doMainMenu() {
		mainMenu.popup()
	}

	function doOpenTunesMenu() {
		openTunes.popup()
	}

	function enshureItemVisible(index) {
		playlist.currentIndex = index
		playlist.positionViewAtIndex(index, ListView.Contain)
	}
}
