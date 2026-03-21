import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Basic

Dialog {
    id: settingsDialog
    modal: true
    title: ""
    anchors.centerIn: parent
    width: 480
    height: 340
    padding: 0
    standardButtons: Dialog.NoButton
    closePolicy: Dialog.CloseOnEscape | Dialog.CloseOnPressOutside

    background: Rectangle {
        color: "#ffffff"
        radius: 10
        border.width: 1
        border.color: "#e5e7eb"
    }

    // Load current key when dialog opens
    onOpened: {
        if (settingsController) {
            var key = settingsController.getSetting("gemini_api_key", "");
            apiKeyField.text = key;
        }
        saveStatus.visible = false;
    }

    contentItem: Item {
        width: parent.width
        height: parent.height

        Column {
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                margins: 28
            }
            spacing: 0

            // Header row
            Row {
                width: parent.width
                spacing: 10

                Image {
                    width: 22
                    height: 22
                    source: "qrc:/icons/ic-preference.svg"
                    sourceSize.width: 22
                    sourceSize.height: 22
                    anchors.verticalCenter: parent.verticalCenter
                }

                Label {
                    text: "Settings"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#1f2937"
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Item { width: 1; height: 20 }

            // Section label
            Label {
                text: "SchedBot — Gemini API Key"
                font.pixelSize: 13
                font.bold: true
                color: "#374151"
            }

            Item { width: 1; height: 8 }

            Label {
                text: "SchedBot uses the Gemini API to filter schedules by natural language. Enter your API key below."
                font.pixelSize: 13
                color: "#6b7280"
                wrapMode: Text.WordWrap
                width: parent.width
            }

            Item { width: 1; height: 4 }

            Label {
                text: "Get a free API key at aistudio.google.com"
                font.pixelSize: 12
                color: "#3b82f6"
                width: parent.width
            }

            Item { width: 1; height: 14 }

            // API key field
            Rectangle {
                width: parent.width
                height: 40
                radius: 6
                border.width: apiKeyField.activeFocus ? 2 : 1
                border.color: apiKeyField.activeFocus ? "#3b82f6" : "#d1d5db"
                color: "#ffffff"

                TextField {
                    id: apiKeyField
                    anchors.fill: parent
                    anchors.margins: 1
                    placeholderText: "sk-ant-..."
                    echoMode: TextInput.Password
                    font.pixelSize: 14
                    color: "#1f2937"
                    background: Rectangle { color: "transparent" }
                    leftPadding: 10
                    rightPadding: 10
                    verticalAlignment: TextInput.AlignVCenter
                }
            }

            Item { width: 1; height: 10 }

            // Save status
            Label {
                id: saveStatus
                visible: false
                text: "✓ API key saved"
                color: "#10b981"
                font.pixelSize: 13
            }
        }

        // Buttons row
        Row {
            anchors {
                bottom: parent.bottom
                bottomMargin: 24
                right: parent.right
                rightMargin: 28
            }
            spacing: 10

            Button {
                width: 80
                height: 36

                background: Rectangle {
                    radius: 6
                    color: cancelBtnArea.containsMouse ? "#f3f4f6" : "#ffffff"
                    border.width: 1
                    border.color: "#d1d5db"
                }

                contentItem: Text {
                    text: "Cancel"
                    color: "#374151"
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                MouseArea {
                    id: cancelBtnArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: settingsDialog.close()
                }
            }

            Button {
                width: 80
                height: 36

                background: Rectangle {
                    radius: 6
                    color: saveBtnArea.containsMouse ? "#35455c" : "#1f2937"
                }

                contentItem: Text {
                    text: "Save"
                    color: "#ffffff"
                    font.pixelSize: 14
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                MouseArea {
                    id: saveBtnArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        if (settingsController) {
                            var ok = settingsController.setSetting("gemini_api_key", apiKeyField.text.trim());
                            if (ok) {
                                saveStatus.text = "✓ API key saved";
                                saveStatus.color = "#10b981";
                            } else {
                                saveStatus.text = "⚠ Failed to save. Try again.";
                                saveStatus.color = "#dc2626";
                            }
                            saveStatus.visible = true;
                        }
                    }
                }
            }
        }
    }
}
