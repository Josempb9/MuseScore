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
import QtQuick.Layouts 1.15

import MuseScore.Ui 1.0
import MuseScore.UiComponents 1.0
import MuseScore.Palette 1.0

Rectangle {
    color: ui.theme.backgroundPrimaryColor

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 26
        anchors.rightMargin: 26

        spacing: 26

        Column {
            Layout.alignment: Qt.AlignVCenter

            spacing: 6

            StyledTextLabel {
                anchors.horizontalCenter: parent.horizontalCenter

                height: 20
                width: editDrumsetButton.width

                text: drumsetView.pitchName
            }

            FlatButton {
                id: editDrumsetButton

                text: qsTrc("palette", "Edit drumset")

                onClicked: {
                    drumsetView.editDrumset()
                }
            }
        }

        DrumsetPanelView {
            id: drumsetView

            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
