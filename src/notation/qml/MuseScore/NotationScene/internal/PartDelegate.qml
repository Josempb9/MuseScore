/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import MuseScore.Ui 1.0
import MuseScore.UiComponents 1.0

ListItemBlank {
    id: root

    property string title: ""
    property int currentPartIndex: -1

    property bool isCreated: false

    property int sideMargin: 0

    signal copyPartRequested()
    signal removePartRequested()
    signal partClicked()
    signal titleEdited(string newTitle)
    signal titleEditingFinished()

    function startEditTitle() {
        if (titleLoader.sourceComponent !== editPartTitleField) {
            titleLoader.sourceComponent = editPartTitleField
        }
    }

    function endEditTitle() {
        if (titleLoader.sourceComponent !== partTitle) {
            titleLoader.sourceComponent = partTitle
            titleEditingFinished()
        }
    }

    height: 42

    onClicked: {
        endEditTitle()
        root.partClicked()
    }

    onDoubleClicked: {
        root.startEditTitle()
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: root.sideMargin - partIcon.width/3
        anchors.rightMargin: root.sideMargin

        spacing: 4

        StyledIconLabel {
            id: partIcon

            iconCode: IconCode.NEW_FILE
        }

        Loader {
            id: titleLoader

            Layout.fillWidth: true

            sourceComponent: partTitle

            Connections {
                target: root

                function onCurrentPartIndexChanged(currentPartIndex) {
                    root.endEditTitle()
                }
            }

            Component {
                id: partTitle

                StyledTextLabel {
                    text: root.title

                    horizontalAlignment: Qt.AlignLeft
                    font: ui.theme.bodyBoldFont
                }
            }

            Component {
                id: editPartTitleField

                TextInputField {
                    Component.onCompleted: {
                        forceActiveFocus()
                    }

                    currentText: root.title

                    onCurrentTextEdited: {
                        root.titleEdited(newTextValue)
                    }

                    onTextEditingFinished: {
                        Qt.callLater(root.endEditTitle)
                    }
                }
            }
        }

        MenuButton {
            menuModel: [
                { "id": "duplicate", "title": qsTrc("notation", "Duplicate"), "enabled": root.isCreated },
                { "id": "delete", "title": qsTrc("notation", "Delete"), "enabled": root.isCreated },
                { "id": "rename", "title": qsTrc("notation", "Rename") },
            ]

            onHandleMenuItem: {
                switch(item.id) {
                case "duplicate":
                    root.copyPartRequested()
                    break
                case "delete":
                    root.removePartRequested()
                    break
                case "rename":
                    root.startEditTitle()
                    break;
                }
            }
        }
    }

    SeparatorLine {
        anchors.bottom: parent.bottom
    }
}
