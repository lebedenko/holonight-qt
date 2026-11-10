// SPDX-License-Identifier: GPL-3.0-or-later
pragma Singleton
import QtQml

QtObject {
    function formatSize(bytes: real): string {
        if (bytes < 0)
            return "";
        if (bytes < 1024)
            return qsTr("%1 B").arg(bytes);
        const units = ["KB", "MB", "GB", "TB"];
        let value = bytes / 1024;
        let unit = 0;
        while (value >= 1024 && unit < units.length - 1) {
            value /= 1024;
            ++unit;
        }
        return qsTr("%1 %2").arg(value.toFixed(1)).arg(units[unit]);
    }
}
