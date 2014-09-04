import QtQuick 2.3

Rectangle {
	id: root

	property int speed: 200
	property alias text: txt.text
	property alias font: txt.font
	property bool runnning: false
	property int textOffset: 0

	clip: true

	Text {
		id: txt
		height: parent.height
		verticalAlignment: Text.AlignVCenter

		onTextChanged: root.checkNeedAnim()
	}

	SequentialAnimation {
		id: anim

		property int delta: 0
		property int duration: delta * root.speed
		property int easing: Easing.OutQuart

		running: root.runnning && delta > 0
		loops: Animation.Infinite

		PropertyAnimation {
			target: txt
			property: "x"
			from: root.textOffset
			to: -anim.delta
			duration: anim.duration / 2
			easing.type: anim.easing
		}
		PropertyAnimation {
			target: txt
			property: "x"
			from: -anim.delta
			to: root.textOffset
			duration: anim.duration / 2
			easing.type: anim.easing
		}
		onRunningChanged: {
			if(!running)
				txt.x = root.textOffset
		}
	}

	Component.onCompleted: {
		checkNeedAnim()
	}

	function checkNeedAnim() {
		var d = txt.width - root.width
		if(d > 0) {
			anim.delta = d
			txt.anchors.horizontalCenter = undefined
		}
		else {
			anim.delta = 0
			txt.anchors.horizontalCenter = root.horizontalCenter
		}
	}

	onWidthChanged: checkNeedAnim()
}
