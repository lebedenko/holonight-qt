import QtQuick
import QtQuick.Controls as C
import QtQuick.Layouts
import org.kde.kirigami as K

C.ApplicationWindow {
    id: root
    width: 700
    height: 600
    visible: true
    property bool constrainContent: false
    function geometry(control, label) {
        return {width: control.width, implicitWidth: control.implicitWidth,
                height: control.height, implicitHeight: control.implicitHeight,
                indicatorWidth: control.indicator.width,
                nextLabelX: label.x, indicatorRight: control.x + control.indicator.x + control.indicator.width}
    }
    function report() { return JSON.stringify({
        checkbox: geometry(check, checkLabel), radio: geometry(radio, radioLabel),
        switchControl: geometry(toggle, toggleLabel),
        scroll: {width: scroll.width, availableWidth: scroll.availableWidth,
                 contentWidth: scroll.contentWidth, labelWidth: body.width,
                 textWidth: body.contentItem.width, lineCount: body.lineCount,
                 textMargin: body.textMargin, horizontalSize: scroll.C.ScrollBar.horizontal.size}
    }) }
    function paletteReport() { return JSON.stringify({
        applicationWindow: root.palette.window.toString(),
        fieldPaletteBase: field.palette.base.toString(), fieldBackground: field.background.color.toString(),
        kirigamiText: body.color.toString(), fieldText: field.color.toString()
    }) }
    Column {
        spacing: 12
        RowLayout {
            spacing: 6
            C.CheckBox { id: check; contentItem: null; padding: 0; spacing: 0 }
            C.Label { id: checkLabel; text: "Adjacent checkbox label" }
        }
        RowLayout {
            spacing: 6
            C.RadioButton { id: radio; contentItem: null; padding: 0; spacing: 0 }
            C.Label { id: radioLabel; text: "Adjacent radio label" }
        }
        RowLayout {
            spacing: 6
            C.Switch { id: toggle; contentItem: null; padding: 0; spacing: 0 }
            C.Label { id: toggleLabel; text: "Adjacent switch label" }
        }
        C.TextField { id: field; text: "Application palette probe" }
        Binding { target: scroll; property: "contentWidth"; value: scroll.availableWidth; when: root.constrainContent }
        Binding { target: body; property: "width"; value: scroll.availableWidth; when: root.constrainContent }
        C.ScrollView {
            id: scroll
            width: 400
            height: 180
            K.SelectableLabel {
                id: body
                text: "A long paragraph with ordinary words to measure wrapping and viewport containment. ".repeat(40)
                textMargin: K.Units.gridUnit
            }
        }
    }
}
