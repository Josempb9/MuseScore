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

import MuseScore.Ui 1.0
import MuseScore.UiComponents 1.0

StyledTabButton {
    id: root

    property alias contextMenuModel: contextMenuButton.menuModel

    signal handleContextMenuItemRequested(var item)

    height: 36
    width: implicitWidth

    leftPadding: 10
    rightPadding: 10

    contentItem: Row {
        spacing: 4

        StyledTextLabel {
            anchors.verticalCenter: parent.verticalCenter

            horizontalAlignment: Qt.AlignLeft
            font: root.isCurrent ? ui.theme.bodyBoldFont : ui.theme.bodyFont

            text: root.text
        }

        MenuButton {
            id: contextMenuButton

            height: 20
            width: height

            anchors.verticalCenter: parent.verticalCenter
            visible: root.isCurrent

            navigation.panel: root.navigation.panel
            navigation.order: root.navigation.order + 1

            onHandleMenuItem: function(item) {
                root.handleContextMenuItemRequested(item)
            }
        }
    }

    background: Rectangle {
        id: backgroundRect

        NavigationFocusBorder{
            navigationCtrl: root.navigation
            anchors.margins: 0
        }

        color: ui.theme.backgroundSecondaryColor
        opacity: 1

        SeparatorLine {
            anchors.right: parent.right

            orientation: Qt.Vertical
        }

        SeparatorLine {
            anchors.bottom: parent.bottom

            visible: !root.isCurrent
        }

        states: [
            State {
                name: "HOVERED"
                when: root.hovered && !root.isCurrent

                PropertyChanges {
                    target: backgroundRect

                    opacity: ui.theme.buttonOpacityHover
                    color: ui.theme.backgroundPrimaryColor
                }
            },

            State {
                name: "SELECTED"
                when: root.isCurrent

                PropertyChanges {
                    target: backgroundRect

                    color: ui.theme.backgroundPrimaryColor
                }
            }
        ]
    }

    states: []
}
