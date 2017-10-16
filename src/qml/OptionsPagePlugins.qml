import QtQuick 2.9
import "qrc:///qmlshared"

Item {
	id: root

	height: col.height
	Column {
		id: col
		spacing: 0

		onChildrenChanged: {
			if(children.length > 0) {
				height += children[children.length - 1].height
			}
		}
		width: parent.width
	}

	Component {
		id: creator

		OptionsEntry {
			property alias enabled: enbl.checked
			property alias name: name.text
			property alias version: ver.text
			property alias description: descr.text

			QompCheckBox {
				id: enbl

				checkBoxHeight: parent.height / 2
				anchors.left: parent.left
			}

			Item {
				id: txt
				anchors.fill: parent
				anchors.leftMargin: enbl.width
				clip: true

				Column {
					width: parent.width
					anchors.verticalCenter: parent.verticalCenter

					Row {
						spacing: 10 * scaler.scaleX

						Text {
							id: name
							font.pointSize: 18
							verticalAlignment: Text.AlignBottom
						}
						Text {
							id: ver
							font.pointSize: name.font.pointSize
							verticalAlignment: name.verticalAlignment
						}
					}

					Text {
						id: descr
						font.pointSize: 15
						width: parent.width
						verticalAlignment: Text.AlignTop
						wrapMode: Text.WordWrap
					}
				}
			}
		}
	}

	function addPlugin(name, descr, version, enabled) {
		creator.createObject(col, {"enabled": enabled, "name": name,
								 "version": version, "description": descr})
	}

	function clearPlugins() {
		col.children = []
		col.height = 0;
	}

	function getSettings() {
		var list = []
		for(var i = 0; i < col.children.length; ++i) {
			list[i] = {"name": col.children[i].name, "enabled": col.children[i].enabled}
		}
		return list;
	}
}
