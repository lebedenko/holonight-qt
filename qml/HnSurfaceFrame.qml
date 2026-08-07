// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Shapes
import Holonight.Core

Item {
    id: root

    property int surfaceRole: HnSurfaceRole.Card
    property int cornerStyleOverride: HnCornerStyle.Inherit
    property real radiusOverride: NaN
    property real chamferOverride: NaN
    property int chamferedCornersOverride: HnCornerMask.Inherit
    property color fillColor: root.semanticFillColor(root.surfaceRole)
    property color borderColor: HoloniightPalette.borderPassive
    property real borderWidth: HnMetrics.borderWidth

    readonly property real normalizedBorderWidth: Math.max(0, root.borderWidth)
    readonly property real frameInset: root.normalizedBorderWidth / 2
    readonly property real drawableWidth: Math.max(0, root.width - root.normalizedBorderWidth)
    readonly property real drawableHeight: Math.max(0, root.height - root.normalizedBorderWidth)
    readonly property int appearanceRevision: HnAppearance.revision
    readonly property var resolvedShape: HnAppearance.resolve(root.surfaceRole, root.cornerStyleOverride,
                                                              root.drawableWidth, root.drawableHeight,
                                                              root.radiusOverride, root.chamferOverride,
                                                              root.chamferedCornersOverride,
                                                              root.appearanceRevision)
    readonly property int shapeKind: root.resolvedShape.kind
    readonly property real effectiveRadius: root.resolvedShape.radius
    readonly property real effectiveChamfer: root.resolvedShape.chamfer
    readonly property int chamferedCorners: root.resolvedShape.corners
    readonly property string pathData: root.buildPath()

    default property alias contentData: contentItem.data

    function semanticFillColor(role) {
        switch (role) {
        case HnSurfaceRole.Window:
            return HoloniightPalette.background
        case HnSurfaceRole.Panel:
        case HnSurfaceRole.Popup:
        case HnSurfaceRole.Menu:
        case HnSurfaceRole.Pill:
        case HnSurfaceRole.Tooltip:
            return HoloniightPalette.surfaceRaised
        case HnSurfaceRole.Card:
        case HnSurfaceRole.Hud:
        case HnSurfaceRole.WorkspaceIndicator:
            return HoloniightPalette.surfaceElevated
        case HnSurfaceRole.Control:
        default:
            return HoloniightPalette.surface
        }
    }

    function hasChamfer(corner) {
        return Boolean(root.chamferedCorners & corner)
    }

    function buildPath() {
        if (root.drawableWidth <= 0 || root.drawableHeight <= 0) {
            return ""
        }

        const left = root.frameInset
        const top = root.frameInset
        const right = root.width - root.frameInset
        const bottom = root.height - root.frameInset
        const radius = root.effectiveRadius
        const chamfer = root.effectiveChamfer
        const topLeft = root.hasChamfer(HnCornerMask.TopLeft)
        const topRight = root.hasChamfer(HnCornerMask.TopRight)
        const bottomRight = root.hasChamfer(HnCornerMask.BottomRight)
        const bottomLeft = root.hasChamfer(HnCornerMask.BottomLeft)

        let path = `M ${left + (topLeft ? chamfer : radius)} ${top}`
        path += ` L ${right - (topRight ? chamfer : radius)} ${top}`
        path += topRight ? ` L ${right} ${top + chamfer}`
                             : ` Q ${right} ${top} ${right} ${top + radius}`
        path += ` L ${right} ${bottom - (bottomRight ? chamfer : radius)}`
        path += bottomRight ? ` L ${right - chamfer} ${bottom}`
                                : ` Q ${right} ${bottom} ${right - radius} ${bottom}`
        path += ` L ${left + (bottomLeft ? chamfer : radius)} ${bottom}`
        path += bottomLeft ? ` L ${left} ${bottom - chamfer}`
                               : ` Q ${left} ${bottom} ${left} ${bottom - radius}`
        path += ` L ${left} ${top + (topLeft ? chamfer : radius)}`
        path += topLeft ? ` L ${left + chamfer} ${top}`
                            : ` Q ${left} ${top} ${left + radius} ${top}`
        return path + " Z"
    }

    Shape {
        anchors.fill: parent
        preferredRendererType: Shape.CurveRenderer
        Accessible.ignored: true

        ShapePath {
            fillColor: root.fillColor
            strokeColor: root.normalizedBorderWidth > 0 ? root.borderColor : "transparent"
            strokeWidth: root.normalizedBorderWidth
            joinStyle: ShapePath.RoundJoin

            PathSvg {
                path: root.pathData
            }
        }
    }

    Item {
        id: contentItem
        anchors.fill: parent
    }
}
