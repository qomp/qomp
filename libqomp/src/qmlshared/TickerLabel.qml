import QtQuick 2.5

Rectangle {
	id: root

	property alias text: txt.text
	property alias font: txt.font
	property alias elide: txt.elide

	property bool runnning: false
	property int textOffset: 0
	property int speed: 100
	property int defaultLabelAlingment: Text.AlignHCenter

	clip: true

	Item {
		id: box

		anchors.fill: parent
		anchors.leftMargin: root.textOffset

		Text {
			id: txt
			height: parent.height
			verticalAlignment: Text.AlignVCenter

			onTextChanged: root.checkNeedAnim()
		}

		SequentialAnimation {
			id: anim

			property int delta: 0
			property int duration: 6000 + delta * root.speed
			property int easing: Easing.OutQuart

			running: root.runnning && delta > 0
			loops: Animation.Infinite

			PropertyAnimation {
				target: txt
				property: "x"
				from: 0
				to: -anim.delta
				duration: anim.duration / 2
				easing.type: anim.easing
			}
			PropertyAnimation {
				target: txt
				property: "x"
				from: -anim.delta
				to: 0
				duration: anim.duration / 2
				easing.type: anim.easing
			}
			onRunningChanged: {
				if(!running)
					txt.x = 0
			}
		}
	}

	Component.onCompleted: {
		checkNeedAnim()
	}

	function checkNeedAnim() {
		txt.anchors.horizontalCenter = undefined
		txt.anchors.horizontalCenterOffset = 0
		txt.x = 0

		var d = txt.width - box.width
		if(d > 0) {
			anim.delta = d
		}
		else {
			anim.delta = 0
			if(defaultLabelAlingment === Text.AlignHCenter) {
				txt.anchors.horizontalCenter = box.horizontalCenter
				txt.anchors.horizontalCenterOffset = -root.textOffset / 2
			}
		}
	}

	onWidthChanged: checkNeedAnim()
	onTextOffsetChanged: checkNeedAnim()
	onDefaultLabelAlingmentChanged: checkNeedAnim()
}
